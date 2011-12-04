default: git-tutor
	./git-tutor ~/tmp/test/

SOURCES=$(wildcard *.cpp)

git-tutor: $(SOURCES)
	g++ -g -lsfml-graphics -lgit2 git-tutor.cpp -o git-tutor
