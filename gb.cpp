#include "gb.hpp"
#include<fstream>
#include<iostream>

void gb::LoadROM(char const* filename) {
	
	//Open the file at the end to find out the size of the rom file 	
	std::ifstream rom_file(filename, std::ios::binary | std::ios::ate);

	if(rom_file.is_open()) {
	
		std::streampos size = rom_file.tellg(); //This returns the number of bytes, how many bytes after which the
	       						//the get pointer is referring to, essentially here its pointing to
							//the end of the file, so it will be the size of the file.
	

		//Point to the beginning of the file
		rom_file.seekg(0,std::ios::beg);

		//Create a buffer for reading the rom file
		char * buffer = new char[size];

		//Read from the beginning of the file into the buffer
		rom_file.read(buffer,size); //Which buffer to put the values into and till which byte, here till the end
		
		rom_file.close(); //Close the rom file now

		//Currently we are assuming no MBC, so we are directly putting the values into the memory
		//TODO: Must include Memory Banking when reading the file, maximum of only 16KB
	
		long address;

		//Loading the first 16KiB memory from Catridge into the dedicated internal Memory of GameBoy
		//or the entire catridge memory into the Internal memory, whichever is smaller
		for(address=0; (address <= 0x3FFF && address < size); address++){
			Memory[address] = buffer[address];
			Catridge_Memory[address] = buffer[address];
		}

		//The next 16KiB is loaded into the internal memory from 0x4000 to 0x7FFF, or the rest of the rom is loaded
		//whichever is smaller
		for(; (address <= 0x7FFF && address < size); address++){
			Memory[address] = buffer[address];
			Catridge_Memory[address] = buffer[address];
		}

		for(; (address < size && address < 0x200000); address++) {
			
			Catridge_Memory[address] = buffer[address];
		
		}
			
		delete[] buffer;	
	}
	

}

