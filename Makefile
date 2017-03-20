CC = gcc
LD = gcc
CFLAGS = -Wall -Werror -O3 -ffast-math -march=native
LDFLAGS = -lm 
INCLUDES =
RM = /bin/rm -f
OBJS = nim.o players.o
EXECUTABLE = nim


all:$(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(LD) -o $(EXECUTABLE) $(OBJS) $(LDFLAGS)

players.o: players.h structs.h
	$(CC) $(CFLAGS) $(INCLUDES) -c players.c -std=c99

nim.o: nim.c players.c players.h structs.h
	$(CC) $(CFLAGS) $(INCLUDES) -c nim.c -std=c99

clean:
	$(RM) $(EXECUTABLE) *.o
