compile: topic_1.cpp
	gcc topic_1.cpp -lm -lglut -lGLU -lGL -lstdc++ -o topic_1

debug: topic_1.cpp
	gcc -g topic_1.cpp -lm -lglut -lGLU -lGL -lstdc++ -o topic_1

mac: curved_surface_calculator.cpp
	gcc curved_surface_calculator.cpp -lm -framework GLUT -framework OpenGL -lstdc++ -o curved_surface_calculator

mac2: topic_1.cpp
	gcc topic_1.cpp -lm -framework GLUT -framework OpenGL -lstdc++ -o topic_1