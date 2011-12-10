run: git-tutor
	#./git-tutor ~/tmp/test/
	./git-tutor ~/projects/uni/cg/6/
	#./git-tutor ~/.dotfiles/
	#./git-tutor ~/projects/advent/
	#./git-tutor ~/projects/git/
	#./git-tutor ~/projects/linux/

SOURCES=$(wildcard src/*.cpp)

git-tutor: $(SOURCES) libgit2/build/libgit2.so.0
	g++ -g -lsfml-graphics -lsfml-window -lsfml-system -I libgit2/include/ -L libgit2/build -lgit2 src/git-tutor.cpp -Wl,-rpath=libgit2/build -o git-tutor

libgit2/build/libgit2.so.0:
	cd libgit2 && mkdir -p build && cd build && cmake .. && make

clean:
	rm git-tutor
	rm -rf libgit2/build/
