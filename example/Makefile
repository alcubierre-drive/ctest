SRC := $(shell find . -iname "*.c")
OBJ := $(patsubst %.c,%.c.o,$(SRC))

EXE := main.x

.PHONY: $(EXE) ctest_run.h

CFLAGS += -I.. -Wall -Wextra -pedantic #-DCTEST_NO_FORK

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o $(EXE)

main.c: ctest_run.h

ctest_run.h:
	../ctest_headers.sh -i test/ -o ctest_run.h

%.c.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

