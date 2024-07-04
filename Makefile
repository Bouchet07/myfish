CXX = g++
CFLAGS = -Wall -Wextra -flto -flto-partition=one -Ofast -MMD -MP -std=c++17
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

ifeq ($(PROFILE_GEN), 1)
	ADITIONAL_FLAGS += -fprofile-generate
endif

ifeq ($(PROFILE_USE), 1)
	ADITIONAL_FLAGS += -fprofile-use
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

BUILD_FLAGS := USE_POPCNT USE_CTZ USE_PEXT DEBUG USE_UTF8 NO_UTF8 NATIVE
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

.PHONY: clean cleanthis debug profile-build
clean:
	rm -rf $(BUILD_DIR_BASE) $(TARGET)

cleanthis:
	rm -rf $(BUILD_DIR) $(TARGET)

cleanthisobj:
	rm -rf $(BUILD_DIR)/*.o

debug:
	@echo $(SRC_FILES)
	@echo $(OBJ_FILES)
	@echo $(BUILD_DIR)
	@echo $(BUILD_DIR_FLAGS)

profile-build:
	$(MAKE) clean
	@echo ""
	@echo "Step 1/4. Building instrumented executable ..."
	@echo ""
	mkdir profile
	$(MAKE) PROFILE_GEN=1
	@echo ""
	@echo "Step 2/4. Running benchmark for pgo-build ..."
	@echo ""
	./$(TARGET) bench
	cp $(BUILD_DIR)/*.gcda ./profile/
	$(MAKE) clean
	@echo ""
	@echo "Step 3/4. Building optimized executable ..."
	@echo ""
	mkdir -p $(BUILD_DIR)
	cp ./profile/*.gcda $(BUILD_DIR)
	$(MAKE) PROFILE_USE=1
	@echo ""
	@echo "Step 4/4. Deleting profile data ..."
	@echo ""
	rm -rf profile
	rm -rf $(BUILD_DIR)/*.gcda
	

-include $(DEP_FILES)