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


void intiUserProcess(int argv) {
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
		ASSERT(currentThread->spaceID== 1);
        while(1 != tableManager->AnyExist()) {
               currentThread->Yield();
        } 
	}
	 printf("exiting the process \n");
     interrupt->Halt();
}

 
int userExec(){
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

    pathLen = getPathLen((char*)name);
    printf("path length is equal to %d\n",pathLen);
    if(pathLen <= 0) { 
    	printf(" no value path length find in userExec  line 43\n");
    	return 0;
    	//ASSERT(FALSE);
    }

    path = new char[pathLen];
    getPath(path, name, pathLen);

    executable = fileSystem->Open((char*) path);
    if (executable == NULL) {
    	printf("no such file in user_Exec line 53 \n");
    int pcreg = machine->ReadRegister(PCReg);
 	machine->WriteRegister(PrevPCReg, pcreg);
    machine->WriteRegister(PCReg, pcreg + 4);
    machine->WriteRegister(NextPCReg, pcreg + 8); 
    return 0;
    	//ASSERT(FALSE);
    }

    thread = new Thread("userExec Thread\n");
 	space = new AddrSpace();
 	if(space->Initialize(executable)) {
 		printf("Create sapce for a new thread; line 59 in userExec\n");
 		thread->space = space;
 	} else{
 		printf("Does not create sapce for a new thread..line 61 in userExec\n");
 		return 0;
 		//ASSERT(FALSE);
 	}
 	delete executable;

 	spid = tableManager->Alloc((void*)thread);
 	printf("pid is: %d name is: %s\n ", spid, thread->getName());
 	if (spid<=0)
 		return 0;
 		//ASSERT(false);
 	thread->spaceID = spid;
 	machine->WriteRegister(2, spid);
 	thread->Fork(intiUserProcess, 0);
 	currentThread->Yield();
 	//uptate the Register.
 	int pcreg = machine->ReadRegister(PCReg);
 	machine->WriteRegister(PrevPCReg, pcreg);
    machine->WriteRegister(PCReg, pcreg + 4);
    machine->WriteRegister(NextPCReg, pcreg + 8); 
    printf ("go to next excution\n");
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
		else {
			pathLen = -1;
			break;
		}
			
	}
	if( pathLen == 0) {
		printf("EXEC PASSES NO NAME\n");
		return pathLen;
	}else if (check){
		printf("file name should ends up with NULL\n");
		return -1;
	}else 
		return pathLen;
}


// function getPath assigns the path of a given file, *name to *path)
void getPath(char *path , int name, int nameLen){
	int check;
	int i;
	//printf("in getPath, the length is line 153. the path is:\n");
	for (i = 0; i < nameLen; i++) {
		if (machine->ReadMem(name+i, 1, &check))
			*(path + i) = check;
	//printf ("value is %c\n", check);
	}
	*(path + i ) = 0;
}

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


//Part 4 system calls for console read and write
/* Read "size" bytes from the open file into "buffer".
 * Return the number of bytes actually read -- if the open file isn't
 * long enough, or if it is an I/O device, and there aren't enough
 * characters to read, return whatever is available (for I/O devices,
 * you should always wait until you can return at least one character).
 */
/* 	
	Try to read size bytes into the user buffer.  
	Return the number of bytes actually read, which may 
	be less than the number of bytes requested, 
	e.g., if there are fewer than size bytes avail­able.
*/
int Read(char *buffer, int size, OpenFileId id) {
	//read from id and push it into buffer
 	char value;
 	char* data = &value;
	synchConsole->ReadConsole(data);
	//printf("*data = %c\n", *data);
	if (*data == '\n') {
		//printf("Found newline.\n");
        machine->WriteMem(machine->ReadRegister(4), 1, *data);
        machine->WriteMem((machine->ReadRegister(4))+2, 1, *data);
		//printf("%c\n", *buffer);
		//printf("%c\n", *(buffer+1));
		return sizeof(buffer);
 	}
    machine->WriteMem(machine->ReadRegister(4), 1, *data);
	//printf("%c", *buffer);
 	return sizeof(buffer);
 }


/* 	
	Write size bytes of data from the buffer 
	into the open descriptor named by id. 
*/
void Write(char *buffer, int size, OpenFileId id) {
    //read buffer and write to id
	synchConsole->WriteConsole(buffer);
}


