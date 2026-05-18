CC = g++
CFLAGS = -Wall -Wextra -std=c++17
LDFLAGS = $(shell pkg-config --libs sdl2)
CFLAGS += $(shell pkg-config --cflags sdl2)

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = .

SOURCES = $(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES)))
TARGET = $(BIN_DIR)/chip8

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean