prioritize
=================
This module implements an interface to getpriority() and setpriority()
for PostgreSQL backends, callable from SQL functions. Essentially,
this module allows users to `renice' their backends.

Synopsis
--------

    SELECT get_backend_priority(pg_backend_pid());

    SELECT set_backend_priority(pg_backend_pid(), 10);

    SELECT set_backend_priority(pid, get_backend_priority(pid) + 5)
      FROM pg_stat_activity
      WHERE usename = CURRENT_USER;

Description
-----------
This module allows users to query and set the priority of PostgreSQL
backend processes. The priority values are used by getpriority()
and setpriority(), which you may be familiar with from the `nice' or `renice'
programs.


1) get_backend_priority(process_id integer)
----------------------------------------------
Returns the current priority of the selected backend. Any user may
query the priority of any other user's backend.

2) set_backend_priority(process_id integer, priority_value integer)
----------------------------------------------
Set the priority of the given backend, specified by its process ID.

Superusers are allowed to set the priority of any backends. Unprivileged
users are only allowed to set the priority of backends with the same
role.

Note, it is only possible to adjust the priority of a process upwards
(meaning, the process will run at a lower priority). This restriction
arises from [SUSv1](http://www.opengroup.org/sud/sud1/xsh/getpriority.htm),
which declares:

> Only a process with appropriate privileges can raise its own priority
> (that is, assign a lower numerical priority value).

and UNIX-like platforms take this to mandate that only root users may 
adjust priority values downwards. Your PostgreSQL processes will (hopefully!)
not be running under root, hence priority values may only be adjusted upwards.


Installation
-------
Installation should be a simple:

    $ make install
    $ CREATE EXTENSION prioritize;

Support
-------

This library is stored in an open [GitHub
repository](https://github.com/schmiddy/pg_prioritize).
Feel free to fork and contribute! Please file bug reports
via [GitHub Issues](http://github.com/schmiddy/pg_prioritize/issues/).


Author
------
[Josh Kupershmidt](mailto:schmiddy@gmail.com)


Copyright and License
---------------------

Copyright (c) Josh Kupershmidt

This module is free software; you can redistribute it and/or modify it under
the [PostgreSQL License](http://www.opensource.org/licenses/postgresql).

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose, without fee, and without a written agreement is
hereby granted, provided that the above copyright notice and this paragraph
and the following two paragraphs appear in all copies.

In no event shall Josh Kupershmidt be liable to any party for direct,
indirect, special, incidental, or consequential damages, including
lost profits, arising out of the use of this software and its documentation,
even if Josh Kupershmidt has been advised of the possibility of such damage.

Josh Kupershmidt specifically disclaims any warranties,
including, but not limited to, the implied warranties of merchantability and 
fitness for a particular purpose. The software provided hereunder is on an "as 
is" basis, and Josh Kupershmidt has no obligations to provide maintenance,
support, updates, enhancements, or modifications.
