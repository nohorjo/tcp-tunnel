CC=gcc

all: bin/server bin/client

bin/%: obj/%.o
	$(CC) $^ -o $@

obj/%.o: src/%.c
	$(CC) -c -o $@ $<

clean:
	rm -rf bin/*
	rm -rf obj/*

