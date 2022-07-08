#include<iostream>
#include "gb.cpp"
#include "gb.hpp"

int main(int argc, char * argv[]) {
	
	if(argc != 2) {
		std::cout<<"Please provide the rom file in the below format \n "<<argv[0]<<" <rom file>\n";
	}
	

	gb GameBoy;
	GameBoy.LoadROM(argv[1]);
/*	
	for(long i =0x100; i < 0x110; i++) { 
		
		std::cout<<int(GameBoy.Memory[i])<<"\n";
	}
*/

	return 0;
}
