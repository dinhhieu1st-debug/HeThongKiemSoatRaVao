CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iunity -Iinclude
SRCS = unity/unity.c \
       src/esp_sensors.c \
       src/esp_connectivity.c \
       test/test_sensor1.c \
       test/test_sensor2.c \
       test/test_connectivity.c \
       test/test_main.c

TARGET = run_esp_tests

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o

.PHONY: all test clean
