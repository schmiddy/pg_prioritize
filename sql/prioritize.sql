CREATE EXTENSION prioritize;

SELECT get_backend_priority(pg_backend_pid());
SELECT set_backend_priority(pg_backend_pid(), -5);
SET client_min_messages = warning;
SELECT set_backend_priority(pg_backend_pid(), 5);
RESET client_min_messages;
SELECT get_backend_priority(pg_backend_pid());

SELECT get_backend_priority(1);
SELECT set_backend_priority(1, 5);