gb::gb() {

	//Initializing Values to registers, PC, Stack Pointer, Internal Memory 

	PC=0x100;

	rA = 0x01;
	rF = 0xB0;
	rB = 0x00;
	rC = 0x13;
	rD = 0x00;
	rE = 0xD8;
	rH = 0x01;
	rL = 0x4D;
	
	SP = 0xFFFE;

	Memory[0xFF05] = 0x00 ;
	Memory[0xFF06] = 0x00 ;
	Memory[0xFF07] = 0x00 ;
	Memory[0xFF10] = 0x80 ;
	Memory[0xFF11] = 0xBF ;
	Memory[0xFF12] = 0xF3 ;
	Memory[0xFF14] = 0xBF ;
	Memory[0xFF16] = 0x3F ;
	Memory[0xFF17] = 0x00 ;
	Memory[0xFF19] = 0xBF ;
	Memory[0xFF1A] = 0x7F ;
	Memory[0xFF1B] = 0xFF ;
	Memory[0xFF1C] = 0x9F ;
	Memory[0xFF1E] = 0xBF ;
	Memory[0xFF20] = 0xFF ;
	Memory[0xFF21] = 0x00 ;
	Memory[0xFF22] = 0x00 ;
	Memory[0xFF23] = 0xBF ;
	Memory[0xFF24] = 0x77 ;
	Memory[0xFF25] = 0xF3 ;
	Memory[0xFF26] = 0xF1 ;
	Memory[0xFF40] = 0x91 ;
	Memory[0xFF42] = 0x00 ;
	Memory[0xFF43] = 0x00 ;
	Memory[0xFF45] = 0x00 ;
	Memory[0xFF47] = 0xFC ;
	Memory[0xFF48] = 0xFF ;
	Memory[0xFF49] = 0xFF ;
	Memory[0xFF4A] = 0x00 ;
	Memory[0xFF4B] = 0x00 ;
	Memory[0xFFFF] = 0x00 ;


	Current_ROM_Bank = 1;

	//Establishing the indirections required to decode the opcode
	
	table[0x0] = &gb::Table00;
	table[0x1] = &gb::Table01;
	table[0x2] = &gb::Table10;
	table[0x3] = &gb::Table11;


	//NOP
	
	table00[0x0] = &gb::nop;

	
	//LD u16, SP

	table00[0x8] = &gb::opcode__0x08__;
	

	//STOP

	table00[0x10] = &gb::opcode__0x10__;


	//JR (unconditional)

	table00[0x18] = &gb::opcode__0x18__;


	//JR (conditional)

	table00[0b00100000] = &gb::opcode__001_cond_000__;
	table00[0b00101000] = &gb::opcode__001_cond_000__;
	table00[0b00110000] = &gb::opcode__001_cond_000__;
	table00[0b00111000] = &gb::opcode__001_cond_000__;


	//LD r16, u16

	table00[0b00000001] = &gb::opcode__00_r16(gp1)_0001__;
	table00[0b00010001] = &gb::opcode__00_r16(gp1)_0001__;
	table00[0b00100001] = &gb::opcode__00_r16(gp1)_0001__;
	table00[0b00110001] = &gb::opcode__00_r16(gp1)_0001__;


	//ADD HL, r16

	table00[0b00001001] = &gb::opcode__00_r16(gp1)_1001__;
	table00[0b00011001] = &gb::opcode__00_r16(gp1)_1001__;
	table00[0b00101001] = &gb::opcode__00_r16(gp1)_1001__;
	table00[0b00111001] = &gb::opcode__00_r16(gp1)_1001__;


	//LD r16, A

	table00[0b00000010] = &gb::opcode__00_r16(gp2)_0010__;
	table00[0b00010010] = &gb::opcode__00_r16(gp2)_0010__;
	table00[0b00100010] = &gb::opcode__00_r16(gp2)_0010__;
	table00[0b00110010] = &gb::opcode__00_r16(gp2)_0010__;


	//LD A, r16

	table00[0b00001010] = &gb::opcode__00_r16(gp2)_1010__;
	table00[0b00011010] = &gb::opcode__00_r16(gp2)_1010__;
	table00[0b00101010] = &gb::opcode__00_r16(gp2)_1010__;
	table00[0b00111010] = &gb::opcode__00_r16(gp2)_1010__;


	//INC r16

	table00[0b00000011] = &gb::opcode__00_r16(gp1)_0011__;
	table00[0b00010011] = &gb::opcode__00_r16(gp1)_0011__;
	table00[0b00100011] = &gb::opcode__00_r16(gp1)_0011__;
	table00[0b00110011] = &gb::opcode__00_r16(gp1)_0011__;

	//DEC r16

	table00[0b00001011] = &gb::opcode__00_r16(gp1)_1011__;
	table00[0b00011011] = &gb::opcode__00_r16(gp1)_1011__;
	table00[0b00101011] = &gb::opcode__00_r16(gp1)_1011__;
	table00[0b00111011] = &gb::opcode__00_r16(gp1)_1011__;

	//INC r8

	table00[0b00000100] = &gb::opcode__00_r8_100__;
	table00[0b00001100] = &gb::opcode__00_r8_100__;
	table00[0b00010100] = &gb::opcode__00_r8_100__;
	table00[0b00011100] = &gb::opcode__00_r8_100__;
	table00[0b00100100] = &gb::opcode__00_r8_100__;
	table00[0b00101100] = &gb::opcode__00_r8_100__;
	table00[0b00110100] = &gb::opcode__00_r8_100__;
	table00[0b00111100] = &gb::opcode__00_r8_100__;


	//DEC r8

	table00[0b00000101] = &gb::opcode__00_r8_101__;
	table00[0b00001101] = &gb::opcode__00_r8_101__;
	table00[0b00010101] = &gb::opcode__00_r8_101__;
	table00[0b00011101] = &gb::opcode__00_r8_101__;
	table00[0b00100101] = &gb::opcode__00_r8_101__;
	table00[0b00101101] = &gb::opcode__00_r8_101__;
	table00[0b00110101] = &gb::opcode__00_r8_101__;
	table00[0b00111101] = &gb::opcode__00_r8_101__;


	//LD r8, u8

	table00[0b00000110] = &gb::opcode__00_r8_110__;
	table00[0b00001110] = &gb::opcode__00_r8_110__;
	table00[0b00010110] = &gb::opcode__00_r8_110__;
	table00[0b00011110] = &gb::opcode__00_r8_110__;
	table00[0b00100110] = &gb::opcode__00_r8_110__;
	table00[0b00101110] = &gb::opcode__00_r8_110__;
	table00[0b00110110] = &gb::opcode__00_r8_110__;
	table00[0b00111110] = &gb::opcode__00_r8_110__;


	//Accumulator/Flag Operations

	table00[0b00000111] = &gb::opcode__00_rlca_111__;
	table00[0b00001111] = &gb::opcode__00_rrca_111__;
	table00[0b00010111] = &gb::opcode__00_rla_111__;
	table00[0b00011111] = &gb::opcode__00_rra_111__;
	table00[0b00100111] = &gb::opcode__00_daa_111__;
	table00[0b00101111] = &gb::opcode__00_cpl_111__;
	table00[0b00110111] = &gb::opcode__00_scf_111__;
	table00[0b00111111] = &gb::opcode__00_ccf_111__;

	return;	

}

