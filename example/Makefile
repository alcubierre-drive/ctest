CC ?= gcc

SRC_C := $(shell find . -iname "*.c")
SRC_CPP := $(shell find . -iname "*.cpp")

OBJ_C := $(patsubst %.c,%.c.o,$(SRC_C))
OBJ_CPP := $(patsubst %.cpp,%.cpp.o,$(SRC_CPP))

DEP_C := $(patsubst %.c,%.c.d,$(SRC_C))
DEP_CPP := $(patsubst %.cpp,%.cpp.d,$(SRC_CPP))

BIN := main.x

.PHONY: $(BIN) ctest_run.h

$(BIN): $(OBJ_C) $(OBJ_CPP)
	$(CC) $(LDFLAGS) $^ -o $@

CFLAGS += -MMD -Wall -Wextra -pedantic #-DCTEST_NO_FORK

main.c: ctest_run.h

ctest_run.h:
	./ctest_run.sh -C -i test/ -o ctest_run.h

%.c.o: %.c %.h Makefile
	$(CC) -c $< -o $@ $(CFLAGS)
%.c.o: %.c Makefile
	$(CC) -c $< -o $@ $(CFLAGS)

%.cpp.o: %.cpp %.h Makefile
	$(CXX) -c $< -o $@ $(CXXFLAGS)
%.cpp.o: %.cpp Makefile
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	-@$(RM) -rf $(shell find . -type f -name '*.o') $(shell find . -type f -name '*.d')
