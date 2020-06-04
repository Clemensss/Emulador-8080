emulador:
	@echo "Emulador with debug"
	gcc -o emubin -DDEBUG -g newCpu.c screen.c main.c $(shell pkg-config --cflags --libs sdl2)

state:
	@echo "state"
	gcc -o emubin -DDEBUG -DSTATE newCpu.c screen.c main.c $(shell pkg-config --cflags --libs sdl2)

cpudiag: 
	@echo "cpudiag"
	gcc -o emubin -DDEBUG -DCPUDIAG newCpu.c screen.c main.c $(shell pkg-config --cflags --libs sdl2)

fuckit:
	@echo "fuckit"
	gcc -o emubin -DDEBUG -DFUCKIT newCpu.c screen.c main.c $(shell pkg-config --cflags --libs sdl2)

