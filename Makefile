CXX		  := g++
CXX_FLAGS := -Wall -Wextra -std=c++17 -ggdb3

PROFILE := valgrind
PROFILE_FILE := valgrind-out.txt
PROFILE_FLAGS := --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=$(PROFILE_FILE)

BIN		:= bin
SRC		:= src
INCLUDE	:= include
LIB		:= lib

LIBRARIES	:=
EXECUTABLE	:= ruby


all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)

profile:
	rm -rf $(PROFILE_FILE)
	$(PROFILE) $(PROFILE_FLAGS) ./$(BIN)/$(EXECUTABLE)

clean:
	rm -rf $(BIN)/*
	rm -rf $(PROFILE_FILE)
