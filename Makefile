NAME:=fillis
MAIN:=src/$(NAME).c

tcc: bin
	tcc -Werror -o bin/$(NAME).tcc.exe $(MAIN)

gcc: bin
	gcc -O3 -Wall -Wextra -o bin/$(NAME).gcc.exe $(MAIN)

run:
	tcc -run $(MAIN) 'clear; ping www.example.com'

bin:
	mkdir bin