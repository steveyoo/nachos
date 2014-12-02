#include "syscall.h"
// test for killing the process
// when the user lelve process causes a exception divide by 0, 
// we dont want to stop runing the machine, we only want to kill
// the process.  add_array_1 will be called 3 times, these process 
// do cause the ceception, but our machine will not stop for thses three
// when add_array() sis call, the machine will stop because we have a 
// Exit(-1) which means the main prcoess is exited and no other process
// in memory.


//IllegalInstrException

void divi(){
   	int a = 100;
	int b = 0;
	int c = 100/0;
	Exit(-1);
}

void divi_2() {
    int a = 100;
	int b = 0;
	int c = 100/0;
}
int main(){
	divi_2();
	divi_2();
	divi_2();
	divi();
	Exit(1);
}