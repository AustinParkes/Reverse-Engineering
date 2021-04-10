#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Macros to assist with quick computations
#define BYTE1(var)  ((var)&0xFF)   		// Gets 1st byte of a region
#define BYTE2(var)	(((var)>>8)&0xFF)     // Gets 2nd byte of a region
#define BYTE3(var)	(((var)>>16)&0xFF)    // Gets 3rd byte of a region
#define BYTE4(var)	(((var)>>24)&0xFF)    // Gets 4th byte of a region

#define LSHIFT1(var) ((var)<<8)    // Shift left by 1 byte
#define LSHIFT2(var) ((var)<<16)   // Shift left by 2 bytes
#define LSHIFT3(var) ((var)<<24)   // Shift left by 3 bytes

#define CLEARWhi(var) ((var)&0xFF00FFFF) // Clear Whi byte in var1
#define CLEARWlo(var) ((var)&0xFFFF00FF) // Clear Wlo byte in var1
#define CLEARR23(var) ((var)&0xFFFFFF00) // Clear R23 byte in var1


// Going to store hash value here and update it as we go
// Use bytes for ease of computation
uint8_t hash[] = {0,0,0,0};

// To keep track of elements in password generator
int current=0;
int prev=0;

void hash_init(){
	// hash = 0xbb40e64d (stored little endian)
	hash[0] = 0x4d; // R12
	hash[1] = 0xe6; // R13
	hash[2] = 0x40; // R14
	hash[3] = 0xbb;	// R15
}

// Should process one byte
void hash_update(uint8_t byte){
	//printf("%c\n", byte);
	
	uint32_t counter;
	
	uint32_t R8;
	uint32_t R9;
	uint32_t R10;
	uint32_t R11;
	uint32_t shift_reg;
	uint32_t hash32;

	// Register mnemonics to help read & write
	uint32_t R0;                
	uint32_t R1;
	uint32_t R18;
	uint32_t R19;
	uint32_t R20;
	uint32_t R21;
		
	uint32_t R22;
	uint32_t R23;
	uint32_t Wlo;
	uint32_t Whi;
	uint32_t Xlo;
	uint32_t Xhi;
	uint16_t mul;              // store multiplication results
	uint32_t var1;             // Whi:Wlo:R23
	


	// Happens right before function calls
	Xlo = byte;				   // 0xee
	//printf("byte: %c\n", (char)Xlo);
	Xhi = 0x0;				   // 0xf3
	
	// 0xf4-0xf7
	R18 = 0xd6;
	R19 = 0x87;
	R20 = 0x27;
	R21 = 0x23;
	
	/*
		Update1 -> Update2
	*/
	
	mul = Xlo*R18;      	// 0x1a8    Xlo*0xd6
	R22 = BYTE1(mul);       // 0x1a9    R22
	R23 = BYTE2(mul);       // 0x1a9    R23
	
	mul = Xhi*R19;      	// 0x1aa    Xhi*0x87
	Wlo = BYTE1(mul);       // 0x1ab	Wlo
	Whi = BYTE2(mul);       // 0x1ab	Whi
	
	mul = Xlo*R19;      	// 0x1ac    Xlo*0x87
	R0 = BYTE1(mul);        // 0x1ac    R0
	R1 = BYTE2(mul);        // 0x1ac    R1
	
	// init var1 with Whi:Wlo:R23
	var1 = LSHIFT2(Whi) + LSHIFT1(Wlo) + R23; // Whi:Wlo:R23
	
	// Update Whi:Wlo:R23
	var1 = var1 + R0;           // 0x1ad          R23 = R23 + R0
	var1 = var1 + LSHIFT1(R1);  // 0x1ae-0x1b0    Wlo = Wlo + R1 & Whi = Whi + C 

	mul = Xhi*R18;				// 0x1b1
	R0 = BYTE1(mul);
	R1 = BYTE2(mul);
	
	// Update Whi:Wlo:R23
	var1 = var1 + R0;			// 0x1b2          R23 = R23 + R0
	var1 = var1 + LSHIFT1(R1);  // 0x1b3-0x1b5    Wlo = Wlo + R1 & Whi = Whi + C 
	/*
		Update1
	*/
		
	mul = Xlo*R21;					// 0x195		R1:R0 = Xlo*0x23
	R0 = BYTE1(mul);            
	R1 = BYTE2(mul);		
	
	// Update Whi:Wlo:R23
	Whi = BYTE3(var1) + R0;
	var1 = CLEARWhi(var1) + LSHIFT2(Whi);	// 0x1a0		Whi = Whi + R0
	
	mul = Xhi*R20;					// 0x1a1        R1:R0 = Xhi * 0x27
	R0 = BYTE1(mul);            
	R1 = BYTE2(mul);
		
	// Update Whi:Wlo:R23
	Whi = BYTE3(var1) + R0;	
	var1 = CLEARWhi(var1) + LSHIFT2(Whi);	// 0x1a2		Whi = Whi + R0
		
	mul = Xlo*R20;					// 0x1a3        R1:R0 = Xlo * 0x27
	R0 = BYTE1(mul);            
	R1 = BYTE2(mul);	
	
	var1 = var1 + LSHIFT1(R0);		// 0x1a4		Wlo = Wlo + R0 (Takes care of carry on next instruction)
	var1 = var1 + LSHIFT2(R1);		// 0x1a5		Whi = Whi + R1 + C
	R1 = 0;
	
	// Move hash into temporary registers
	// HASH STORED LITTLE ENDIAN IN AVR
	R8 = hash[0];					// 0xfa
	R9 = hash[1];					// 0xfa
	R10 = hash[2];					// 0xfb
	R11 = hash[3];					// 0xfb
	
	// R11:R10:R9:R8
	shift_reg = (LSHIFT3(R11) + LSHIFT2(R10) + LSHIFT1(R9) + R8);	// 0xfd-0x100 goes from R11 downto R8
	
	counter = 0xd;					// 0xfc	R18 = 0xd
	
	// 0xfd-0x101
	while (counter != 0){
		shift_reg = shift_reg>>1;	
		counter--;
	}
	
	// Restore R8:R9:R10:R11
	R8 = BYTE1(shift_reg);
	R9 = BYTE2(shift_reg);
	R10 = BYTE3(shift_reg);
	R11 = BYTE4(shift_reg);	
	
	Xhi = BYTE3(var1);									// 0x103 	Xhi = Whi	
	Xlo = BYTE2(var1);									// 0x103 	Xlo = Wlo	
	
	R23 = BYTE1(var1);
	var1 = CLEARWhi(var1) + LSHIFT2(R23);				// 0x104	Whi = R23
	var1 = CLEARWlo(var1) + LSHIFT1(R22);				// 0x104	Wlo = R22

	Wlo = BYTE2(var1);
	var1 = CLEARWlo(var1) + LSHIFT1((Wlo^R8));			// 0x105	Wlo = Wlo ^ R8
	Whi = BYTE3(var1);
	var1 = CLEARWhi(var1) + LSHIFT2((Whi^R9));	// 0x106 	Whi = Whi ^ R9

	Xlo = (Xlo^R10);					// 0x107
	Xhi = (Xhi^R11);					// 0x108
	
	// R15:R12 (hash) for ease of addition
	hash32 = LSHIFT3(hash[3]) + LSHIFT2(hash[2]) + LSHIFT1(hash[1]) + hash[0];
	
	// Do it this way so carries go over
	hash32 = hash32 + BYTE2(var1);			// 0x109 	R12 = R12 + Wlo
	Whi = BYTE3(var1);
	hash32 = hash32 + LSHIFT1(Whi);	// 0x10a 	R13 = R13 + Whi
	hash32 = hash32 + LSHIFT2(Xlo);			// 0x10b	R14 = R14 + Xlo
	hash32 = hash32 + LSHIFT3(Xhi);			// 0x10c	R15 = R15 + Xhi
	//printf("hash_update end: 0x%x\n", hash32);
	// Store hash back into memory
	hash[0] = BYTE1(hash32); 				// 0x10d	R12
	hash[1] = BYTE2(hash32);				// 0x10e	R13
	hash[2] = BYTE3(hash32);				// 0x10f	R14
	hash[3] = BYTE4(hash32);				// 0x110	R15
	
}

