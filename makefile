all:
	make compile
	make executar

compile:
	gcc vsh.c -o vsh -g

executar:
	./vsh