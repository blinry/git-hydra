SOURCES=$(wildcard src/*.cpp)

git-hydra: $(SOURCES)
	g++ -g -lsfml-graphics -lsfml-window -lsfml-system -lgit2 src/git-hydra.cpp -o git-hydra

clean:
	rm -f git-hydra
