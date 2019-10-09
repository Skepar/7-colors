all: 7colors 

7colors: 7colors.o utils.o ai.o
	gcc 7colors.o utils.o ai.o  -o 7colors

7colors.o: 7colors.c
	gcc -Wall -Werror -Wextra -Wno-unused-function -Wno-unused-parameter 7colors.c -c -g

utils.o: utils.c
	gcc -Wall -Werror -Wextra -Wno-unused-function -Wno-unused-parameter utils.c -c -g

ai.o: ai.c
	gcc -Wall -Werror -Wextra -Wno-unused-function -Wno-unused-parameter ai.c -c -g
