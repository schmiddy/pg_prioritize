#include <unistd.h>
#include <sys/resource.h>
#include "postgres.h"
#include "fmgr.h"
#include "storage/proc.h"
#include "storage/procarray.h"


#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif


PG_FUNCTION_INFO_V1(set_backend_priority);
PG_FUNCTION_INFO_V1(get_backend_priority);


Datum
get_backend_priority(PG_FUNCTION_ARGS)
{
    int pid = PG_GETARG_INT32(0);
    int priority;

    if (!IsBackendPid(pid)) {
        ereport(WARNING,
                (errmsg("PID %d is not a PostgreSQL server process", pid)));
	PG_RETURN_NULL();
    }

    priority = getpriority(PRIO_PROCESS, pid);
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
    int pid      = PG_GETARG_INT32(0);
    int prio     = PG_GETARG_INT32(1);
    bool success = true;
    int old_prio;

    if (!superuser()) {
	/*
         * Since the user is not superuser, check for matching roles. Trust
         * that BackendPidGetProc will return NULL if the pid isn't valid,
         * even though the check for whether it's a backend process is below.
         * The IsBackendPid check can't be relied on as definitive even if it
         * was first. The process might end between successive checks
         * regardless of their order. There's no way to acquire a lock on an
         * arbitrary process to prevent that.
         */
        proc = BackendPidGetProc(pid);

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
	    ereport(ERROR,
                (errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
                 (errmsg("must be superuser to nice arbitrary backends"))));

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
	old_prio = getpriority(PRIO_PROCESS, pid);
	if (old_prio > prio)
	{
	    ereport(WARNING,
		    (errmsg("Not possible to lower a process's priority (currently %d)", old_prio)));
	    success = false;
	}
	else if (old_prio == prio) {
	    ereport(NOTICE,
		    (errmsg("Priority of backend %d remaining at %d", pid, prio)));
	    success = false;
	}
	else if (setpriority(PRIO_PROCESS, pid, prio) == 0)
		ereport(NOTICE,
			(errmsg("Changed priority of backend %d from %d to %d", pid,
				old_prio, getpriority(PRIO_PROCESS, pid))));
	else {
	    ereport(WARNING,
		    (errmsg("Error setting priority of %d", pid)));
	    success = false;
	}
    }

    PG_RETURN_BOOL(success);
}
