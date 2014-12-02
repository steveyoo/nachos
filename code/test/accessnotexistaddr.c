#include "syscall.h"

// test for AddressErrorException 
// this test should fail immediately and stop the main process then the machine stops
int addr(){
	 char* addr = 100000000;
	 char reference = *addr;
}

int main(){
	addr();
	addr();
	addr();
	Exit(1);
}