.PHONY: all

all: scheduler.out

scheduler.out: scheduler.c
	gcc -g -o $@ $^

run: scheduler.out
	./scheduler.out 0 FIFO example_fifo.in
	./scheduler.out 1 FIFO example_fifo.in

.PHONY: clean
clean:
	rm -f *.out *.o
