#include "syscall.h"
#include "system.h"
#include "table.h"

// Register #4 saves a pointer of type char * in virtual memory address,
// you need to translate that into physical memory by replacing the virtual
// page id in the address into physical page id. This string is a parameter 
// of the user program that calls the Exec, so you should use the page table of that process.
 
// You need to use register #4 to read the string out of the user program's memory,
//  so that you can know the file path to the executable.
 
// Once you get the file path, you can now create a new process with a
//  new address space, open the file and load the executable into the new address space.
 
// So, the value of the register #4, points you to a string, which points you to a file, 
// which further provides you the information on how the new process's memory should look like


void
intiUserProcess(int argv) {
    currentThread->space->InitRegisters();
    currentThread->space->RestoreState();
    machine->Run();
}

void userExit(){
	int spid = currentThread->spaceID;
	if(spid != 1){
		delete currentThread->space;
		tableManager->Release(spid);
		currentThread->Finish();
	} else 
		{
		// what to do if it is a parent thread?	
	}

	 printf("exiting the process \n");
     interrupt->Halt();

}

 
SpaceId userExec(){
	//SpaceId Exec(char *name, int argc, char **argv, int opt);
	int name;
	// int  argc, argv, opt;
	int pathLen = 0;
    char *path;
    OpenFile *executable;
    AddrSpace *space;
    Thread *thread;
    SpaceId spid = 0;
    
    //read the argument out from the Registers.
    name = machine->ReadRegister(4);
    // argc = machine->ReadRegister(5);
    // argv = machine->ReadRegister(6);
    // opt = machine->ReadRegister(7);
    // all data we have from the Registers are int
    // have to do coversion first
    // determine which type of the data is
    // int = 4 bit
    // int[] = 4 bit * len
    // char = 1 bit 
    // char[] = 1 bit * len
    // char*  = ???
    pathLen = getPathLen((char*)name);
    printf("path length is equal to %d\n",pathLen);
    if(pathLen <= 0) { 
    	printf(" no value path length find in userExec  line 43\n");
    	return 0;
    	// OR ASSERT(FALSE);
    }

    path = new char[pathLen];
    getPath(path, name, pathLen);

    executable = fileSystem->Open((char*) path);
    if (executable == NULL) {
    	printf("no such file in user_Exec line 53 \n");
    	return 0;
    }

    thread = new Thread("userExec Thread");
 	space = new AddrSpace();
 	if(space->Initialize(executable)) {
 		printf("create sapce for a new thread; line 59 in userExec");
 		thread->space = space;
 	} else{
 		printf("does not create sapce for a new thread..line 61 in userExec");
 		return 0;
 	}
 	delete executable;

 	spid = tableManager->Alloc((void*)thread);
 	printf("pid is --------------- :%d\n ", spid);
 	if (spid<=0)
 		return spid;
 	machine->WriteRegister(2, spid);
 	thread->spaceID = spid;
 	thread->Fork(intiUserProcess, 0);
 	currentThread->Yield();
 	//uptate the Register.
 	int pcreg = machine->ReadRegister(PCReg);

 	machine->WriteRegister(PrevPCReg, pcreg);
    machine->WriteRegister(PCReg, pcreg + 4);
    machine->WriteRegister(NextPCReg, pcreg + 8); 
  // hint from piazza.
  // First, PC should be always multiples of 4, since MIPS instructions are always 4-byte aligned.
  // Second, You also need to update the NextPCReg
	return spid;
}


// function getPathLen() gets the path len of first parameter of Exec
int getPathLen(char *name){
 	// assume the length max length of name is 64 bit;
 	int assumeMaxLen = 64;
 	int pathLen = 0;
 	int len = assumeMaxLen;
	int numPage = machine->pageTableSize;
	int argSize = numPage * PageSize;
	int check = 1;

	if ((int)name > argSize){
		printf("pointer address is bigger then user process address\n");
		return -1;
	}

	if(argSize - (int)name < assumeMaxLen){
		printf("pointer in the range\n");
		len = argSize - (int)name;
	}
	//assume the max length of the name(pointers to ) is 64 bit
	// -1 indicate we have access a invalid physciall address
	// 0 indicate name length is 0
	// else return the normal lenght
	for(int i = 0 ; i < len && check; i++) {
		if (machine->ReadMem((int)name + i, 1, &check))
			pathLen = (i + 1);
	}
	if( pathLen == 0) {
		printf("EXEC PASSES NO NAME");
		return pathLen;
	}else
		return pathLen;
}


// function getPath assigns the path of a given file, *name to *path)
void getPath(char *path , int name, int nameLen){
	int check;
	int i;
	printf("in getPath, the length is line 153. the path is:\n");
	for (i = 0; i < nameLen; i++) {
		if (machine->ReadMem(name+i, 1, &check))
			*(path + i) = check;
	printf ("value is %c\n", check);
	}
	*(path + i ) = 0;
}




