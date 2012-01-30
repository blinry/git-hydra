SOURCES=$(wildcard src/*.cpp)

git-tutor: $(SOURCES) libgit2/build/libgit2.so.0 SFML/build/lib/libsfml-graphics.so.2.0
	g++ -g -lsfml-graphics -lsfml-window -lsfml-system -I libgit2/include/ -I SFML/include/ -L libgit2/build -L SFML/build/lib -lgit2 src/git-tutor.cpp -Wl,-rpath='$$ORIGIN/libgit2/build' -Wl,-rpath='$$ORIGIN/SFML/build/lib' -o git-tutor

libgit2/build/libgit2.so.0:
	cd libgit2 && mkdir -p build && cd build && cmake .. && make

SFML/build/lib/libsfml-graphics.so.2.0:
	cd SFML && mkdir -p build && cd build && cmake .. && make

clean:
	rm git-tutor
	rm -rf libgit2/build/
	rm -rf SFML/build/
