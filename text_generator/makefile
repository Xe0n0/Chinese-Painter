CXX=c++
CXXFLAG=-std=c++11
INCPATH=-I/usr/local/opt/opencv/include
LIBPATH=-L/usr/local/opt/opencv/lib -std=c++11 -stdlib=libc++ -lopencv_core -lopencv_highgui -lopencv_video -lopencv_imgproc

generator: generator.cpp
	$(CXX) $(INCPATH) $(LIBPATH) -o generator generator.cpp
clean:
	rm 0*.bmp
	rm 1*.bmp
	rm 2*.bmp
	rm 3*.bmp
	rm generator
	rm generator.o
