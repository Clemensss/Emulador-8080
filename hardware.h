#ifdef HARDWARE_H
#define HARDWARE_H

#include "emulator.h" 
#include "machine.h" 
#include <time.h> 

struct Input
{
    uint8_t port0;
    uint8_t port1;
    uint8_t port2;
    uint8_t port3;
};

typedef struct Input i_port;

struct Output
{
    uint8_t port2;
    uint8_t port3;
    uint8_t port4;
    uint8_t port5;
    uint8_t port6;
};
typedef struct Ouput o_port;

struct Port
{
    i_port *input;
    o_port *output;
};

typedef struct Port port;
#endif 
