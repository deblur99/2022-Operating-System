CC = gcc
TARGET = threadlab
OBJS = main.o threadpool.o 
CFLAGS = -Wall -g
LDFLAGS = -pthread 

all:$(TARGET)

$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)
