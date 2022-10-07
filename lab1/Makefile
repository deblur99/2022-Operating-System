CC = gcc
TARGET = main.out
OBJS = main.o input_data_handler.o input_validate.o
CFLAGS = -Wall -g
LDFLAGS = -lc

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

clean :
	rm -f $(TARGET) $(OBJS)
