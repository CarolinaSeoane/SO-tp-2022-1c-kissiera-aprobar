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
	gnome-terminal -- bash -c './consola.out instrucciones.txt 250; sleep 20'
	gnome-terminal -- bash -c './consola.out inexit.txt 640; sleep 20' 

base_config:
	# Config Kernel
	sed -i 's/^ALGORITMO_PLANIFICACION=.*/ALGORITMO_PLANIFICACION=FIFO/' ../kernel/kernel.config
	sed -i 's/^ESTIMACION_INICIAL=.*/ESTIMACION_INICIAL=10000/' ../kernel/kernel.config
	sed -i 's/^ALFA=.*/ALFA=0.5/' ../kernel/kernel.config
	sed -i 's/^GRADO_MULTIPROGRAMACION=.*/GRADO_MULTIPROGRAMACION=4/' ../kernel/kernel.config
	sed -i 's/^TIEMPO_MAXIMO_BLOQUEADO=.*/TIEMPO_MAXIMO_BLOQUEADO=100000/' ../kernel/kernel.config

	# Config CPU
	sed -i 's/^ENTRADAS_TLB=.*/ENTRADAS_TLB=4/' ../cpu/cpu.config
	sed -i 's/^REEMPLAZO_TLB=.*/REEMPLAZO_TLB=LRU/' ../cpu/cpu.config
	sed -i 's/^RETARDO_NOOP=.*/RETARDO_NOOP=1000/' ../cpu/cpu.config

	# Config Memoria
	sed -i 's/^TAM_MEMORIA=.*/TAM_MEMORIA=4096/' ../memoria/memoria.config
	sed -i 's/^TAM_PAGINA=.*/TAM_PAGINA=64/' ../memoria/memoria.config
	sed -i 's/^ENTRADAS_POR_TABLA=.*/ENTRADAS_POR_TABLA=4/' ../memoria/memoria.config
	sed -i 's/^RETARDO_MEMORIA=.*/RETARDO_MEMORIA=1000/' ../memoria/memoria.config
	sed -i 's/^ALGORITMO_REEMPLAZO=.*/ALGORITMO_REEMPLAZO=CLOCK-M/' ../memoria/memoria.config
	sed -i 's/^MARCOS_POR_PROCESO=.*/MARCOS_POR_PROCESO=4/' ../memoria/memoria.config
	sed -i 's/^RETARDO_SWAP=.*/RETARDO_SWAP=2000/' ../memoria/memoria.config

base_instrucciones:
	# Instrucciones
	gnome-terminal -- bash -c './consola.out pruebas/BASE_1.txt 200; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/BASE_2.txt 300; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/BASE_2.txt 300; sleep 20'

planificacion_fifo_config:
	# Config Kernel
	sed -i 's/^ALGORITMO_PLANIFICACION=.*/ALGORITMO_PLANIFICACION=FIFO/' ../kernel/kernel.config
	sed -i 's/^ESTIMACION_INICIAL=.*/ESTIMACION_INICIAL=20000/' ../kernel/kernel.config
	sed -i 's/^ALFA=.*/ALFA=0.5/' ../kernel/kernel.config
	sed -i 's/^GRADO_MULTIPROGRAMACION=.*/GRADO_MULTIPROGRAMACION=4/' ../kernel/kernel.config
	sed -i 's/^TIEMPO_MAXIMO_BLOQUEADO=.*/TIEMPO_MAXIMO_BLOQUEADO=10000/' ../kernel/kernel.config

	# Config CPU
	sed -i 's/^ENTRADAS_TLB=.*/ENTRADAS_TLB=4/' ../cpu/cpu.config
	sed -i 's/^REEMPLAZO_TLB=.*/REEMPLAZO_TLB=LRU/' ../cpu/cpu.config
	sed -i 's/^RETARDO_NOOP=.*/RETARDO_NOOP=1000/' ../cpu/cpu.config

	# Config Memoria
	sed -i 's/^TAM_MEMORIA=.*/TAM_MEMORIA=4096/' ../memoria/memoria.config
	sed -i 's/^TAM_PAGINA=.*/TAM_PAGINA=64/' ../memoria/memoria.config
	sed -i 's/^ENTRADAS_POR_TABLA=.*/ENTRADAS_POR_TABLA=4/' ../memoria/memoria.config
	sed -i 's/^RETARDO_MEMORIA=.*/RETARDO_MEMORIA=1000/' ../memoria/memoria.config
	sed -i 's/^ALGORITMO_REEMPLAZO=.*/ALGORITMO_REEMPLAZO=CLOCK-M/' ../memoria/memoria.config
	sed -i 's/^MARCOS_POR_PROCESO=.*/MARCOS_POR_PROCESO=4/' ../memoria/memoria.config
	sed -i 's/^RETARDO_SWAP=.*/RETARDO_SWAP=2000/' ../memoria/memoria.config

