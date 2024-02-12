CXX = g++
CFLAGS = -Wall -Wextra -flto -Ofast -MMD -MP 
SRC_DIR = ./src
BUILD_DIR = ./build
TARGET = myfish

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
HEADER_FILES := $(wildcard $(SRC_DIR)/*.h)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))
DEP_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.d,$(SRC_FILES))

ifeq ($(NO_POPCNT), 1)
    CFLAGS += -DNO_POPCNT 
endif

ifeq ($(NO_CTZ), 1)
    CFLAGS += -DNO_CTZ 
endif

ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG
endif

all: $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<"
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ_FILES)
	@echo "Linking $@"
	$(CXX) $(CFLAGS) $^ -o $@

.PHONY: clean debug
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

debug:
	@echo $(SRC_FILES)
	@echo $(OBJ_FILES)

-include $(DEP_FILES)