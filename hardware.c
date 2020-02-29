#include "hardware.h" 

//TODO
    //display and shit
void generate_interrupt(state8080 *state)
{
    command_maker(0xd7);
    command_maker(0xcf);
}

uint8_t read_i_port(port *i, uint8_t port)
{
    if(port == 0) return i->input->port0;
    else if(port == 1) return i->input->port1;
    else if(port == 2) return i->input->port2;
    else if(port == 3) return i->input->port3;
}

uint16_t write_o_port(port *o, uint8_t port, uint8_t data)
{
    if(port == 2)
    {
	uint8_t x = 8 - data;
	uint8_t result = o->output->port4 >> x;
	o->input->port3 = result;
    }
    
    else if(port == 5)
    {
	//something around fleet data;
    }
    
    else if(port == 4) 
    {
	data = (uint16_t) data;
	o->output->port4 = data << 8 | o->output->port4 >> 8;	
    }
}

uint16_t read_o_port(port *o, uint8_t port, uint8_t data)
{

}


uint8_t write_i_port(port *i, uint8_t port)
{

}
