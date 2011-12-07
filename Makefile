default: git-tutor
	./git-tutor ~/tmp/test/

SOURCES=$(wildcard src/*.cpp)

git-tutor: $(SOURCES)
	g++ -g -lsfml-graphics -lgit2 src/git-tutor.cpp -o git-tutor
