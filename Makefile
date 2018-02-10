all: task3 task4
	gcc src/main.c src/screen_util.c src/image.c src/screen.c src/transformer.c -I src -o bin/main -lm -g

task3:
	gcc src/main_3.c src/screen_util.c src/image.c src/screen.c src/transformer.c -I src -o bin/main_3 -lm -g

task4:
	gcc src/main_4.c src/screen_util.c src/image.c src/screen.c src/transformer.c -I src -o bin/main_4 -lm -g