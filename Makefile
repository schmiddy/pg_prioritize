MODULES = prioritize
EXTENSION = $(MODULES)
EXTVERSION = 1.0
EXTSQL = $(MODULES)--$(EXTVERSION).sql

DATA_built = $(MODULES).sql
DATA = uninstall_$(MODULES).sql
DOCS = README.md
REGRESS = $(MODULES)

SQL_IN = $(MODULES).sql.in

USE_EXTENSION = $(shell pg_config --version | grep -qE " 8\.|9\.0" && echo no || echo yes)

ifeq ($(USE_EXTENSION),yes)
all: $(EXTSQL)

$(EXTSQL): $(EXTENSION).sql
	cp $< $@
DATA = $(EXTSQL)
EXTRA_CLEAN += $(EXTSQL)
endif

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
