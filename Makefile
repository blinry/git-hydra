default:
	g++ -g -lgit2 git-tutor.cpp $(wildcard src/*.cpp) -Iinclude -o git-tutor
	./gittut
