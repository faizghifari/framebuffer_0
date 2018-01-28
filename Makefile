all:
	gcc src/main.1.c src/screen_util.c src/image.c src/screen.c -I src -o bin/main