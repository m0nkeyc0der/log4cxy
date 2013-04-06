# ===========================
# Final binary
# ===========================
LOG4CXY = log4cxy


# ===========================
# Sources & objects
# ===========================
SRC_DIR = src
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

OBJ_DIR = obj
OBJECTS = $(addprefix $(OBJ_DIR)/,$(patsubst %.cpp,%.o,$(notdir $(SOURCES))))


# ===========================
# C++ compiler & linker flags
# ===========================
CXX_WARNINGS   = -pedantic -Wall -Werror
DEBUG_FLAGS    = -g -fno-omit-frame-pointer
OPTIMIZE_FLAGS = -O3

CXX_FLAGS      = -MMD -pthread -c -fmessage-length=0 -std=c++11 \
	         $(CXX_WARNINGS) \
	         $(DEBUG_FLAGS) \
	         $(OPTIMIZE_FLAGS)

LINK_FLAGS     = -pthread -lprofiler $(DEBUG_FLAGS)
# ===========================


# ===========================
# Make targets
# ===========================

.PHONY: all clean run

all: $(LOG4CXY)
clean:
	rm -rfv $(OBJ_DIR) $(LOG4CXY) *.log *.prof

run: $(LOG4CXY)
	./$(LOG4CXY)

$(LOG4CXY): $(OBJECTS) Makefile
	g++ $(LINK_FLAGS) -o $@ $(OBJECTS)

VPATH = $(SRC_DIR)
$(OBJ_DIR)/%.o: %.cpp Makefile
	@test -d $(OBJ_DIR) || mkdir -v $(OBJ_DIR)
	g++ $(CXX_FLAGS) $< -o $@

include $(wildcard $(OBJ_DIR)/*.d)
include profile.inc.mk
	