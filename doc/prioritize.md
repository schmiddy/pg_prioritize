prioritize 1.0.0
============

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
backend processes. The priority values being queried and set are
as reported by getpriority() and setpriority(), which you may know
in the form of the `nice' or `renice' commands.

Superuser are allowed to set the priority of any backends. Unprivileged
users are only allowed to set the priority of backends with the same
role.

Note, it is only possible to adjust the priority of a process upwards
(meaning, the process will run at a lower priority). This restriction
comes from setpriority(), which only lets root users adjust priority
values downwards.

History
-------
### v1.0.0 (2012-04-05)
  * Initial version



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
