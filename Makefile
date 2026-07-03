CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
SRC_DIR := src
BUILD_DIR := build
TARGET := music_player

NIAUDIO_PRESENT := $(wildcard $(SRC_DIR)/miniaudio.h)

ALL_CPP := $(wildcard $(SRC_DIR)/*.cpp)

SRCS := $(ALL_CPP)
EXTRA_FLAGS := -DUSE_MINIAUDIO
EXTRA_LIBS := -lpthread -ldl -lm

OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(EXTRA_LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(EXTRA_FLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: all
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
