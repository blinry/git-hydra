SOURCES=$(wildcard src/*.cpp)

git-hydra: $(SOURCES)
	clang -g -lsfml-graphics -lsfml-window -lsfml-system -lgit2 src/git-hydra.cpp -o git-hydra -lstdc++ -lm

clean:
	rm -f git-hydra
