CC = gcc
LD = gcc
CFLAGS = -Wall -Werror -O3 -ffast-math -march=native -fopenmp
LDFLAGS = -L/opt/X11/lib -lX11 -lm -fopenmp
INCLUDES = -I/opt/X11/include
RM = /bin/rm -f
OBJS = nim.o
EXECUTABLE = nim


all:$(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(LD) -o $(EXECUTABLE) $(OBJS) $(LDFLAGS)
	
pi.o: pi.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -std=c99

nim.o: nim.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c nim.c -std=c99

clean:
	$(RM) $(EXECUTABLE) *.o
