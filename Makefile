default:
	g++ -g -lgit2 gittut.cpp $(wildcard src/*.cpp) -Iinclude -o gittut
	./gittut
