all:
	g++ -Isrc/Include -Lsrc/lib -o main main.cpp game.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer