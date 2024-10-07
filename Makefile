# Load environment variables from .env
include .env

# Target executable name
TARGET_EXEC := vulkanGameEngine

# Build, source, and shader directories
BUILD_DIR := ./build
SRC_DIRS := ./src
SHADER_DIR := ./shaders

# Find all C and C++ source files
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')

# Generate object file names
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# Generate dependency file names
DEPS := $(OBJS:.o=.d)

# Find all shader files
SHADERS := $(shell find $(SHADER_DIR) -name '*.vert' -or -name '*.frag')

# Generate SPIR-V file names
SPVS := $(SHADERS:%=$(BUILD_DIR)/%.spv)

# Include directories
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Compiler flags
CXXFLAGS := -ggdb -std=c++20 -pedantic -Wall -Wextra -Werror
CPPFLAGS := $(INC_FLAGS) -MMD -MP

# Linker flags (including GLFW, Vulkan, and other libraries)
LDFLAGS := -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

# Build the final executable
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS) $(SPVS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Build rule for C source files
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Build rule for C++ source files
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# Compiles shader files into SPIR-V binaries
$(BUILD_DIR)/%.spv: %
	mkdir -p $(dir $@)
	${GLSLC} $< -o $@

.PHONY: test clean

# Runs the compiled executable
test: $(BUILD_DIR)/$(TARGET_EXEC)
	@echo "Running $(TARGET_EXEC)..."
	$(BUILD_DIR)/$(TARGET_EXEC)

clean:
	rm -r $(BUILD_DIR)

# Include dependency files
-include $(DEPS)
