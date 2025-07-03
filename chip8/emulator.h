#include <stdint.h>

typedef unsigned _BitInt(12) uint12_t;
typedef unsigned _BitInt(1) bit;

struct chip8_system {
	uint16_t current_opcode;
	uint8_t memory[0x1000];
	uint8_t registers[16];
	uint12_t counter;
	uint12_t addr; // I register
	uint12_t stack[0x1000];
	uint8_t delay_timer;
	uint8_t sound_timer;
	uint16_t input; // why tf is this a uint16_t? is an input thing at all??
	bit display[64 * 32];
};


