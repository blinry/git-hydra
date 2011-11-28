default:
	g++ -g -lsfml-graphics -lgit2 git-tutor.cpp $(wildcard src/*.cpp) -Iinclude -o git-tutor
	./git-tutor
