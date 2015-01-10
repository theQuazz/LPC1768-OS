CC = armcc
CFLAGS = -Wall -pedantic -MMD

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)

TEST_EXEC = bin/test

test: CC = clang
test: CFLAGS += -DSTATIC_HEAP -g
test: $(TEST_EXEC)

$(TEST_EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

-include $(DEP)

.PHONY: clean

clean:
	$(RM) $(OBJ) $(TEST_EXEC)
