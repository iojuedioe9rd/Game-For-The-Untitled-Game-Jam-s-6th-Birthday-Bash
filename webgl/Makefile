# Directories
BASE_DIR   := ..
GAME_DIR   := $(BASE_DIR)/Game
ENGINE_DIR := $(BASE_DIR)/Engine
BUILD_DIR  := build
DIST_DIR   := dist
OUT_BASE   := index

# Compiler and flags
CXX        	 := emcc
CXX_LIBS_DIR := -L"$(ENGINE_DIR)/vendor/SDL3/Web"
CXX_LIBS     := -lSDL3
CXX_INCLUDES := -I"$(ENGINE_DIR)/vendor/SDL3/include" -I"$(ENGINE_DIR)"
CXXFLAGS     := -O1 -msimd128 -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 \
                -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1 -s FULL_ES3=1 \
                -Wall -I"$(ENGINE_DIR)/src" $(CXX_LIBS_DIR) $(CXX_LIBS) $(CXX_INCLUDES)

# Source files
SRC_CPP := $(shell find $(GAME_DIR)/src -name "*.cpp") \
           $(shell find $(ENGINE_DIR)/src -name "*.cpp") \
           $(shell find $(ENGINE_DIR)/Platform/WebGL -name "*.cpp") \
           $(shell find $(ENGINE_DIR)/Platform/OpenGL -name "*.cpp")



SRC_C := 

# Object files
OBJ_CPP := $(patsubst $(BASE_DIR)/%, $(BUILD_DIR)/%, $(SRC_CPP:.cpp=.o))
OBJ_INL := $(patsubst $(BASE_DIR)/%, $(BUILD_DIR)/%, $(SRC_INL:.inl=.o))
OBJ_C   := $(patsubst $(BASE_DIR)/%, $(BUILD_DIR)/%, $(SRC_C:.c=.o))

OBJ := $(OBJ_CPP) $(OBJ_C)

# --- Targets ----------------------------------------------------------------

.PHONY: all clean print

# Link into HTML + JS + WASM
$(DIST_DIR)/$(OUT_BASE).html: $(OBJ)
	@mkdir -p $(DIST_DIR)
	$(CXX) $(CXXFLAGS) $^ -std=c++17 -Wl,--whole-archive -Wl,--no-whole-archive -o $@ --preload-file assets

# Compile rules
$(BUILD_DIR)/%.o: ../%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -std=c++17 -c $< -o $@

$(BUILD_DIR)/%.o: ../%.c
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Default: build HTML + JS + WASM
all: clean print $(DIST_DIR)/$(OUT_BASE).html

# Echo what’s being built
print:
	@echo "Sources:  $(SRC_CPP) $(SRC_C) $(SRC_INL)"
