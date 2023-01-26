CC = gcc -g
CFLAGS = -O3 -Wall -Werror

OBJS = reader.o

reader: $(OBJS)
	$(CC) $(CFLAGS) -o reader $(OBJS)

reader.o: reader.c reader.h

format:
	clang-format --style=file -i *.c *.h