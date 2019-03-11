LIBS= -lsfml-audio -lsfml-graphics -lsfml-system -lsfml-window
CCFLAGS = -Wall -Wextra -Wpedantic -O3
NAME_OF_EXEC=main
PATH_TO_ARKANOID=Tutorials/DiveIntoC++11/1_Arkanoid/
ARKANOID_EXEC=${PATH_TO_ARKANOID}p1.cpp

main: main.cpp
	g++ ${CCFLAGS} main.cpp -o ${NAME_OF_EXEC} ${LIBS}

arkanoid: ${ARKANOID_EXEC}
		g++ ${ARKANOID_EXEC} -o $@ ${LIBS}

all: main arkanoid

clean:
	rm -rf ${NAME_OF_EXEC} arkanoid
