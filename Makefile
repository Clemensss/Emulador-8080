emulador:
	@echo "Emulador with debug"
	gcc -o emubin -Wall -DDEBUG -g newCpu.c screen.c main.c $(shell pkg-config --cflags --libs sdl2)

cpudiag: 
	@echo "cpudiag"
	gcc -o emubin -Wall -DDEBUG -DCPUDIAG -g newCpu.c screen.c main.c $(shell pkg-config --cflags --libs sdl2)

