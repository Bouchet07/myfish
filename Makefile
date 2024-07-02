CXX = g++
CFLAGS = -Wall -Wextra -flto -Ofast -MMD -MP -march=native
ADITIONAL_FLAGS = 
SRC_DIR = ./src
BUILD_DIR_BASE = ./build
TARGET = myfish

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
#SRC_FILES := ./src/bitboard.cpp ./src/uci.cpp ./src/attacks.cpp ./src/main.cpp
HEADER_FILES := $(wildcard $(SRC_DIR)/*.h)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))
DEP_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.d,$(SRC_FILES))

ifeq ($(ARCH), bmi2)
	USE_POPCNT = 1
	USE_CTZ = 1
	USE_PEXT = 1
endif

ifeq ($(NATIVE), 1)
	ADITIONAL_FLAGS += -march=native
endif

ifeq ($(USE_POPCNT), 1)
    ADITIONAL_FLAGS += -DUSE_POPCNT -mpopcnt
endif

ifeq ($(USE_CTZ), 1)
    ADITIONAL_FLAGS += -DUSE_CTZ
endif

ifeq ($(USE_PEXT), 1)
	ADITIONAL_FLAGS += -DUSE_PEXT
endif

ifeq ($(DEBUG), 1)
	ADITIONAL_FLAGS += -DDEBUG
endif

ifeq ($(USE_UTF8), 1)
	ADITIONAL_FLAGS += -DUSE_UTF8
endif

ifeq ($(NO_UTF8), 1)
	ADITIONAL_FLAGS += -DNO_UTF8
endif

ifeq ($(BENCHMARK), 1)
	ADITIONAL_FLAGS += -DBENCHMARK
endif

BUILD_FLAGS := USE_POPCNT USE_CTZ USE_PEXT DEBUG USE_UTF8 NO_UTF8 BENCHMARK
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
ifeq ($(GDB), 1)
	$(CXX) -g $(ADITIONAL_FLAGS) -c $< -o $@
else
	$(CXX) $(CFLAGS) $(ADITIONAL_FLAGS) -c $< -o $@
endif

$(TARGET): $(OBJ_FILES)
	@echo "Linking $@"
ifeq ($(GDB), 1)
	$(CXX) -g $(ADITIONAL_FLAGS) $^ -o $@
else
	$(CXX) $(CFLAGS) $(ADITIONAL_FLAGS) $^ -o $@
endif
	cp $(TARGET) $(BUILD_DIR)

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