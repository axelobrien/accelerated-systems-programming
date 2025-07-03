#include <stddef.h> // for NULL
#include <stddef.h> // for size_t
#include <stdint.h> // for uint32_t
#include <stdio.h>  // for printf
#include <stdlib.h> // for EXIT_FAILURE
#include <fcntl.h> // open
#include <unistd.h> // read
#include <stdbool.h> // bool, true, false
#include <SDL2/SDL.h>

#include "emulator.h"

#define SCREEN_SIZE (100)
#define SCALE_FACTOR (1)

int main(int argc, char const *const *const argv) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return EXIT_FAILURE;
	}

	SDL_Window *const window =
		SDL_CreateWindow(
			"sdl2_demo", 
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			1024,
			512,
			SDL_WINDOW_SHOWN
		);

	if (window == NULL) {
		return EXIT_FAILURE;
	}

	SDL_Renderer *const renderer = SDL_CreateRenderer(
		window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (renderer == NULL) {
		return EXIT_FAILURE;
	}

	SDL_PixelFormat *const format =
		SDL_AllocFormat(SDL_GetWindowPixelFormat(window));

	if (format == NULL) {
		return EXIT_FAILURE;
	}

	uint32_t const colors[] = {
		SDL_MapRGB(format, 0x00, 0x00, 0x00),
		SDL_MapRGB(format, 0xFF, 0xFF, 0xFF),
	};

	SDL_FreeFormat(format);

	SDL_Texture *const screen = SDL_CreateTexture(
		renderer,
		SDL_GetWindowPixelFormat(window),
		SDL_TEXTUREACCESS_STREAMING,
		64 * SCALE_FACTOR,
		32 * SCALE_FACTOR
	);

	if (screen == NULL) {
		return EXIT_FAILURE;
	}

	// it's also our job to make sure that sprites get clipped when out of bounds.
	// Note: position can be out of bounds (it's wrapped)

	if (argc < 2) {
		puts("Please pass in a file to run");
		return EXIT_FAILURE;
	}

	int fd = open(argv[1], O_RDONLY);

	if (fd < 0) {
		puts("Could not open file");
		return EXIT_FAILURE;
	}

	struct chip8_system system = {};

	ssize_t bytes_read = read(fd, system.memory + 0x200, 0x6A0); // 0x1000 - 0x200 at the start - 0x100 for display refresh - 96 stack

	if (bytes_read < 1) {
		puts("Error reading file");
		return EXIT_FAILURE;
	}

	system.counter = 0x200;
	bool wait_for_key = false;

	while (1) {
		system.current_opcode = (system.memory[system.counter] << 8) | system.memory[system.counter + 1];
		//system.current_opcode = system.memory[system.counter];
		if (system.delay_timer > 0) {
			system.delay_timer--;
		}

		if (system.sound_timer > 0) {
			printf("\7");
			system.sound_timer--;
		}


		SDL_Event event;
		while (SDL_PollEvent(&event)) { 
			if (event.type == SDL_QUIT) {
				goto done;
			}

			if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
				int key = 17;

				switch (event.key.keysym.sym) {
					case SDLK_0:
						key = 0;
						break;
					case SDLK_1:
						key = 1;
						break;
					case SDLK_2:
						key = 2;
						break;
					case SDLK_3:
						key = 3;
						break;
					case SDLK_4:
						key = 4;
						break;
					case SDLK_5:
						key = 5;
						break;
					case SDLK_6:
						key = 6;
						break;
					case SDLK_7:
						key = 7;
						break;
					case SDLK_8:
						key = 8;
						break;
					case SDLK_9:
						key = 9;
						break;
					case SDLK_a:
						key = 10;
						break;
					case SDLK_b:
						key = 11;
						break;
					case SDLK_c:
						key = 12;
						break;
					case SDLK_d:
						key = 13;
						break;
					case SDLK_e:
						key = 14;
						break;
					case SDLK_f:
						key = 15;
						break;
				}

				switch (system.current_opcode & 0xF0FF) {
					case 0xE09E:
						if (key == (system.registers[((system.current_opcode >> 8) & 0xF)] & 0xF)) {
							system.counter += 2;
						}
						break;
					case 0xE0A1:
						if (key != (system.registers[((system.current_opcode >> 8) & 0xF)] & 0xF)) {
							system.counter += 2;
						}
						break;
					case 0xE00A:
						system.registers[(system.current_opcode >> 8) & 0xF] = key;
						wait_for_key = false;
						break;
				}

			}
		}

		if (wait_for_key) {
			continue;
		}

		system.counter += 2;

		switch (system.current_opcode) {
			case 0x00E0:
				for (int i = 0; i < 0x100; i++) {
					system.memory[0x0F00 + i] = 0;
				}
				break;
			case 0x00EE:
				for (uint8_t i = 0; i < 0x1000; i++) {
					if (system.stack[i] == 0) {
						if (i != 0) {
							i--;
						}

						system.counter = system.stack[i];
						system.stack[i] = 0;
						break;
					}
				}
				break;
		}

		printf("opcode: %x\n", system.current_opcode);

		switch ((system.current_opcode & 0xF000) >> 12) {
			case 1:
				// goto NNN;
				system.counter = system.current_opcode & 0x0FFF;
				break;
			case 2:
				bool found_space = false;
				for (uint32_t i = 0; i < 0x1000; i++) {
					if (system.stack[i] == 0) {
						system.stack[i] = system.counter;
						found_space = true;
						break;
					}
				}

				if (!found_space) {
					printf("Stack Overflow!\n");
					goto done;
				}
				system.counter = system.current_opcode & 0x0FFF;
				break;
			case 3:
				if (system.registers[(system.current_opcode >> 8) & 0xF] == (system.current_opcode & 0xFF)) {
					system.counter += 2;
				}
				break;
			case 4:
				if (system.registers[(system.current_opcode >> 8) & 0xF] != (system.current_opcode & 0xFF)) {
					system.counter += 2;
				}
				break;
			case 6:
				// Vx = NN
				system.registers[(system.current_opcode >> 8) & 0x0F] = system.current_opcode & 0x00FF;
				printf("V%d = %d\n", (system.current_opcode >> 8) & 0x0F, system.current_opcode & 0x00FF);
				break;
			case 7:
				// Vx += NN
				system.registers[(system.current_opcode >> 8) & 0x0F] += system.current_opcode & 0x00FF;
				printf("V%d += %d\n", (system.current_opcode >> 8) & 0x0F, system.current_opcode & 0x00FF);
				break;
			case 0xA:
				// I = NNN
				system.addr = system.current_opcode & 0x0FFF;
				printf("I: %d\n", system.addr);
				break;
			case 0xB:
				system.counter = system.registers[0] + system.current_opcode & 0x0FFF;
				break;
			case 0xC:
				system.registers[(system.current_opcode >> 8) & 0x0F] = rand() & (system.current_opcode & 0x0FF);
				break;
			case 0xD:
				// Draw
				int xPos = system.registers[(system.current_opcode & 0x0F00) >> 8] % 64;
				int yPos = system.registers[(system.current_opcode & 0x00F0) >> 4] % 32;
				int height = system.current_opcode & 0x000F;

				printf("xPos: %d\n", xPos);
				printf("yPos: %d\n", yPos);
				printf("height: %d\n", height);

				int bits_visible = xPos + 8 - 64;

				if (bits_visible < 1) {
					bits_visible = 8;
				}

				uint8_t shift = xPos % 8;

				system.registers[0xF] = 0;
				for (int i = 0; i < height; i++) {
					uint8_t row = system.memory[system.addr + i] & (0xFF << (8 - bits_visible));
					bit *write_location = system.display + xPos + (yPos * 64) + (i * 64);

					for (int j = 0; j < 8; j++) {
						bit original_value = *write_location;
						*write_location ^= row >> (7 - j);
						if (original_value == 1 && *write_location == 0) {
							system.registers[0xF] = 0;
						}
							
						write_location++;
					}
					


//					system.memory[0xF00 + (xPos / 8) + (yPos * 8) + (height * 8)] ^= row << shift;
//					if (shift != 0) {
//						system.memory[0xF01 + (xPos / 8) + (yPos * 8) + (height * 8)] ^= row >> shift;
//					}
				}



				break;
		}

		uint8_t original_reg = 0;
		switch (system.current_opcode & 0xF00F) {
			case 0x5000:
				if (system.registers[(system.current_opcode >> 8) & 0xF] == system.registers[(system.current_opcode >> 4) & 0xF]) {
					system.counter += 2;
				}
				break;
			case 0x8000:
				system.registers[(system.current_opcode >> 8) & 0xF] = system.registers[(system.current_opcode >> 4) & 0xF];
				break;
			case 0x8001:
				system.registers[(system.current_opcode >> 8) & 0xF] |= system.registers[(system.current_opcode >> 4) & 0xF];
				break;
			case 0x8002:
				system.registers[(system.current_opcode >> 8) & 0xF] &= system.registers[(system.current_opcode >> 4) & 0xF];
				break;
			case 0x8003:
				system.registers[(system.current_opcode >> 8) & 0xF] ^= system.registers[(system.current_opcode >> 4) & 0xF];
				break;
			case 0x8004:
				system.registers[(system.current_opcode >> 8) & 0xF] += system.registers[(system.current_opcode >> 4) & 0xF];

				if (system.registers[(system.current_opcode >> 8) & 0xF] < system.registers[(system.current_opcode >> 4) & 0xF]) {
					system.registers[0xF] = 1;
				} else {
					system.registers[0xF] = 0;
				}
				break;
			case 0x8005:
				original_reg = system.registers[(system.current_opcode >> 8) & 0xF];
				system.registers[(system.current_opcode >> 8) & 0xF] -= system.registers[(system.current_opcode >> 4) & 0xF];

				if (system.registers[(system.current_opcode >> 8) & 0xF] > original_reg) {
					system.registers[0xF] = 1;
				} else {
					system.registers[0xF] = 0;
				}
				break;
			case 0x8006:
				system.registers[0xF] = system.registers[(system.current_opcode >> 8) & 0xF] & 0x1;
				system.registers[(system.current_opcode >> 8) & 0xF] >>= 1;
				break;
			case 0x8007:
				original_reg = system.registers[(system.current_opcode >> 8) & 0xF];
				system.registers[(system.current_opcode >> 8) & 0xF] = system.registers[(system.current_opcode >> 4) & 0xF] - system.registers[(system.current_opcode >> 8) & 0xF];

				if (system.registers[(system.current_opcode >> 4) & 0xF] > system.registers[(system.current_opcode >> 8) & 0xF]) {
					system.registers[0xF] = 1;
				} else {
					system.registers[0xF] = 0;
				}
				break;
			case 0x800E:
				system.registers[0xF] = system.registers[(system.current_opcode >> 8) & 0xF] >> 7;
				system.registers[(system.current_opcode >> 8) & 0xF] <<= 1;
				break;
			case 0x9000:
				if (system.registers[(system.current_opcode >> 8) & 0xF] != system.registers[(system.current_opcode >> 4) & 0xF]) {
					system.counter += 2;
				}
				break;
		}
		
		uint12_t original_I = system.addr;
		switch (system.current_opcode & 0xF0FF) {
			case 0xF007:
				system.registers[(system.current_opcode >> 8) & 0xF] = system.delay_timer;
				break;
			case 0xF00A:
				// ?!
				wait_for_key = true;
				break;
			case 0xF015:
				system.delay_timer = system.registers[(system.current_opcode >> 8) & 0xF];
				break;
			case 0xF018:
				system.sound_timer = system.registers[(system.current_opcode >> 8) & 0xF];
				break;
			case 0xF1E:
				system.addr += system.registers[(system.current_opcode >> 8) & 0xF];
				break;
			case 0xF029:
				// ?!
			break;
			case 0xF033:
				int Vx_copy = system.registers[(system.current_opcode >> 8) & 0xF];
				for (int i = 2; i >= 0; i--) {
					system.memory[system.addr + i] = Vx_copy % 10;
					// printf("Vx_copy: %d\n", Vx_copy % 10);
					Vx_copy /= 10;
				}
				break;
			case 0xF055:
				for (int i = 0; i < ((system.current_opcode >> 8) & 0xF); i++) {
					system.memory[system.addr + i] = system.registers[i];
				}

				// system.addr = original_I;
				break;
			case 0xF065:
				if (system.current_opcode == 0xF065) {
					system.registers[0] = system.memory[system.addr];
					break;
				}

				for (int i = 0; ((i < system.current_opcode >> 8) & 0xF); i++) {
					system.registers[i] = system.memory[system.addr + i];
					//system.addr++;
				}

				// system.addr = original_I;
				break;
		}

		uint32_t *pixels;
		int unused;
		SDL_LockTexture(screen, NULL, (void **)&pixels, &unused);
		for (int i = 0; i < 64; i++) {
			for (int j = 0; j < 32; j++) {
				for (int k = 0; k < 8; k++) {
					//pixels[j + k + (i * 32)] = colors[(system.memory[0xF00 + j + (i * 32)] & (1 << k)) > 0 ? 1 : 0];
					pixels[j + k + (i * 32)] = colors[system.display[j + k + (i * 32)]];
					//printf("pos %d, %d = %d\n", )
				}
			}
		}
		SDL_UnlockTexture(screen);
		SDL_RenderCopy(renderer, screen, NULL, NULL);
		SDL_RenderPresent(renderer);
	}
done:
	SDL_DestroyTexture(screen);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
}
