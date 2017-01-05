LDLIBS = -lGL -lglut -lGLEW

OBJS = maze

all: HKMaze
	g++ main.cpp HKMaze.o ${LDLIBS} -o maze
	
HKMaze: 
	g++ -c HKMaze.cpp
	
clean:
	rm -rf *.o *.*~ ${OBJS}