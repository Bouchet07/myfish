CC = g++
CFLAGS = -Wall -Wextra -flto -Ofast
SRC_DIR = ./src
TARGET = myfish

.PHONY: all debug clean

all:
	$(CC) $(CFLAGS) $(SRC_DIR)/main.cpp -o $(TARGET)

debug:
	$(CC) $(CFLAGS) -DDEBUG $(SRC_DIR)/main.cpp -o $(TARGET)

clean:
	del $(TARGET).exe
