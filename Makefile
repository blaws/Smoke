all: Smoke

Smoke: Smoke.c
	g++ Smoke.c -o Smoke -framework GLUT -framework OpenGL

clean:
	rm -f *~ Smoke
