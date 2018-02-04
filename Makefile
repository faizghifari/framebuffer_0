all:
	gcc src/main.c src/screen_util.c src/image.c src/screen.c -I src -o bin/main -lm -g
