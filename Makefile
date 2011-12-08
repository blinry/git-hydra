default: git-tutor
	./git-tutor ~/tmp/test/
	#./git-tutor ~/projects/advent/
	#./git-tutor ~/projects/git/
	#./git-tutor ~/projects/linux/

SOURCES=$(wildcard src/*.cpp)

git-tutor: $(SOURCES)
	g++ -g -lsfml-graphics -lgit2 src/git-tutor.cpp -o git-tutor
