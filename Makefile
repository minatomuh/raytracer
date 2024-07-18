CC = g++
CFLAGS = -Wall -Wextra -pedantic -std=c++20 -O2 -MMD -MP
EXEC = raytracer.exe

SRC_DIR = ./src
INCLUDE_DIR = ./include
SHAPE_DIR = $(SRC_DIR)/shape

SRCS = $(SRC_DIR)/main.cpp \
       $(SRC_DIR)/Ray.cpp \
       $(SRC_DIR)/RGB.cpp \
       $(SRC_DIR)/SceneBuilder.cpp \
       $(SRC_DIR)/Vector.cpp \
       $(SHAPE_DIR)/Object.cpp \
       $(SHAPE_DIR)/Mesh.cpp \
       $(SHAPE_DIR)/Sphere.cpp \
       $(SHAPE_DIR)/Triangle.cpp \
       $(INCLUDE_DIR)/tinyxml2.cpp

OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

-include $(DEPS)

%.o: %.cpp
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(EXEC) $(OBJS) $(DEPS)