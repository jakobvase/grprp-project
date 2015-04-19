compile: shader_curver.cpp
	gcc shader_curver.cpp -std=c++11 -lm -lglut -lGLU -lGL -lGLEW -lstdc++ -pthread -o shader_curver

debug: shader_curver.cpp
	gcc -g shader_curver.cpp -std=c++11 -lm -lglut -lGLU -lGL -lGLEW -lstdc++ -pthread -o shader_curver

mac: shader_curver.cpp
	gcc shader_curver.cpp -lm -framework GLUT -framework GLEW -framework OpenGL -lstdc++ -o shader_curver
