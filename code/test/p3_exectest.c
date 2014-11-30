#include "syscall.h"

// [10 pts] Write test programs that test multiprogramming: Exec, Exit, and your AddrSpace implementation. 
// You should make sure that Exec cleanly handles problem cases where the program string argument is invalid 
// for some reason -- bad string address, does not end in a null character, and specifies a filename that does 
// not exist. You should also test the boundaries of AddrSpace -- trying load a program that doesn't fit into 
// physical memory, being able to load many programs one after the other (i.e., AddrSpace releases memory when
//  a process goes away so it can be reused again).


// NumPhysPages  128
// every test case in "../test/*" has Exit() function in it,
// if Exit() not working, we cannot clear pysical memory, current thread' space and  table

// this test should excute to the  Exec("../test/outmemorytest", 0, 0, 0), and halts.

int main(){
	// this Exec should print out a statement "no such  no such file in user_Exec line 53" 
	// and continues to excute next Exec
	Exec("../test/noExistFile", 0, 0, 0);
	
	// Exec should find the path and excute the Exit();
	// excution is sucessed
	Exec("../test/exectest",0,0,0);

	// conitune to call Exec to run another problems 
	// it should works  becuase we have clear the physical memory, space and table
	// of by using the Exit in the "/test/exectest"
	Exec("../test/array",0,0,0);

	// conitune to call Exec to run another problems 
	// it should works  becuase we have clear the physical memory, space and table
	// of by using the Exit in the "/test/exectest"
	Exec("../test/array",0,0,0);

	// conitune to call Exec to run another problems 
	// it should works  becuase we have clear the physical memory, space and table
	// of by using the Exit in the "/test/exectest"
	Exec("../test/array",0,0,0);

	// conitune to call Exec to run another problems 
	// it should works  becuase we have clear the physical memory, space and table
	// of by using the Exit in the "/test/exectest"
	Exec("../test/array",0,0,0);

	// this Exec cause should print out a statement." file is too big to be load"
	// and half the process because we dont have enough memory
	Exec("../test/outmemorytest", 0, 0, 0);
}