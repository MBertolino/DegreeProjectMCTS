CC = gcc
LD = gcc
CFLAGS = -Wall -Werror -O3 -ffast-math -march=native
LDFLAGS = -lm 
INCLUDES =
RM = /bin/rm -f
OBJS = players.o
EXECUTABLE = nim
STATS = nim_stats
OPT = nim_optmv

all: $(EXECUTABLE)
stats: $(STATS)
opt: $(OPT)


$(EXECUTABLE): nim.o $(OBJS)
	$(LD) -o $(EXECUTABLE) nim.o $(OBJS) $(LDFLAGS)

$(STATS): nim_stats.o $(OBJS)
	$(LD) -o $(STATS) nim_stats.o $(OBJS) $(LDFLAGS)

$(OPT): nim_optmv.o $(OBJS)
	$(LD) -o $(OPT) nim_optmv.o $(OBJS) $(LDFLAGS)

players.o: players.c players.h structs.h
	$(CC) $(CFLAGS) $(INCLUDES) -c players.c -std=c99

nim.o: nim.c players.c players.h structs.h
	$(CC) $(CFLAGS) $(INCLUDES) -c nim.c -std=c99

nim_stats.o: nim_stats.c players.c players.h structs.h
	$(CC) $(CFLAGS) $(INCLUDES) -c nim_stats.c -std=c99

nim_optmv.o: nim_optmv.c players.c players.h structs.h
	$(CC) $(CFLAGS) $(INCLUDES) -c nim_optmv.c -std=c99

clean:
	$(RM) $(EXECUTABLE) $(STATS) $(OPT) *.o
