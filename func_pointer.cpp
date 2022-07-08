#include<iostream>

int Add(int a, int b) {
	return a + b;
}

int main() {

	int (*func) (int,int);

	func = Add;

	std::cout<<Add(3,4)<<"\n";
	std::cout<<func(5,6)<<"\n";

	int table[32];

	for(int i=0; i<32; i++) {
		table[i] = i;
	}

	for(int i=0; i<32; i++) {
		std::cout<<table[i]<<"\n";
	}


	std::cout<<table[0b00110];

	return 0;

}
