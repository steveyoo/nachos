// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

// void
// ExceptionHandler(ExceptionType which)
// {
//     int type = machine->ReadRegister(2);

//     if ((which == SyscallException) && (type == SC_Halt)) {
//         DEBUG('a', "Shutdown, initiated by user program.\n");
//         interrupt->Halt();
//     } else {
//         printf("Unexpected user mode exception %d %d\n", which, type);
//         ASSERT(FALSE);
//     }
// }

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if ( which == SyscallException ) {
    	switch (type) {
    		case SC_Halt:
    			DEBUG('a', "Shutdown, initiated by user program.\n");
        		interrupt->Halt();
        		break;
        	case SC_Exit:
                // TODO: should be moved to sysexit.cc like Exec 
        		userExit();
        		break;	
        	case SC_Exec:
        		userExec();
        		break;
        	case SC_Join:
        		printf("Join is the extra credit part\n");
                ASSERT(FALSE);
        		// to handle Join
        		break;
        	case SC_Create:
        		printf("SC_Create is not required\n");
                ASSERT(FALSE);
        		break;
	     	case SC_Open:
	     		printf(" SC_Open not required\n");
                ASSERT(FALSE);
	     		break;

			case SC_Read:
                {
                    // passing in &buffer[i] instead of just buffer
                    // i isn't being updated for some reason, the buffer isn't moving addresses.
                    // do i need to writemem?
                    int buffer;
                    machine->ReadMem(machine->ReadRegister(4), 1, &buffer);
                    // printf("%u\n", machine->ReadRegister(4));
                    // printf("%u\n", buffer);
                    // printf("%u\n", buffer);
                    int size = machine->ReadRegister(5);
                    OpenFileId id = machine->ReadRegister(6);

                    //buffer address == 0 is often a valid address in Nachos (why?). 
                    //Size == 0 is also a valid argument.

                    //Try to read into a memory that does not have a valid 
                    //physical page mapped or not writable (when size > 0).
                    if(size > 0 && (unsigned int)&buffer < 0){
                        printf("Invalid address.\n");
                        machine->WriteRegister(2, -1);
                        machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                        machine->WriteRegister(PCReg, machine->ReadRegister(PCReg) + 4);
                        machine->WriteRegister(NextPCReg, machine->ReadRegister(PCReg) + 8); 
                        break;
                    }
                    //Try to read on file id other than serial console input.
                    if(id != ConsoleInput) {
                        printf("Not ConsoleInput.\n");
                        machine->WriteRegister(2, -1);
                        machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                        machine->WriteRegister(PCReg, machine->ReadRegister(PCReg) + 4);
                        machine->WriteRegister(NextPCReg, machine->ReadRegister(PCReg) + 8); 
                        break;
                    }
                    //Try to read with a negative size.
                    if(size < 0) {
                        printf("Argument size cannot be < 0.\n");
                        machine->WriteRegister(2, -1);
                        machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                        machine->WriteRegister(PCReg, machine->ReadRegister(PCReg) + 4);
                        machine->WriteRegister(NextPCReg, machine->ReadRegister(PCReg) + 8); 
                        break;
                    }
                    machine->WriteRegister(2, Read((char*)&buffer, size, id));
                    //machine->ReadMem(machine->ReadRegister(4), 1, &buffer);
                    //printf("%d - %c\n", machine->ReadRegister(4), buffer);
                    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                    machine->WriteRegister(PCReg, machine->ReadRegister(PCReg) + 4);
                    machine->WriteRegister(NextPCReg, machine->ReadRegister(PCReg) + 8); 
                }
				break;

			case SC_Write:
                {
                    int buffer;
                    machine->ReadMem(machine->ReadRegister(4), 1, &buffer);  
                    //printf("%d\n", machine->ReadRegister(4));
                    int size = machine->ReadRegister(5);
                    OpenFileId id = machine->ReadRegister(6);

                    if(size > 0 && (unsigned int)&buffer < 0){
                        printf("Invalid address.\n");
                        machine->WriteRegister(2, -1);
                        machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                        machine->WriteRegister(PCReg, machine->ReadRegister(PCReg) + 4);
                        machine->WriteRegister(NextPCReg, machine->ReadRegister(PCReg) + 8); 
                        break;
                    }
                    if(id != ConsoleOutput) {
                        printf("Not ConsoleOutput.\n");
                        machine->WriteRegister(2, -1);
                        machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                        machine->WriteRegister(PCReg, machine->ReadRegister(PCReg) + 4);
                        machine->WriteRegister(NextPCReg, machine->ReadRegister(PCReg) + 8); 
                        break;
                    }
                    if(size < 0) {
                        printf("Argument size cannot be < 0.\n");
                        machine->WriteRegister(2, -1);
                        machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                        machine->WriteRegister(PCReg, machine->ReadRegister(PCReg) + 4);
                        machine->WriteRegister(NextPCReg, machine->ReadRegister(PCReg) + 8); 
                        break;
                    }
                    for (int i = 0; i < size; i++) {
                        machine->ReadMem(machine->ReadRegister(4) + i, 1, &buffer);  
                        Write((char*)&buffer, size, id);
                    }
                    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                    machine->WriteRegister(PCReg, machine->ReadRegister(PCReg) + 4);
                    machine->WriteRegister(NextPCReg, machine->ReadRegister(PCReg) + 8); 
                }
				break;

			case SC_Close:
				printf("SC_Close is not required\n");
                ASSERT(FALSE);
				break;
			case SC_Fork:
				printf("SC_Forkis not required\n");
                ASSERT(FALSE);
				break;
				printf("Fork is not required\n");
                ASSERT(FALSE);
			case SC_Yield:
                    printf("SC_Yield is required\n");
                    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                    machine->WriteRegister(PCReg, machine->ReadRegister(PCReg) + 4);
                    machine->WriteRegister(NextPCReg, machine->ReadRegister(PCReg) + 8); 
				break;
            default :
                 printf("Unexpected user mode exception %d %d\n", which, type);
                ASSERT(FALSE);
    	}	// end of switch casle	
    }	// end of if statement
        
    else if (which == NumExceptionTypes) {
        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(false);
    }
    else if (which == IllegalInstrException) {
        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(false);
    } 
    else if (which == OverflowException) {
        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(false);
    }
    else if (which == AddressErrorException) {

        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(false);
    } else {
        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(FALSE);
    }

}