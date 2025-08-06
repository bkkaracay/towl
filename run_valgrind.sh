gcc -lm -g -D TOWL_TEST -I./src/ src/*.c test/*.c -o bin/test && valgrind --leak-check=full --show-leak-kinds=all -s bin/test
