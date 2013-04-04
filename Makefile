LOG4CXY = log4cxy

SRC_DIR = src
SOURCES = $(notdir $(wildcard $(SRC_DIR)/*.cpp))

OBJ_DIR = obj
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))

CXX_FLAGS = -MMD -pedantic -Wall -Werror -c -fmessage-length=0 -std=c++11

.PHONY: all clean

all: $(LOG4CXY)
clean:
	rm -rfv $(OBJ_DIR) $(LOG4CXY)

$(LOG4CXY): $(OBJ_DIR) $(addprefix $(OBJ_DIR)/,$(OBJECTS)) Makefile
	g++ -o $@ $(addprefix $(OBJ_DIR)/,$(OBJECTS))

VPATH = $(SRC_DIR)
$(OBJ_DIR)/%.o: %.cpp Makefile
	g++ $(CXX_FLAGS) $< -o $@

$(OBJ_DIR):
	mkdir -v $(OBJ_DIR)

include $(wildcard $(OBJ_DIR)/*.d)

