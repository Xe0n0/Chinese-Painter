CXX=g++
INCPATH=-I/usr/local/opt/opencv/include
LIBPATH=-L/usr/local/opt/opencv/lib -lstdc++ -lopencv_core -lopencv_highgui -lopencv_video -lopencv_imgproc

%.o:%.c
	g++ -o $@ $(INCPATH) $(LIBPATH) $^

generator: generator.o
	g++ -o generator generator.o $(INCPATH) $(LIBPATH)
clean:
	rm 0*.bmp
	rm 1*.bmp
	rm 2*.bmp
	rm 3*.bmp
	rm generator
	rm generator.o
