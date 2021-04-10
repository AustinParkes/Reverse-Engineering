/*
 * Emulate the ATmega328p program with simavr to brute-force hash values
 *
 * Compile with -lsimavr
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <simavr/sim_avr.h>
#include <simavr/sim_core.h>

// To keep track of elements in password generator
int current=0;
int prev=0;

// Program memory from hash_24_bits.bin
#define _PROG_MEM_SIZE 1024
const uint8_t prog_mem[_PROG_MEM_SIZE] = {
    0x0c, 0x94, 0x34, 0x00, 0x0c, 0x94, 0x51, 0x00, 0x0c, 0x94,
    0x51, 0x00, 0x0c, 0x94, 0x51, 0x00, 0x0c, 0x94, 0x51, 0x00,
    0x0c, 0x94, 0x51, 0x00, 0x0c, 0x94, 0x51, 0x00, 0x0c, 0x94,
    0x51, 0x00, 0x0c, 0x94, 0x51, 0x00, 0x0c, 0x94, 0x51, 0x00,
    0x0c, 0x94, 0x51, 0x00, 0x0c, 0x94, 0xc0, 0x00, 0x0c, 0x94,
    0x51, 0x00, 0x0c, 0x94, 0x51, 0x00, 0x0c, 0x94, 0x51, 0x00,
    0x0c, 0x94, 0x51, 0x00, 0x0c, 0x94, 0x51, 0x00, 0x0c, 0x94,
    0x51, 0x00, 0x0c, 0x94, 0x63, 0x00, 0x0c, 0x94, 0x51, 0x00,
    0x0c, 0x94, 0x51, 0x00, 0x0c, 0x94, 0x51, 0x00, 0x0c, 0x94,
    0x51, 0x00, 0x0c, 0x94, 0x51, 0x00, 0x0c, 0x94, 0x51, 0x00,
    0x0c, 0x94, 0x51, 0x00, 0x11, 0x24, 0x1f, 0xbe, 0xcf, 0xef,
    0xd8, 0xe0, 0xde, 0xbf, 0xcd, 0xbf, 0x11, 0xe0, 0xa0, 0xe0,
    0xb1, 0xe0, 0xe2, 0xe7, 0xf3, 0xe0, 0x02, 0xc0, 0x05, 0x90,
    0x0d, 0x92, 0xa2, 0x34, 0xb1, 0x07, 0xd9, 0xf7, 0x21, 0xe0,
    0xa2, 0xe4, 0xb1, 0xe0, 0x01, 0xc0, 0x1d, 0x92, 0xa4, 0x38,
    0xb2, 0x07, 0xe1, 0xf7, 0x0e, 0x94, 0x3a, 0x01, 0x0c, 0x94,
    0xb7, 0x01, 0x0c, 0x94, 0x00, 0x00, 0x89, 0xe1, 0x80, 0x93,
    0xc4, 0x00, 0xe1, 0xec, 0xf0, 0xe0, 0x88, 0xe1, 0x80, 0x83,
    0x10, 0x92, 0x42, 0x01, 0x10, 0x92, 0x43, 0x01, 0x80, 0x81,
    0x80, 0x68, 0x80, 0x83, 0x78, 0x94, 0x08, 0x95, 0x1f, 0x92,
    0x0f, 0x92, 0x0f, 0xb6, 0x0f, 0x92, 0x11, 0x24, 0x2f, 0x93,
    0x8f, 0x93, 0x9f, 0x93, 0xef, 0x93, 0xff, 0x93, 0x80, 0x91,
    0x43, 0x01, 0x90, 0xe0, 0x01, 0x96, 0x8f, 0x73, 0x99, 0x27,
    0x20, 0x91, 0x42, 0x01, 0x28, 0x13, 0x03, 0xc0, 0x80, 0x91,
    0xc6, 0x00, 0x0a, 0xc0, 0xe0, 0x91, 0x43, 0x01, 0xf0, 0xe0,
    0x90, 0x91, 0xc6, 0x00, 0xec, 0x5b, 0xfe, 0x4f, 0x90, 0x83,
    0x80, 0x93, 0x43, 0x01, 0xff, 0x91, 0xef, 0x91, 0x9f, 0x91,
    0x8f, 0x91, 0x2f, 0x91, 0x0f, 0x90, 0x0f, 0xbe, 0x0f, 0x90,
    0x1f, 0x90, 0x18, 0x95, 0x90, 0x91, 0xc0, 0x00, 0x95, 0xff,
    0xfc, 0xcf, 0x80, 0x93, 0xc6, 0x00, 0x08, 0x95, 0xcf, 0x93,
    0xdf, 0x93, 0xec, 0x01, 0x89, 0x91, 0x88, 0x23, 0x19, 0xf0,
    0x0e, 0x94, 0x8e, 0x00, 0xfa, 0xcf, 0xdf, 0x91, 0xcf, 0x91,
    0x08, 0x95, 0x10, 0x92, 0x80, 0x00, 0x25, 0x9a, 0x2d, 0x98,
    0xf8, 0x94, 0x10, 0x92, 0x6f, 0x00, 0x08, 0x95, 0x2d, 0x9a,
    0x87, 0xe3, 0x91, 0xe0, 0x0e, 0x94, 0x95, 0x00, 0x10, 0x92,
    0x85, 0x00, 0x10, 0x92, 0x84, 0x00, 0x8f, 0xef, 0x9f, 0xef,
    0x90, 0x93, 0x89, 0x00, 0x80, 0x93, 0x88, 0x00, 0x82, 0xe0,
    0x80, 0x93, 0x6f, 0x00, 0x78, 0x94, 0x8d, 0xe0, 0x80, 0x93,
    0x81, 0x00, 0x08, 0x95, 0x1f, 0x92, 0x0f, 0x92, 0x0f, 0xb6,
    0x0f, 0x92, 0x11, 0x24, 0x2d, 0x98, 0x10, 0x92, 0x81, 0x00,
    0xf8, 0x94, 0x10, 0x92, 0x6f, 0x00, 0x0f, 0x90, 0x0f, 0xbe,
    0x0f, 0x90, 0x1f, 0x90, 0x18, 0x95, 0x4d, 0xe4, 0x56, 0xee,
    0x60, 0xe4, 0x7b, 0xeb, 0xfc, 0x01, 0x40, 0x83, 0x51, 0x83,
    0x62, 0x83, 0x73, 0x83, 0x08, 0x95, 0x8f, 0x92, 0x9f, 0x92,
    0xaf, 0x92, 0xbf, 0x92, 0xcf, 0x92, 0xdf, 0x92, 0xef, 0x92,
    0xff, 0x92, 0x0f, 0x93, 0x1f, 0x93, 0xcf, 0x93, 0xdf, 0x93,
    0xec, 0x01, 0x14, 0x2f, 0x06, 0x2f, 0xfb, 0x01, 0x8e, 0x2f,
    0x80, 0x1b, 0x81, 0x17, 0x20, 0xf5, 0xa1, 0x91, 0xc8, 0x80,
    0xd9, 0x80, 0xea, 0x80, 0xfb, 0x80, 0xb0, 0xe0, 0x26, 0xed,
    0x37, 0xe8, 0x47, 0xe2, 0x53, 0xe2, 0x0e, 0x94, 0x9d, 0x01,
    0x46, 0x01, 0x57, 0x01, 0x2d, 0xe0, 0xb6, 0x94, 0xa7, 0x94,
    0x97, 0x94, 0x87, 0x94, 0x2a, 0x95, 0xd1, 0xf7, 0xdc, 0x01,
    0xcb, 0x01, 0x88, 0x25, 0x99, 0x25, 0xaa, 0x25, 0xbb, 0x25,
    0xc8, 0x0e, 0xd9, 0x1e, 0xea, 0x1e, 0xfb, 0x1e, 0xc8, 0x82,
    0xd9, 0x82, 0xea, 0x82, 0xfb, 0x82, 0xd8, 0xcf, 0xdf, 0x91,
    0xcf, 0x91, 0x1f, 0x91, 0x0f, 0x91, 0xff, 0x90, 0xef, 0x90,
    0xdf, 0x90, 0xcf, 0x90, 0xbf, 0x90, 0xaf, 0x90, 0x9f, 0x90,
    0x8f, 0x90, 0x08, 0x95, 0x0f, 0x93, 0x1f, 0x93, 0xfc, 0x01,
    0x40, 0x81, 0x51, 0x81, 0x62, 0x81, 0x73, 0x81, 0x8a, 0x01,
    0x9b, 0x01, 0x81, 0xe1, 0x00, 0x0f, 0x11, 0x1f, 0x22, 0x1f,
    0x33, 0x1f, 0x8a, 0x95, 0xd1, 0xf7, 0x40, 0x27, 0x51, 0x27,
    0x62, 0x27, 0x73, 0x27, 0x40, 0x83, 0x51, 0x83, 0x62, 0x83,
    0x73, 0x83, 0x1f, 0x91, 0x0f, 0x91, 0x08, 0x95, 0xcf, 0x93,
    0xdf, 0x93, 0x00, 0xd0, 0x00, 0xd0, 0x00, 0xd0, 0xcd, 0xb7,
    0xde, 0xb7, 0x83, 0xe0, 0x8d, 0x83, 0x0e, 0x94, 0xa1, 0x00,
    0x0e, 0x94, 0x53, 0x00, 0x84, 0xe0, 0x91, 0xe0, 0x0e, 0x94,
    0x95, 0x00, 0xce, 0x01, 0x01, 0x96, 0x0e, 0x94, 0xd0, 0x00,
    0x20, 0xe0, 0x90, 0x91, 0x42, 0x01, 0x80, 0x91, 0x43, 0x01,
    0x98, 0x17, 0xd1, 0xf3, 0xe0, 0x91, 0x42, 0x01, 0xf0, 0xe0,
    0xec, 0x5b, 0xfe, 0x4f, 0x30, 0x81, 0x3e, 0x83, 0x80, 0x91,
    0x42, 0x01, 0x90, 0xe0, 0x01, 0x96, 0x8f, 0x73, 0x99, 0x27,
    0x80, 0x93, 0x42, 0x01, 0x3a, 0x30, 0x11, 0xf0, 0x3d, 0x30,
    0x51, 0xf5, 0x22, 0x23, 0x29, 0xf3, 0xce, 0x01, 0x01, 0x96,
    0x0e, 0x94, 0x1f, 0x01, 0x40, 0xe0, 0x50, 0xe0, 0x2d, 0x81,
    0x84, 0x2f, 0x04, 0x2e, 0x00, 0x0c, 0x99, 0x0b, 0x30, 0xe0,
    0x82, 0x17, 0x93, 0x07, 0x7c, 0xf4, 0xe1, 0xe0, 0xf0, 0xe0,
    0xec, 0x0f, 0xfd, 0x1f, 0xe8, 0x0f, 0xf9, 0x1f, 0xdc, 0x01,
    0xa0, 0x50, 0xbf, 0x4f, 0x20, 0x81, 0x8c, 0x91, 0x28, 0x13,
    0x51, 0xe0, 0x4f, 0x5f, 0xe8, 0xcf, 0x51, 0x11, 0x03, 0xc0,
    0x0e, 0x94, 0xa9, 0x00, 0xb9, 0xcf, 0x81, 0xe2, 0x91, 0xe0,
    0x0e, 0x94, 0x95, 0x00, 0xb4, 0xcf, 0x41, 0xe0, 0xbe, 0x01,
    0x6a, 0x5f, 0x7f, 0x4f, 0xce, 0x01, 0x01, 0x96, 0x0e, 0x94,
    0xda, 0x00, 0x21, 0xe0, 0xb3, 0xcf, 0x0e, 0x94, 0xa8, 0x01,
    0xa5, 0x9f, 0x90, 0x0d, 0xb4, 0x9f, 0x90, 0x0d, 0xa4, 0x9f,
    0x80, 0x0d, 0x91, 0x1d, 0x11, 0x24, 0x08, 0x95, 0xa2, 0x9f,
    0xb0, 0x01, 0xb3, 0x9f, 0xc0, 0x01, 0xa3, 0x9f, 0x70, 0x0d,
    0x81, 0x1d, 0x11, 0x24, 0x91, 0x1d, 0xb2, 0x9f, 0x70, 0x0d,
    0x81, 0x1d, 0x11, 0x24, 0x91, 0x1d, 0x08, 0x95, 0xf8, 0x94,
    0xff, 0xcf, 0xb2, 0x8f, 0xf9, 0xa2, 0x50, 0x6c, 0x65, 0x61,
    0x73, 0x65, 0x20, 0x65, 0x6e, 0x74, 0x65, 0x72, 0x20, 0x74,
    0x68, 0x65, 0x20, 0x70, 0x61, 0x73, 0x73, 0x77, 0x6f, 0x72,
    0x64, 0x3a, 0x0d, 0x0a, 0x00, 0x49, 0x6e, 0x63, 0x6f, 0x72,
    0x72, 0x65, 0x63, 0x74, 0x20, 0x70, 0x61, 0x73, 0x73, 0x77,
    0x6f, 0x72, 0x64, 0x21, 0x0d, 0x0a, 0x00, 0x53, 0x55, 0x43,
    0x43, 0x45, 0x53, 0x53, 0x21, 0x0d, 0x0a, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

void set_sp(avr_t *avr, uint16_t sp)
{
	// Stack pointer is a 16-bit value at 0x5d
	*(uint16_t *)&avr->data[R_SPL] = sp;
}

uint16_t get_sp(avr_t *avr)
{
	// Stack pointer is 16-bit value at 0x5d
	return *(uint16_t *)&avr->data[R_SPL];
}

avr_t *avr_core_init(void)
{
	avr_t *avr;
	// Set up the AVR target
	avr = avr_make_mcu_by_name("atmega328p");
	if (!avr) {
		fprintf(stderr, "unable to create avr object.\n");
		exit(1);
	}
	avr_init(avr);
	avr->frequency = 16000000;
	avr->vcc = 5000;
	avr->avcc = 5000;

	// Read in the program memory contents
	memcpy(avr->flash, prog_mem, _PROG_MEM_SIZE);
	return avr;
}

uint32_t calculate_hash(avr_t *avr, const char *password)
{
	uint32_t addr; 
	uint32_t last_pc;
	const char *pw_start = password;  // remember where we started 
	int state;

	// Initialize ring buffer read offset to 0 (at 0x142)
	avr->data[0x142]=0x00;
	
	// Init ring buffer write offset to 0xFF so it never equals read offset(at 0x143)
	avr->data[0x143]=0xFF;
	
	// Store password at ring buffer location (starts at 0x144)
	// Store until end of buffer
	for (addr=0x144; addr<0x183; addr++){
		avr->data[addr]=*password;   // Store byte of password to ring buffer
		password++;                  // Move to next character
	}
	password=pw_start;   // Reset password to original address
	
	// Set Stack pointer to 0x200
    set_sp(avr, 0x200); 
    // Init [SP+0x5]=4 for the number of bytes that hash will be compared with
    avr->data[get_sp(avr)+5]=0x4;

    // Set Y=0x200 (set to SP in main() )
    avr->data[28]=0x00;  // Low byte
    avr->data[29]=0x02;  // High byte

	// Set starting address for execution
	// Program counter is a *byte* address for simavr
	avr->pc = 0x296;  // After password prompt: 0x296
	
	// Run until an ending address that you define
	// Again, this is a *byte* address for simavr
	state = avr->state;
	while ((state != cpu_Done) && (state != cpu_Crashed) &&
	       (avr->pc != 0x2de)) {  // last working: 0x2ba   Very End:0x2de    current:0x2ba 
		state = avr_run(avr);
		last_pc=avr->pc;
	}
	if (state == cpu_Crashed)
		printf("CPU crashed: PC==%x\n", last_pc);

		
    // So calculated hash is  SP + 0x1 = 0x900
	// Retrieve the hash value from memory and return it

	return *(uint32_t *)&avr->data[get_sp(avr)+1];
}


/*
	Iterates through ALL possible products of characters '0'-'z' up to infinite indices
*/
void new_password(int *element, char *password)
{
	char *start=password;

	if ((*element)<=10){                  // no more than 3 elements for now
		if (current == prev)                 // If we aren't on the main counting index
			current=(*element);              // Start at the last index
		password[current]++;    		 	 // Increment current element
		if (password[0] != '{'){             // Loop until 0th index hits last character
			if (password[current] == '{'){   // Check if current element reached end
				password[current]='0';       // Reset curr element to beginning
				prev=current;                // Save the current				
				// Need to take care of the element before us
				if ((current))               // Make sure we aren't on 0th index
					current=current-1;       // New current is the previous element
			}
			else if (current != (*element)){  // If not on last index
				current=(*element);	          // Go back to the last index
				prev=current;                 // No need for a previous at the moment
			}
		}
	 	else if (password[0] == '{'){            // if 0th index reached end
	 		password[0] = 48; 					 // Reset 0th index back to '0'
	 		password[*(element)+1] = 48; 		 // Reset index to '0' from '\n'
			password[*(element)+2] = 47; 		 // Reset index to '\' from x0
	 		(*element)++;	                     // Need to add an element since we reset 
	 		current=prev;
		}
	}

	password=start;
	// Add newline and null
	password[(*element)+1]='\n';
	password[(*element)+2]=0;
	//printf("%s",password);

}

int main(int argc, char **argv)
{
	unsigned long long counter;
	uint32_t hash = 0, wanted_hash = 0xa2f98fb2;
	avr_t *avr;

	char password[256] = {47,47,47,47,47,47,47,47,47,47};   // init first 10 characters to '\'
	int element=0;
	avr = avr_core_init();

	counter = 0;
	// Mask hashes with 0xffffff to only check lower 24 bits
	// Can modify this to only check lower 16 bits for passwords i know ('4Xi\n')
	while ((hash & 0xffffff) != (wanted_hash & 0xffffff)) {
		new_password(&element, password);
		hash = calculate_hash(avr, password);
		counter++;
		if (counter % 10000 == 0) {
			printf("Iteration %llu; Password %s; Hash 0x%08x\n",
			       counter, password, hash);
		}
	}

	printf("Found a match after %llu attempts!\n", counter);
	printf("Password: %s\n", password);
	printf("Hash: 0x%08x\n", hash);

	return 0;
}
