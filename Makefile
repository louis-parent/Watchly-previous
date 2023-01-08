COMPILER_OPTIONS=-Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wpedantic -Wconversion -Wsign-conversion -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion -Wformat=2 -Wimplicit-fallthrough -Weffc++ -ansi -pedantic -std=c++20
LINKER_OPTIONS=-lpthread

COMPILER=g++
TARGET=watchly.run

OBJECT_EXTENSION=.o
SOURCE_EXTENSION=.cpp
HEADER_EXTENSION=.hpp

SRC_DIRECTORY=src
BIN_DIRECTORY=bin

SOURCE_FILES=$(shell find "${SRC_DIRECTORY}" -type f -follow -name "*${SOURCE_EXTENSION}")
HEADER_FILES=$(shell find "${SRC_DIRECTORY}" -type f -follow -name "*${HEADER_EXTENSION}")
OBJECT_FILES=$(subst $(SRC_DIRECTORY),$(BIN_DIRECTORY),$(subst $(SOURCE_EXTENSION),$(OBJECT_EXTENSION),$(SOURCE_FILES)))

all: $(TARGET)
	@echo "Make done !"

$(TARGET): $(OBJECT_FILES)
	@echo "Linking object files into executable $@..."
	@$(COMPILER) -o $@ $^ $(LINKER_OPTIONS)

$(BIN_DIRECTORY)/%$(OBJECT_EXTENSION): $(SRC_DIRECTORY)/%$(SOURCE_EXTENSION) $(SRC_DIRECTORY)/%$(HEADER_EXTENSION)
	@echo "Compiling source $< ..."
	@mkdir -p $(dir $@)
	@$(COMPILER) -o $@ -c $< $(COMPILER_OPTIONS)
	
$(BIN_DIRECTORY)/%$(OBJECT_EXTENSION): $(SRC_DIRECTORY)/%$(SOURCE_EXTENSION)
	@echo "Compiling source $< ..."
	@mkdir -p $(dir $@)
	@$(COMPILER) -o $@ -c $< $(COMPILER_OPTIONS)

.PHONY: clean mrproper

clean:
	@echo "Cleaning object files..."
	@rm -rf $(BIN_DIRECTORY)
	@mkdir $(BIN_DIRECTORY)

mrproper: clean
	@echo "Cleaning target executable..."
	@rm -rf $(TARGET)
