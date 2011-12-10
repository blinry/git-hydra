run: git-tutor
	#./git-tutor ~/tmp/test/
	./git-tutor ~/projects/uni/cg/6/
	#./git-tutor ~/.dotfiles/
	#./git-tutor ~/projects/advent/
	#./git-tutor ~/projects/git/
	#./git-tutor ~/projects/linux/

SOURCES=$(wildcard src/*.cpp)

git-tutor: $(SOURCES)
	g++ -g -lsfml-graphics -lsfml-window -lsfml-system -I ../libgit2/include/ -L ../libgit2/ -lgit2 src/git-tutor.cpp -Wl,-rpath=../libgit2 -o git-tutor
