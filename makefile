compile: topic_1.cpp
	gcc topic_1.cpp -lm -lglut -lGLU -lGL -lstdc++ -o topic_1

debug: topic_1.cpp
	gcc -g topic_1.cpp -lm -lglut -lGLU -lGL -lstdc++ -o topic_1
