CREATE OR REPLACE FUNCTION set_backend_priority(integer, integer)
RETURNS boolean
AS '$libdir/prioritize.so', 'set_backend_priority' LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION get_backend_priority(integer)
RETURNS integer
AS '$libdir/prioritize.so', 'get_backend_priority' LANGUAGE C STRICT;
