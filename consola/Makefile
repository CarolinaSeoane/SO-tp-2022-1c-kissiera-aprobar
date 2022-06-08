# Globales
CC = gcc
OUTFILE = consola.out

### Reglas ###

SRC = $(shell find ./src -type f -iname "*.c" -exec basename {} \;)
OBJ = $(SRC:%.c=./obj/%.o)

SHARED = $(shell find ../shared/obj/*.o)

build: makefolder makeshared $(OBJ)
	$(CC) -o $(OUTFILE) main.c $(OBJ) $(SHARED) $(LINK)

valgrind: build
	valgrind --leak-check=full --show-leak-kinds=all --log-file=valgrind.log ./$(OUTFILE)

LINK = -lpthread -lcommons -lm -lreadline
CFLAGS = -c

debug: CFLAGS := $(CFLAGS) -g
debug: makeshared_debug build

./obj/%.o: ./src/%.c
	$(CC) $(CFLAGS) $< -o $@ $(LINK)

makefolder:
	if [ ! -d ./obj ]; then mkdir obj; fi

makeshared:
	make -C ../shared

makeshared_debug:
	make debug -C ../shared

# Clean

clean:
	rm -rf obj
	rm -rf *.out
	rm -rf *.log
	make clean -C ../shared

# Ejecutar terminales

instruccion:
	gnome-terminal -- bash -c './consola.out instrucciones.txt 9; sleep 20'
	gnome-terminal -- bash -c './consola.out inexit.txt 4; sleep 20' 

base:
	gnome-terminal -- bash -c './consola.out pruebas/BASE_1.txt 200; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/BASE_2.txt 300; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/BASE_2.txt 300; sleep 20'

planificacion:
	gnome-terminal -- bash -c './consola.out pruebas/PLANI_1.txt 320; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/PLANI_1.txt 320; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/PLANI_2.txt 456; sleep 20'

suspension:
	gnome-terminal -- bash -c './consola.out pruebas/SUSPE_1.txt 500; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/SUSPE_2.txt 342; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/SUSPE_3.txt 344; sleep 20'
