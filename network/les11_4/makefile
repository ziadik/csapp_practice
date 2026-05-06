CC = gcc
CFLAGS = -Wall -O2
LIBS = -lws2_32
TARGET = hostinfo.exe
SOURCES = hostinfo.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET) ziidik.ru

help:
	@echo "Usage:"
	@echo "  make        - compile program"
	@echo "  make run    - compile and run with google.com"
	@echo "  make clean  - remove executable"