shader_c: shader_curver.cpp
	gcc shader_curver.cpp -std=c++11 -lm -lglut -lGLU -lGL -lGLEW -lstdc++ -pthread -o shader_curver

shader_debug: shader_curver.cpp
	gcc -g shader_curver.cpp -std=c++11 -lm -lglut -lGLU -lGL -lGLEW -lstdc++ -pthread -o shader_curver

cpu_c: cpu_curver.cpp
	gcc cpu_curver.cpp -std=c++11 -lm -lglut -lGLU -lGL -lstdc++ -pthread -o cpu_curver

cpu_debug: cpu_curver.cpp
	gcc -g cpu_curver.cpp -lm -lglut -lGLU -lGL -lstdc++ -o cpu_curver

mac: cpu_curver.cpp
	gcc cpu_curver.cpp -lm -framework GLUT -lGLEW -framework OpenGL -lstdc++ -o cpu_curver

clean:
	rm *.o cpu_curver shader_curver