uint32_t hash_finalize(){
	
	// Registers for ease of comparing to avr code
	uint32_t R16, R17, R18, R19;
	uint32_t R20, R21, R22, R23;
	
	uint32_t register32=0; // R19:R16
	uint32_t counter;
	
	// To compare to avr code easier
	// 0x122 - 0x125
	R20 = hash[0];
	R21 = hash[1];
	R22 = hash[2];
	R23 = hash[3];
	
	// To compare to avr code easier
	// 0x127-0x128
	R16 = R20;
	R17 = R21;
	R18 = R22;
	R19 = R23;
	
	// R19:R16		
	register32 = (LSHIFT3(R19) + LSHIFT2(R18) + LSHIFT1(R17) + R16);	
	
	counter=0x11;								// 0x128
	while (counter != 0){
		register32 = register32 + R16;			// 0x129	R16 = R16 + R16
		register32 = register32 + LSHIFT1(R17);	// 0x12a	R17 = R17 + R17
		register32 = register32 + LSHIFT2(R18);	// 0x12b	R18 = R18 + R18
		register32 = register32 + LSHIFT3(R19);	// 0x12c	R19 = R19 + R19
		counter--;								// 0x12d
	}
	
	// Ease of reading
	R16 = BYTE1(register32);
	R17 = BYTE2(register32);
	R18 = BYTE3(register32);
	R19 = BYTE4(register32);
	
	// To compare to avr code easier
	R20 = R20^R16;								// 0x12f	R20 = R20^R16
	R21 = R21^R17;								// 0x130	R21 = R21^R17
	R22 = R22^R18;								// 0x131	R22 = R20^R18
	R23 = R23^R19;								// 0x132	R23 = R20^R19
	
	hash[0] = R20;								// 0x133	
	hash[1] = R21;								// 0x134
	hash[2] = R22;								// 0x135
	hash[3] = R23;								// 0x136
	
	// Reverse little endian and return hash
	return (LSHIFT3(hash[3]) + LSHIFT2(hash[2]) + LSHIFT1(hash[1]) + hash[0]);
}

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

void main(){
	uint32_t good_hash=0xa2f98fb2; 
	uint32_t final_hash;
	//char password[] = {47,47,47,47,47,47,47,47,47,47};
	char password[]="0FFqM\n";
	int element=0;
	uint64_t attempts=0;
	
	while ((final_hash&0xFFFFFF) != (good_hash&0xFFFFFF)){
		hash_init();
		//new_password(&element, password);

		int count=0;
		while ((password[count]) != '\n'){
			hash_update((uint8_t)password[count]);
			count++;
		}
	
		final_hash = hash_finalize();
		attempts++;
		
		// Stole this from Austin Roach's code
		if (attempts % 100000 == 0) {
			printf("Iteration %lu; Password %s; Hash 0x%08x\n",
			       attempts, password, final_hash);
		}
	}

	
	printf("Found a match after %lu attempts\n", attempts);
	printf("Password: %s\n", password);
	printf("final hash: 0x%x\n", final_hash);

}


