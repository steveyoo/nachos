#include "syscall.h"

// [10 pts] Write test programs that test multiprogramming: Exec, Exit, and your AddrSpace implementation. 
// You should make sure that Exec cleanly handles problem cases where the program string argument is invalid 
// for some reason -- bad string address, does not end in a null character, and specifies a filename that does 
// not exist. You should also test the boundaries of AddrSpace -- trying load a program that doesn't fit into 
// physical memory, being able to load many programs one after the other (i.e., AddrSpace releases memory when
//  a process goes away so it can be reused again).


// NumPhysPages  128


int main(){


	// this call should be falil because thread does not exist.
	// Exec("../test/thread", 0, 0, 0);
	// Yield();


	// this one should print out a statement." file is too big to be load"
	// and return 0
	Exec("../test/outmemorytest", 0, 0, 0);
	Yield();

	// Exec should find the path and excute the Exit();
	// excution is sucessed
	Exec("../test/exectest",0,0,0);
}