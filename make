emulador:
    gcc emulador.c machine.c display.c hardware.c main.c $(shell pkg-config --cflags --libs sdl2)
