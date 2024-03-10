CXX = g++
CFLAGS = -Wall -Wextra -flto -Ofast -MMD -MP 
SRC_DIR = ./src
BUILD_DIR_BASE = ./build
TARGET = myfish

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
#SRC_FILES := ./src/bitboard.cpp ./src/uci.cpp ./src/attacks.cpp ./src/main.cpp
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

ifeq ($(UTF8), 1)
	CFLAGS += -DUTF8
endif

ifeq ($(NO_UTF8), 1)
	CFLAGS += -DNO_UTF8
endif

ifeq ($(BENCHMARK), 1)
	CFLAGS += -DBENCHMARK
endif

BUILD_FLAGS := NO_POPCNT NO_CTZ DEBUG UTF8 NO_UTF8 BENCHMARK
BUILD_DIR_FLAGS := $(foreach flag,$(BUILD_FLAGS),$(if $(filter 1,$(value $(flag))),$(flag).,_.))
BUILD_DIR_FLAGS := $(subst $(eval) ,,$(BUILD_DIR_FLAGS))
BUILD_DIR_FLAGS := $(patsubst %.,%,$(BUILD_DIR_FLAGS))

# Update BUILD_DIR to reflect flags
BUILD_DIR := $(BUILD_DIR_BASE)/$(BUILD_DIR_FLAGS)
# Update object file paths
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))

all: $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<"
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ_FILES)
	@echo "Linking $@"
	$(CXX) $(CFLAGS) $^ -o $@

.PHONY: clean cleanthis debug
clean:
	rm -rf $(BUILD_DIR_BASE) $(TARGET)

cleanthis:
	rm -rf $(BUILD_DIR) $(TARGET)

debug:
	@echo $(SRC_FILES)
	@echo $(OBJ_FILES)
	@echo $(BUILD_DIR)
	@echo $(BUILD_DIR_FLAGS)

-include $(DEP_FILES)