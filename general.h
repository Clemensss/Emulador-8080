#ifndef GENERAL_H_
#define GENERAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <time.h> 

#include <SDL2/SDL.h>
#include "emulador.h" 
#include "machine.h"
#include "hardware.h"
#include "display.h"


#define RAM_SIZE 65536 
#define REG state->registers

#endif