planificacion_fifo_instrucciones:
	# Instrucciones
	gnome-terminal -- bash -c './consola.out pruebas/PLANI_1.txt 320; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/PLANI_1.txt 320; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/PLANI_2.txt 456; sleep 20'

planificacion_srt_config:
	# Config Kernel
	sed -i 's/^ALGORITMO_PLANIFICACION=.*/ALGORITMO_PLANIFICACION=SRT/' ../kernel/kernel.config
	sed -i 's/^ESTIMACION_INICIAL=.*/ESTIMACION_INICIAL=20000/' ../kernel/kernel.config
	sed -i 's/^ALFA=.*/ALFA=0.5/' ../kernel/kernel.config
	sed -i 's/^GRADO_MULTIPROGRAMACION=.*/GRADO_MULTIPROGRAMACION=4/' ../kernel/kernel.config
	sed -i 's/^TIEMPO_MAXIMO_BLOQUEADO=.*/TIEMPO_MAXIMO_BLOQUEADO=10000/' ../kernel/kernel.config

	# Config CPU
	sed -i 's/^ENTRADAS_TLB=.*/ENTRADAS_TLB=4/' ../cpu/cpu.config
	sed -i 's/^REEMPLAZO_TLB=.*/REEMPLAZO_TLB=LRU/' ../cpu/cpu.config
	sed -i 's/^RETARDO_NOOP=.*/RETARDO_NOOP=1000/' ../cpu/cpu.config

	# Config Memoria
	sed -i 's/^TAM_MEMORIA=.*/TAM_MEMORIA=4096/' ../memoria/memoria.config
	sed -i 's/^TAM_PAGINA=.*/TAM_PAGINA=64/' ../memoria/memoria.config
	sed -i 's/^ENTRADAS_POR_TABLA=.*/ENTRADAS_POR_TABLA=4/' ../memoria/memoria.config
	sed -i 's/^RETARDO_MEMORIA=.*/RETARDO_MEMORIA=1000/' ../memoria/memoria.config
	sed -i 's/^ALGORITMO_REEMPLAZO=.*/ALGORITMO_REEMPLAZO=CLOCK-M/' ../memoria/memoria.config
	sed -i 's/^MARCOS_POR_PROCESO=.*/MARCOS_POR_PROCESO=4/' ../memoria/memoria.config
	sed -i 's/^RETARDO_SWAP=.*/RETARDO_SWAP=2000/' ../memoria/memoria.config

planificacion_srt_instrucciones:
	# Instrucciones
	gnome-terminal -- bash -c './consola.out pruebas/PLANI_1.txt 320; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/PLANI_1.txt 320; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/PLANI_2.txt 456; sleep 20'

