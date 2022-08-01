CC = clang
CFLAGS = -Wall -Wextra

all: distr/hikreset.o src/main.c distr/scanner.o
	$(CC) $(CFLAGS) src/main.c distr/hikreset.o distr/scanner.o -l curl -o distr/hikreset.elf

distr/hikreset.o: src/hikreset.c src/hikreset.h
	$(CC) $(CFLAGS) src/hikreset.c -o distr/hikreset.o -c

distr/scanner.o: src/scanner.c src/scanner.h
	$(CC) $(CFLAGS) src/scanner.c -o distr/scanner.o -c

analyze: src/scanner.c src/scanner.h src/hikreset.c src/hikreset.h src/main.c
	clang $(CFLAGS) --analyze src/*
clean:
	rm -rf distr/*