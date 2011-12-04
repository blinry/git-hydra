default: git-tutor
	./git-tutor ~/tmp/test/

git-tutor: git-tutor.cpp
	g++ -g -lsfml-graphics -lgit2 $< -o git-tutor
