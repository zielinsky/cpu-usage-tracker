CC = gcc -g
CFLAGS = -O3 -Wall -Werror

OBJS = reader.o cputracker.o analyzer.o

reader: $(OBJS)
	$(CC) $(CFLAGS) -o cputracker $(OBJS)

reader.o: reader.c reader.h cputracker.h
analyzer.o: analyzer.c analyzer.h cputracker.h
cputracker.o: cputracker.c reader.h cputracker.h 

format:
	clang-format --style=file -i *.c *.h