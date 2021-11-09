CC = cc
CFLAGS  = -g -Wall
TARGET = todo

all : $(TARGET)

$(TARGET) : $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean :
	$(RM) $(TARGET)