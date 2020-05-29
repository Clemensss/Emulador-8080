emulador:
	@echo "Emulador with debug"
	gcc -o emubin -DEBUG newCpu.c screen.c main.c $(shell pkg-config --cflags --libs sdl2)

state:
	@echo "state"
	gcc -o emubin -DEBUG -DSTATE newCpu.c screen.c main.c $(shell pkg-config --cflags --libs sdl2)

cpudiag: 
	@echo "cpudiag"
	gcc -o emubin -DEBUG -DCPUDIAG newCpu.c screen.c main.c $(shell pkg-config --cflags --libs sdl2)

fuckit:
	@echo "fuckit"
	gcc -o emubin -DEBUG -DFUCKIT newCpu.c screen.c main.c $(shell pkg-config --cflags --libs sdl2)

