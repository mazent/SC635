#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_EXTRA_CLEAN := versione.h

test.o: versione.h

versione.h:
	$(COMPONENT_PATH)/versione.sh

