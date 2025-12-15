CC = gcc
CFLAGS = -Wall
TARGET = hw2
SOURCE = main.c Init.c Scheduler.c Thread.c TestCase3.c TestCase2.c TestCase1.c ThreadQ.c

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)
