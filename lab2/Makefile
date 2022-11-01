CC = gcc
TARGET = dkuware
OBJS = dkuware.o utils.o
CFLAGS = -Wall -g
LDFLAGS = -lc -lpthread -lssl -lcrypto

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

clean :
	rm -f $(TARGET) $(OBJS)
