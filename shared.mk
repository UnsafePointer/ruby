CXX_FLAGS := -Wall -Wextra -std=c++17 $(CXX_ARCH_SPECIFIC_FLAGS) `$(SYSROOT)/usr/bin/sdl2-config --cflags`

GLAD_BUILD_DIR := glad

PROFILE := valgrind
PROFILE_FILE := valgrind-out.txt
PROFILE_FLAGS := --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=$(PROFILE_FILE)

BIN		:= bin
SRC		:= src
INCLUDE	:= include
LIB		:= lib

LIBRARIES	:= `$(SYSROOT)/usr/bin/sdl2-config --libs` -ldl
EXECUTABLE	:= ruby


all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(GLAD_BUILD_DIR)/src/*.c:
	python -m glad --out-path=$(GLAD_BUILD_DIR) --api="gl=$(OPENGL_API)" --extensions="GL_KHR_debug" --generator="c"

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp $(GLAD_BUILD_DIR)/src/*.c
	$(CXX) $(CXX_FLAGS) $(PREPROCESSOR_MACROS) -I$(INCLUDE) -I$(GLAD_BUILD_DIR)/include -L$(LIB) $^ -o $@ $(LIBRARIES) $(SDL2_FLAGS)

profile:
	rm -rf $(PROFILE_FILE)
	$(PROFILE) $(PROFILE_FLAGS) ./$(BIN)/$(EXECUTABLE)

clean:
	rm -rf $(BIN)/*
	rm -rf $(PROFILE_FILE)
	rm -rf $(GLAD_BUILD_DIR)