suspension_fifo_config:
	# Config Kernel
	sed -i 's/^ALGORITMO_PLANIFICACION=.*/ALGORITMO_PLANIFICACION=FIFO/' ../kernel/kernel.config
	sed -i 's/^ESTIMACION_INICIAL=.*/ESTIMACION_INICIAL=20000/' ../kernel/kernel.config
	sed -i 's/^ALFA=.*/ALFA=0.5/' ../kernel/kernel.config
	sed -i 's/^GRADO_MULTIPROGRAMACION=.*/GRADO_MULTIPROGRAMACION=2/' ../kernel/kernel.config
	sed -i 's/^TIEMPO_MAXIMO_BLOQUEADO=.*/TIEMPO_MAXIMO_BLOQUEADO=8000/' ../kernel/kernel.config

	# Config CPU
	sed -i 's/^ENTRADAS_TLB=.*/ENTRADAS_TLB=4/' ../cpu/cpu.config
	sed -i 's/^REEMPLAZO_TLB=.*/REEMPLAZO_TLB=LRU/' ../cpu/cpu.config
	sed -i 's/^RETARDO_NOOP=.*/RETARDO_NOOP=1000/' ../cpu/cpu.config

	# Config Memoria
	sed -i 's/^TAM_MEMORIA=.*/TAM_MEMORIA=4096/' ../memoria/memoria.config
	sed -i 's/^TAM_PAGINA=.*/TAM_PAGINA=64/' ../memoria/memoria.config
	sed -i 's/^ENTRADAS_POR_TABLA=.*/ENTRADAS_POR_TABLA=4/' ../memoria/memoria.config
	sed -i 's/^RETARDO_MEMORIA=.*/RETARDO_MEMORIA=1000/' ../memoria/memoria.config
	sed -i 's/^ALGORITMO_REEMPLAZO=.*/ALGORITMO_REEMPLAZO=CLOCK-M/' ../memoria/memoria.config
	sed -i 's/^MARCOS_POR_PROCESO=.*/MARCOS_POR_PROCESO=4/' ../memoria/memoria.config
	sed -i 's/^RETARDO_SWAP=.*/RETARDO_SWAP=2000/' ../memoria/memoria.config

suspension_fifo_instrucciones:
	# Instrucciones
	gnome-terminal -- bash -c './consola.out pruebas/SUSPE_1.txt 500; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/SUSPE_2.txt 342; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/SUSPE_3.txt 344; sleep 20'

suspension_srt_config:
	# Config Kernel
	sed -i 's/^ALGORITMO_PLANIFICACION=.*/ALGORITMO_PLANIFICACION=SRT/' ../kernel/kernel.config
	sed -i 's/^ESTIMACION_INICIAL=.*/ESTIMACION_INICIAL=20000/' ../kernel/kernel.config
	sed -i 's/^ALFA=.*/ALFA=0.5/' ../kernel/kernel.config
	sed -i 's/^GRADO_MULTIPROGRAMACION=.*/GRADO_MULTIPROGRAMACION=2/' ../kernel/kernel.config
	sed -i 's/^TIEMPO_MAXIMO_BLOQUEADO=.*/TIEMPO_MAXIMO_BLOQUEADO=8000/' ../kernel/kernel.config

	# Config CPU
	sed -i 's/^ENTRADAS_TLB=.*/ENTRADAS_TLB=4/' ../cpu/cpu.config
	sed -i 's/^REEMPLAZO_TLB=.*/REEMPLAZO_TLB=LRU/' ../cpu/cpu.config
	sed -i 's/^RETARDO_NOOP=.*/RETARDO_NOOP=1000/' ../cpu/cpu.config

	# Config Memoria
	sed -i 's/^TAM_MEMORIA=.*/TAM_MEMORIA=4096/' ../memoria/memoria.config
	sed -i 's/^TAM_PAGINA=.*/TAM_PAGINA=64/' ../memoria/memoria.config
	sed -i 's/^ENTRADAS_POR_TABLA=.*/ENTRADAS_POR_TABLA=4/' ../memoria/memoria.config
	sed -i 's/^RETARDO_MEMORIA=.*/RETARDO_MEMORIA=1000/' ../memoria/memoria.config
	sed -i 's/^ALGORITMO_REEMPLAZO=.*/ALGORITMO_REEMPLAZO=CLOCK-M/' ../memoria/memoria.config
	sed -i 's/^MARCOS_POR_PROCESO=.*/MARCOS_POR_PROCESO=4/' ../memoria/memoria.config
	sed -i 's/^RETARDO_SWAP=.*/RETARDO_SWAP=2000/' ../memoria/memoria.config

