LDFLAGS = -framework GLUT -framework OpenGL -lpng
offscreen: offscreen.cpp
	c++ $(LDFLAGS) -o offscreen offscreen.cpp
solar-system: solar-system.cpp
	c++ $(LDFLAGS) -o solar-system solar-system.cpp 
terrain: clean terrain.cpp terrain.h terrain_gen.py
	c++ $(LDFLAGS) -o terrain terrain.cpp
demo: demo.cpp
	c++ $(LDFLAGS) -g -o demo demo.cpp
clean:
	rm -f terrain offscreen solar-system
