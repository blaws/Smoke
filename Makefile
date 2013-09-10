all: Smoke

Smoke: Smoke.cpp
	g++ Smoke.cpp -o Smoke -framework GLUT -framework OpenGL

clean:
	rm -f *~ Smoke
