CFLAGS = -std=gnu11 -Wall -Werror -Wextra -pthread
LDLIBS = -lrt

.PHONY: all clean

all: task1_a task1_b task2_a task2_b

clean:
	$(RM) task1_a task1_b task2_a task2_b

task1_a: task1_a.c
task1_b: task1_b.c
task2_a: task2_a.c
task2_b: task2_b.c