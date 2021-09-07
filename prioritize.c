#include "postgres.h"

#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>

#include "fmgr.h"
#include "miscadmin.h"
#include "storage/proc.h"
#include "storage/procarray.h"


PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(set_backend_priority);
PG_FUNCTION_INFO_V1(get_backend_priority);

extern Datum get_backend_priority(PG_FUNCTION_ARGS);
extern Datum set_backend_priority(PG_FUNCTION_ARGS);

Datum
get_backend_priority(PG_FUNCTION_ARGS)
{
    int pid = PG_GETARG_INT32(0);
    int priority;
    int save_errno = errno;


    if (!IsBackendPid(pid)) {
        ereport(WARNING,
                (errmsg("PID %d is not a PostgreSQL server process", pid)));
	PG_RETURN_NULL();
    }

    errno = 0;
    priority = getpriority(PRIO_PROCESS, pid);
    if (priority == -1) {
	/* We need to check errno to determine whether an error has occurred
	   or if the priority of the process is just '-1'.
	*/
	if (errno == ESRCH || errno == EINVAL) {
	    errno = save_errno;
	    ereport(ERROR,
		    (errcode(ERRCODE_IO_ERROR),
		     (errmsg("getpriority() could not find the requested backend"))));
	}
    }
    errno = save_errno;
    PG_RETURN_INT32(priority);
}


/* Set the 'nice' priority of the given backend. The priority passed in should
 * typically be between 1 and 20, inclusive, since priorities may only
 * be adjusted upwards by non-root users. If the backend had been manually
 * set (by a root user) to a negative nice value, it may be possible to pass
 * in a greater-but-still-negative value as the new priority.
 */
Datum
set_backend_priority(PG_FUNCTION_ARGS)
{
    PGPROC *proc;
    int pid        = PG_GETARG_INT32(0);
    int prio       = PG_GETARG_INT32(1);
    int save_errno = errno;
    bool success   = true;

    if (pid == MyProcPid) {
	/* Quick check: if we are setting the priority of our own backend,
	 * skip permissions checks and chekcs of whether 'pid' is a valid
	 * backend.
	 */
    }
    else if (!superuser()) {
	/*
         * Since the user is not superuser, check for matching roles. Trust
         * that BackendPidGetProcWithLock will return NULL if the pid isn't
         * valid, even though the check for whether it's a backend process is
         * below. The IsBackendPid check can't be relied on as definitive even
         * if it was first. The process might end between successive checks
         * regardless of their order. There's no way to acquire a lock on an
         * arbitrary process to prevent that.
         *
         * We do take the ProcArrayLock manually instead of using
         * BackendPidGetProc, because we need to be sure that the contents of
         * proc don't change from under us while we're using them.
         */
        LWLockAcquire(ProcArrayLock, LW_SHARED);

        proc = BackendPidGetProcWithLock(pid);

	if (proc == NULL) {
	    /*
	     * This is just a warning so a loop-through-resultset will not
	     * abort if one backend terminated on its own during the run
	     */
	    ereport(WARNING,
		    (errmsg("PID %d is not a PostgreSQL server process", pid)));
	    success = false;
	}

	else if (proc->roleId != GetUserId())
            LWLockRelease(ProcArrayLock);
	    ereport(ERROR,
                (errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
                 (errmsg("must be superuser to nice arbitrary backends"))));
        LWLockRelease(ProcArrayLock);


	/* Otherwise, the backend PID is valid and our user is allowed
	 * to set its priority.
	 */
    }

    else if (!IsBackendPid(pid))
    {
        ereport(WARNING,
                (errmsg("PID %d is not a PostgreSQL server process", pid)));
	success = false;
    }

    if (success) {
	errno = 0;
	if (setpriority(PRIO_PROCESS, pid, prio) == 0) {
	    ereport(NOTICE,
		    (errmsg("Set priority of backend %d to %d", pid, prio)));
	}
	else {
	    if (errno == ESRCH || errno == EINVAL) {
		errno = save_errno;
		ereport(ERROR,
			(errcode(ERRCODE_IO_ERROR),
			 (errmsg("setpriority(): could not find the requested backend"))));
	    }
	    else {
		/* Assume EPERM or EACCES */
		ereport(WARNING,
			(errmsg("setpriority(): permission denied")));
		success = false;
	    }
	}
    }

    errno = save_errno;
    PG_RETURN_BOOL(success);
}