suspension_srt_instrucciones:
	# Instrucciones
	gnome-terminal -- bash -c './consola.out pruebas/SUSPE_1.txt 500; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/SUSPE_2.txt 342; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/SUSPE_3.txt 344; sleep 20'

memoria_clock_config:
	# Config Kernel
	sed -i 's/^ALGORITMO_PLANIFICACION=.*/ALGORITMO_PLANIFICACION=FIFO/' ../kernel/kernel.config
	sed -i 's/^ESTIMACION_INICIAL=.*/ESTIMACION_INICIAL=20000/' ../kernel/kernel.config
	sed -i 's/^ALFA=.*/ALFA=0.5/' ../kernel/kernel.config
	sed -i 's/^GRADO_MULTIPROGRAMACION=.*/GRADO_MULTIPROGRAMACION=2/' ../kernel/kernel.config
	sed -i 's/^TIEMPO_MAXIMO_BLOQUEADO=.*/TIEMPO_MAXIMO_BLOQUEADO=5000/' ../kernel/kernel.config

	# Config CPU
	sed -i 's/^ENTRADAS_TLB=.*/ENTRADAS_TLB=1/' ../cpu/cpu.config
	sed -i 's/^REEMPLAZO_TLB=.*/REEMPLAZO_TLB=LRU/' ../cpu/cpu.config
	sed -i 's/^RETARDO_NOOP=.*/RETARDO_NOOP=1000/' ../cpu/cpu.config

	# Config Memoria
	sed -i 's/^TAM_MEMORIA=.*/TAM_MEMORIA=2048/' ../memoria/memoria.config
	sed -i 's/^TAM_PAGINA=.*/TAM_PAGINA=256/' ../memoria/memoria.config
	sed -i 's/^ENTRADAS_POR_TABLA=.*/ENTRADAS_POR_TABLA=4/' ../memoria/memoria.config
	sed -i 's/^RETARDO_MEMORIA=.*/RETARDO_MEMORIA=1000/' ../memoria/memoria.config
	sed -i 's/^ALGORITMO_REEMPLAZO=.*/ALGORITMO_REEMPLAZO=CLOCK/' ../memoria/memoria.config
	sed -i 's/^MARCOS_POR_PROCESO=.*/MARCOS_POR_PROCESO=3/' ../memoria/memoria.config
	sed -i 's/^RETARDO_SWAP=.*/RETARDO_SWAP=5000/' ../memoria/memoria.config

memoria_clock_instrucciones:
	# Instrucciones
	gnome-terminal -- bash -c './consola.out pruebas/MEMORIA_1.txt 4096; sleep 20'

memoria_clock_m_config:
	# Config Kernel
	sed -i 's/^ALGORITMO_PLANIFICACION=.*/ALGORITMO_PLANIFICACION=FIFO/' ../kernel/kernel.config
	sed -i 's/^ESTIMACION_INICIAL=.*/ESTIMACION_INICIAL=20000/' ../kernel/kernel.config
	sed -i 's/^ALFA=.*/ALFA=0.5/' ../kernel/kernel.config
	sed -i 's/^GRADO_MULTIPROGRAMACION=.*/GRADO_MULTIPROGRAMACION=2/' ../kernel/kernel.config
	sed -i 's/^TIEMPO_MAXIMO_BLOQUEADO=.*/TIEMPO_MAXIMO_BLOQUEADO=5000/' ../kernel/kernel.config

	# Config CPU
	sed -i 's/^ENTRADAS_TLB=.*/ENTRADAS_TLB=1/' ../cpu/cpu.config
	sed -i 's/^REEMPLAZO_TLB=.*/REEMPLAZO_TLB=LRU/' ../cpu/cpu.config
	sed -i 's/^RETARDO_NOOP=.*/RETARDO_NOOP=1000/' ../cpu/cpu.config

	# Config Memoria
	sed -i 's/^TAM_MEMORIA=.*/TAM_MEMORIA=2048/' ../memoria/memoria.config
	sed -i 's/^TAM_PAGINA=.*/TAM_PAGINA=256/' ../memoria/memoria.config
	sed -i 's/^ENTRADAS_POR_TABLA=.*/ENTRADAS_POR_TABLA=4/' ../memoria/memoria.config
	sed -i 's/^RETARDO_MEMORIA=.*/RETARDO_MEMORIA=1000/' ../memoria/memoria.config
	sed -i 's/^ALGORITMO_REEMPLAZO=.*/ALGORITMO_REEMPLAZO=CLOCK-M/' ../memoria/memoria.config
	sed -i 's/^MARCOS_POR_PROCESO=.*/MARCOS_POR_PROCESO=3/' ../memoria/memoria.config
	sed -i 's/^RETARDO_SWAP=.*/RETARDO_SWAP=5000/' ../memoria/memoria.config

