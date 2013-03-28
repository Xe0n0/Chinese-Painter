CXXFLAGS = -framework GLUT -framework OpenGL
OBJS = main.o
all: $(OBJS)
	c++ $(CXXFLAGS) -o glutApp $(OBJS) 
