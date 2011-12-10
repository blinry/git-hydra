SOURCES=$(wildcard src/*.cpp)

git-tutor: $(SOURCES) libgit2/build/libgit2.so.0
	g++ -g -lsfml-graphics -lsfml-window -lsfml-system -I libgit2/include/ -L libgit2/build -lgit2 src/git-tutor.cpp -Wl,-rpath='$$ORIGIN/libgit2/build' -o git-tutor

libgit2/build/libgit2.so.0:
	cd libgit2 && mkdir -p build && cd build && cmake .. && make

clean:
	rm git-tutor
	rm -rf libgit2/build/