memoria_clock_m_instrucciones:
	# Instrucciones
	gnome-terminal -- bash -c './consola.out pruebas/MEMORIA_1.txt 4096; sleep 20'

tlb_fifo_config:
	# Config Kernel
	sed -i 's/^ALGORITMO_PLANIFICACION=.*/ALGORITMO_PLANIFICACION=FIFO/' ../kernel/kernel.config
	sed -i 's/^ESTIMACION_INICIAL=.*/ESTIMACION_INICIAL=20000/' ../kernel/kernel.config
	sed -i 's/^ALFA=.*/ALFA=0.5/' ../kernel/kernel.config
	sed -i 's/^GRADO_MULTIPROGRAMACION=.*/GRADO_MULTIPROGRAMACION=2/' ../kernel/kernel.config
	sed -i 's/^TIEMPO_MAXIMO_BLOQUEADO=.*/TIEMPO_MAXIMO_BLOQUEADO=5000/' ../kernel/kernel.config

	# Config CPU
	sed -i 's/^ENTRADAS_TLB=.*/ENTRADAS_TLB=4/' ../cpu/cpu.config
	sed -i 's/^REEMPLAZO_TLB=.*/REEMPLAZO_TLB=FIFO/' ../cpu/cpu.config
	sed -i 's/^RETARDO_NOOP=.*/RETARDO_NOOP=1000/' ../cpu/cpu.config

	# Config Memoria
	sed -i 's/^TAM_MEMORIA=.*/TAM_MEMORIA=2048/' ../memoria/memoria.config
	sed -i 's/^TAM_PAGINA=.*/TAM_PAGINA=128/' ../memoria/memoria.config
	sed -i 's/^ENTRADAS_POR_TABLA=.*/ENTRADAS_POR_TABLA=4/' ../memoria/memoria.config
	sed -i 's/^RETARDO_MEMORIA=.*/RETARDO_MEMORIA=3000/' ../memoria/memoria.config
	sed -i 's/^ALGORITMO_REEMPLAZO=.*/ALGORITMO_REEMPLAZO=CLOCK/' ../memoria/memoria.config
	sed -i 's/^MARCOS_POR_PROCESO=.*/MARCOS_POR_PROCESO=16/' ../memoria/memoria.config
	sed -i 's/^RETARDO_SWAP=.*/RETARDO_SWAP=3000/' ../memoria/memoria.config

tlb_fifo_instrucciones:
	# Instrucciones
	gnome-terminal -- bash -c './consola.out pruebas/TLB_1.txt 2048; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/TLB_2.txt 2048; sleep 20'

