#pragma once

//Declaring data types for hving consistent declarations as GameBoy

typedef uint8_t BYTE;
typedef uint16_t WORD; //In Gameboy one word is like 16 bits, commonly its 32 bits

class gb {

	public :

		BYTE Catridge_Memory[0x200000]; //Gameboy catridge size is 0x200000 Bytes i.e 2MB
		BYTE Video_Memory[160][144][3];	//Screen size of GB is 160 x 144, 3 for rgb
		BYTE Memory[0x10000]; //64 KBytes of Internal Memory
		BYTE Current_ROM_Bank; //Which Memory bank is between 0x4000 and 0x7FFF 1,2,....
					// ROM address (i.e Internal Memory 0x0 to 0x7FFF)
					// From 0x0 to 0x7FFF, the catridge is loaded into Internal memory

		gb();
		void LoadROM(char const * filename);		
		void Cycle();		

	private:

		uint8_t clock_cycles{};

		BYTE rA{}; //Accumulator Register
		BYTE rB{};
		BYTE rC{};
		BYTE rD{};
		BYTE rE{};
		BYTE rF{};
		BYTE rH{};
		BYTE rL{}; //Flag Resgister

		WORD PC{};
		WORD SP{}; 

		WORD RegisterAF();
		WORD RegisterBC();
		WORD RegisterDE();
		WORD RegisterHL();

		WORD opcode{};


		//Provides Appropriate Delay Between Instructions to make it cycle accurate 
		void Delay();

		//Brace yourself for a long list of opcodes

		void nop();

	

		//Function Pointer declaration for the class member functions to make it easier to define
		typedef void (gb::*Opcode) ();


		//INFO: SM83 (GameBoy SoC) encoding can be boiled down to a byte 
		//We first look at the 8th and 7th bit, to determine which opcode it i
		//Then after that is determined, we we will direct it to some other pointer table which points to the 
		//actual opcode
		//These levels of indirection of pointers and decoding the bits helps to modularize the code
		//If individually implemented without this level of indirection of pointers, it becomes difficult
		//to code 500+ opcodes
		//This sort of indirection is also used in Virtual Memory and File Systems in OS to make our lives easier
		//and save space

		Opcode table[0x4u];
		Opcode table00[0x3Fu];	
		Opcode table01[0x3Fu];	
		Opcode table10[0x3Fu];	
		Opcode table11[0x3Fu];	


		//Functions for each of the Table to figure out which index to pick, resulting in picking the operation
		//Essentially decoding

		void Table00();
		void Table01();
		void Table10();
		void Table11();
};
