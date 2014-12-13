// addrspace.h
//	Data structures to keep track of executing user programs
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H
#include "noff.h"
#include "copyright.h"
#include "filesys.h"
#include "memorymanager.h"


#define UserStackSize 1024 	// increase this as necessary!

class BackingStore;
class AddrSpace {
public:
    AddrSpace();	// Create an address space,
    ~AddrSpace();	              // De-allocate an address space
    int Initialize(OpenFile *executable);     
    // initializing it with the program
    // stored in the file "executable"

    void InitRegisters();		// Initialize user-level CPU registers,
    // before jumping to user code
    int getNumPages(){ return numPages;}


    void SaveState();			// Save/restore address space-specific
    void RestoreState();	// info on a context switch

    bool HandlePageFault(OpenFile *executable, int bvpn);

    int Translate(int addr);

    void ReadIntoMem(OpenFile *exec, int addr, int size, int infile);

    void setBit(int vpn);

    void Evict();

    int getPteIndex(TranslationEntry *);
    // make header file global.
    NoffHeader noffH;
    OpenFile *executableFile;
    
 

private:
    	// Assume linear page table translation
    // for now!
    TranslationEntry *pageTable;
    unsigned int numPages;		// Number of pages in the virtual
    // address space
    BackingStore *backingStore;
};


class BackingStore {
public:
    /* Create a backing store file for an AddrSpace */
    BackingStore(AddrSpace *as);
    ~BackingStore();
/* Write the virtual page referenced   by pte to the backing st
ore */
/* Example invocation: PageOut(&machine->pageTable[virtualPage]) or */
/*                     PageOut(&space->pageTable[virtualPage]) */
    void PageOut(TranslationEntry *pte);

/* Read the virtual page referenced by pte from the backing store */
    void PageIn(TranslationEntry *pte);

    
private:
       FileSystem* BSFile;
       OpenFile *swap;
       char fileName[16];
       AddrSpace* space;
};




#endif // ADDRSPACE_H
