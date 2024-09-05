include .env

CC = g++
CFLAGS = -ggdb -std=c++23 -pedantic -Wall -Wextra -Wconversion -Wsign-conversion -Weffc++ -I$(TINYOBJ_PATH)
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

# Creates list of spv files and sets as dependency
vertSources = $(shell find ./shaders -type f -name "*.vert")
vertObjFiles = $(patsubst %.vert, %.vert.spv, $(vertSources))
fragSources = $(shell find ./shaders -type f -name "*.frag")
fragObjFiles = $(patsubst %.frag, %.frag.spv, $(fragSources))

# Source files
CPP_FILES := $(wildcard *.cpp)

TARGET = n.out
$(TARGET): $(vertObjFiles) $(fragObjFiles)
$(TARGET): *.cpp *.hpp
	$(CC) $(CFLAGS) -o $(TARGET) *.cpp $(LDFLAGS)

# compile the shader targets
%.spv: %
	${GLSLC} $< -o $@

.PHONY: test clean

test: $(TARGET)
	./$(TARGET)

clean:
	$(RM) $(TARGET)
	$(RM) ./shaders/*.spv