void gb::Table00() {


}

void gb::Table01() {


}

void gb::Table10() {


}

void gb::Table11() {


}

WORD gb::RegisterAF() {

	WORD AF =  (rA << 8u) | (rF);

	return AF;
}

WORD gb::RegisterBC() {

	WORD BC =  (rB << 8u) | (rC);

	return BC;
}

WORD gb::RegisterDE() {

	WORD DE =  (rD << 8u) | (rE);

	return DE;
}

WORD gb::RegisterHL() {

	WORD HL =  (rH << 8u) | (rL);

	return HL;
}


void gb::Cycle() {

	//read the first byte of the next instruction to figure out what is the opcode using SM83 encoding table
	opcode = memory[PC];
	PC = PC + 1;

	//Fetch takes 4 clock cycles
	clock_cycles += 4;

	//Picking the first two bits of the opcode to enable the first level of decoding
	(this->*table[(opcode & 0xC0u) >> 6u]) ();


	//Provide Delay, each opcode takes a variable amount of clock cyles to execute
	//Let's say it takes x clock cycles
	//So we have to provide a delay of x clock cycles before executing the next instruction
	//This x will be stored in a variable called clock_cycles
	//and delay based on the value stored in it
	//fetching an opcode (accessing memory) always takes 4 clock_cycles and then executing could
	//take another x-4 clock_cycles

	Delay(clock_cycles);	
       		

	//resetting to zero clock cycles for the next instruction
	clock_cycles = 0;

	return;

}

void gb::Delay() {


}


//Opcode Implementation Follows Below:
//The general format of which opcode is implemented is between the double underscores 
//Eg: gb::opcode__'opcode format'__()
//gb::opcode__001_cond_000__()
//This implements the opcode whose byte format is as 001 cond 000
//cond i.e condition is 2 bits in length

//Beginning of Table00 opcodes
//i.e 8th (MSB) and 7th bits of the byte

//NOP
void gb::nop() {


}

//LD u16, SP
void gb::opcode__0x08__() {


}

//STOP
void gb::opcode__0x10__() {


}

//JR (unconditional)
void gb::opcode__0x18__() {

}

//JR (conditional)
void gb::opcode__001_cond_000__() {

}

//LD r16,u16
void gb::opcode__00_r16(gp1)_0001__() {

}

//ADD HL,r16
void gb::opcode__00_r16(gp1)_1001__() {
	
}

//LD r16,A
void gb::opcode__00_r16(gp2)_0010__() {

}

//LD A,r16
void gb::opcode__00_r16(gp2)_1010__() {

}

//INC r16
void gb::opcode__00_r16(gp1)_0011__() {

}

//DEC r16
void gb::opcode__00_r16(gp1)_1011__() {

}

//INC r8
void gb::opcode__00_r8_100__() {

}

//DEC r8
void gb::opcode__00_r8_101__() {

}

//LD r8,u8
void gb::opcode__00_r8_110__() {

}

//Accumulator/Flag operations

void gb::opcode__00_rlca_111__() {

}

void gb::opcode__00_rrca_111__() {

}

void gb::opcode__00_rla_111__() {

}

void gb::opcode__00_rra_111__() {

}

void gb::opcode__00_daa_111__() {

}

void gb::opcode__00_cpl_111__() {

}

void gb::opcode__00_scf_111__() {

}

void gb::opcode__00_ccf_111__() {

}

//End of Table00 opcodes