tlb_lru_config:
	# Config Kernel
	sed -i 's/^ALGORITMO_PLANIFICACION=.*/ALGORITMO_PLANIFICACION=FIFO/' ../kernel/kernel.config
	sed -i 's/^ESTIMACION_INICIAL=.*/ESTIMACION_INICIAL=20000/' ../kernel/kernel.config
	sed -i 's/^ALFA=.*/ALFA=0.5/' ../kernel/kernel.config
	sed -i 's/^GRADO_MULTIPROGRAMACION=.*/GRADO_MULTIPROGRAMACION=2/' ../kernel/kernel.config
	sed -i 's/^TIEMPO_MAXIMO_BLOQUEADO=.*/TIEMPO_MAXIMO_BLOQUEADO=5000/' ../kernel/kernel.config

	# Config CPU
	sed -i 's/^ENTRADAS_TLB=.*/ENTRADAS_TLB=4/' ../cpu/cpu.config
	sed -i 's/^REEMPLAZO_TLB=.*/REEMPLAZO_TLB=LRU/' ../cpu/cpu.config
	sed -i 's/^RETARDO_NOOP=.*/RETARDO_NOOP=1000/' ../cpu/cpu.config

	# Config Memoria
	sed -i 's/^TAM_MEMORIA=.*/TAM_MEMORIA=2048/' ../memoria/memoria.config
	sed -i 's/^TAM_PAGINA=.*/TAM_PAGINA=128/' ../memoria/memoria.config
	sed -i 's/^ENTRADAS_POR_TABLA=.*/ENTRADAS_POR_TABLA=4/' ../memoria/memoria.config
	sed -i 's/^RETARDO_MEMORIA=.*/RETARDO_MEMORIA=3000/' ../memoria/memoria.config
	sed -i 's/^ALGORITMO_REEMPLAZO=.*/ALGORITMO_REEMPLAZO=CLOCK/' ../memoria/memoria.config
	sed -i 's/^MARCOS_POR_PROCESO=.*/MARCOS_POR_PROCESO=16/' ../memoria/memoria.config
	sed -i 's/^RETARDO_SWAP=.*/RETARDO_SWAP=3000/' ../memoria/memoria.config

tlb_lru_instrucciones:
	# Instrucciones
	gnome-terminal -- bash -c './consola.out pruebas/TLB_1.txt 2048; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/TLB_2.txt 2048; sleep 20'

integral_config:
	# Config Kernel
	sed -i 's/^ALGORITMO_PLANIFICACION=.*/ALGORITMO_PLANIFICACION=SRT/' ../kernel/kernel.config
	sed -i 's/^ESTIMACION_INICIAL=.*/ESTIMACION_INICIAL=50000/' ../kernel/kernel.config
	sed -i 's/^ALFA=.*/ALFA=0.2/' ../kernel/kernel.config
	sed -i 's/^GRADO_MULTIPROGRAMACION=.*/GRADO_MULTIPROGRAMACION=6/' ../kernel/kernel.config
	sed -i 's/^TIEMPO_MAXIMO_BLOQUEADO=.*/TIEMPO_MAXIMO_BLOQUEADO=1000/' ../kernel/kernel.config

	# Config CPU
	sed -i 's/^ENTRADAS_TLB=.*/ENTRADAS_TLB=2/' ../cpu/cpu.config
	sed -i 's/^REEMPLAZO_TLB=.*/REEMPLAZO_TLB=LRU/' ../cpu/cpu.config
	sed -i 's/^RETARDO_NOOP=.*/RETARDO_NOOP=100/' ../cpu/cpu.config

	# Config Memoria
	sed -i 's/^TAM_MEMORIA=.*/TAM_MEMORIA=10240/' ../memoria/memoria.config
	sed -i 's/^TAM_PAGINA=.*/TAM_PAGINA=256/' ../memoria/memoria.config
	sed -i 's/^ENTRADAS_POR_TABLA=.*/ENTRADAS_POR_TABLA=4/' ../memoria/memoria.config
	sed -i 's/^RETARDO_MEMORIA=.*/RETARDO_MEMORIA=500/' ../memoria/memoria.config
	sed -i 's/^ALGORITMO_REEMPLAZO=.*/ALGORITMO_REEMPLAZO=CLOCK-M/' ../memoria/memoria.config
	sed -i 's/^MARCOS_POR_PROCESO=.*/MARCOS_POR_PROCESO=4/' ../memoria/memoria.config
	sed -i 's/^RETARDO_SWAP=.*/RETARDO_SWAP=1000/' ../memoria/memoria.config

integral_instrucciones:
	# Instrucciones
	gnome-terminal -- bash -c './consola.out pruebas/INTEGRAL_1.txt 2048; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/INTEGRAL_2.txt 2048; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/INTEGRAL_3.txt 2048; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/INTEGRAL_4.txt 2048; sleep 20'
	gnome-terminal -- bash -c './consola.out pruebas/INTEGRAL_5.txt 2048; sleep 20'