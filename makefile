CC=gcc
DIRS=bin obj
$(shell mkdir -p $(DIRS))

all: bin/server bin/client

bin/%: obj/%.o obj/utils.o
	$(CC) $^ -o $@

obj/%.o: src/%.c
	$(CC) -c -o $@ $< -Iinclude -Wall

clean:
	rm -rf bin/*
	rm -rf obj/*

