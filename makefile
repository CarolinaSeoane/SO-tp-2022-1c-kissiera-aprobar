all:
	make -C memoria
	make -C cpu
	make -C kernel
	make -C consola
	./ejecutar.sh

clean:
	make clean -C kernel
	make clean -C consola
	make clean -C memoria
	make clean -C cpu

ins:
	make instruccion  -C consola