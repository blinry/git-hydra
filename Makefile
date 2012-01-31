SOURCES=$(wildcard src/*.cpp)

git-tutor: $(SOURCES) deps/libgit2/build/libgit2.so.0 deps/SFML/build/lib/libsfml-graphics.so.2.0
	g++ -g -lsfml-graphics -lsfml-window -lsfml-system -I deps/libgit2/include/ -I deps/SFML/include/ -L deps/libgit2/build -L deps/SFML/build/lib -lgit2 src/git-tutor.cpp -Wl,-rpath='$$ORIGIN/deps/libgit2/build' -Wl,-rpath='$$ORIGIN/deps/SFML/build/lib' -o git-tutor

deps/libgit2/build/libgit2.so.0:
	cd deps/libgit2 && mkdir -p build && cd build && cmake .. && make

deps/SFML/build/lib/libsfml-graphics.so.2.0:
	cd deps/SFML && mkdir -p build && cd build && cmake .. && make

clean:
	rm git-tutor
	rm -rf deps/libgit2/build/
	rm -rf deps/SFML/build/
