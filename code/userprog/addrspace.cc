

// addrspace.cc
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.
#include "addrspace.h"
#include "copyright.h"
#include "system.h"
#ifdef HOST_SPARC
#include <strings.h>
#endif

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------
//extern MemoryManager *memoryManager;

static void
SwapHeader (NoffHeader *noffH)
{
    noffH->noffMagic = WordToHost(noffH->noffMagic);
    noffH->code.size = WordToHost(noffH->code.size);
    noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
    noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
    noffH->initData.size = WordToHost(noffH->initData.size);
    noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
    noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
    noffH->uninitData.size = WordToHost(noffH->uninitData.size);
    noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
    noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace()
{  

    //dummy constructor, its job is replace by Initialize
}
int AddrSpace::Initialize(OpenFile *executable){

    backingStore = new BackingStore(this);
    RandomInit(888);

    // NoffHeader noffH;
    unsigned int i, size;
    this->executableFile = executable;

    executableFile->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) &&
            (WordToHost(noffH.noffMagic) == NOFFMAGIC))
        SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size
           + UserStackSize;	// we need to increase the size
    // to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;
   
    // if (numPages > NumPhysPages) {
    //     printf("File is too big to be loaded.\n");
    //     //return -1;
    // }
   // create a pageTable (amount of page entires) for the current process 

    DEBUG('4',"Initializing address space, Program size %d, Virtual pages %d, Physical pages %d\n", size, numPages, NumPhysPages);
    //ASSERT(numPages <= NumPhysPages)		// check we're not trying
    // to run anything too big --
    // at least until we have
    // virtual memory
    // first, set up the translation
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
        pageTable[i].virtualPage = i;
        //pageTable[i].physicalPage = 0;// assign a free physical page
        // zero out the entire address space, to zero the unitialized data segment
        // and the stack segment  
        // zero the allocted page
         //  ASSERT(pageTable[i].physicalPage >= 0);
        //  bzero(&machine->mainMemory[pageTable[i].physicalPage * PageSize], PageSize);
        pageTable[i].valid = FALSE;
        pageTable[i].use = FALSE;
        pageTable[i].dirty = FALSE;
        pageTable[i].readOnly = FALSE;  // if the code segment was entirely on
                                        // a separate page, we could set its
                                         // pages to be read-only
        pageTable[i].isStore = FALSE;
    }   

    return 1;
}

bool
AddrSpace::HandlePageFault(OpenFile *executable, int badvpn)
{   
    int codePageNum, codeVirtAddr, codeFileOffset, codeSize;
    int dataPageNum, dataVirtAddr, dataFileOffset, dataSize;

    int n = memoryManager->AllocPage();
    if(n == -1) {
        printf("Couldn't find free physical page. Going to evict.\n");
        return false;
    }
    else {
        //allocate physical address
        pageTable[badvpn].physicalPage = n;
        printf("Faulted virtual page %d now points to physical page %d\n",badvpn,n);
    }
    
   if(!pageTable[badvpn].isStore){
        codePageNum = (noffH.code.virtualAddr + noffH.code.size)/PageSize;
        dataPageNum = (noffH.initData.virtualAddr + noffH.initData.size)/PageSize;
        // load code
        if(codePageNum >= badvpn && noffH.code.size > 0){   
            if (noffH.code.virtualAddr/PageSize == badvpn){
                codeVirtAddr = noffH.code.virtualAddr;                    
                codeSize = PageSize - noffH.code.virtualAddr % PageSize;
                codeFileOffset = noffH.code.inFileAddr;    
                executable->ReadAt(&(machine->mainMemory[Translate(codeVirtAddr)]), codeSize, codeFileOffset);
            }  
            else if (codePageNum > badvpn){
                codeFileOffset = noffH.code.inFileAddr + PageSize * badvpn - noffH.code.virtualAddr;
                codeVirtAddr = PageSize * badvpn ;
                codeSize = PageSize;
                executable->ReadAt(&(machine->mainMemory[Translate(codeVirtAddr)]), codeSize, codeFileOffset);
            }
           else if (codePageNum == badvpn){
                codeFileOffset = noffH.code.inFileAddr + PageSize * badvpn - noffH.code.virtualAddr;
                codeVirtAddr = PageSize * badvpn ;
                codeSize = (noffH.code.virtualAddr + noffH.code.size) % PageSize;
                executable->ReadAt(&(machine->mainMemory[Translate(codeVirtAddr)]), codeSize, codeFileOffset);
                bzero(&(machine->mainMemory[pageTable[badvpn].physicalPage*PageSize + codeSize]), sizeof(PageSize-codeSize));
            }
        }
        // load data
        if((dataPageNum >= badvpn) && (badvpn >= codePageNum) && (noffH.initData.size > 0) ){

            if (noffH.initData.virtualAddr/PageSize == badvpn){
                dataVirtAddr = noffH.initData.virtualAddr;                    
                dataSize = PageSize - noffH.initData.virtualAddr % PageSize;
                dataFileOffset = noffH.initData.inFileAddr;    
                executable->ReadAt(&(machine->mainMemory[Translate(dataVirtAddr)]), dataSize, dataFileOffset);
            }
                
            else if (dataPageNum > badvpn){
                dataFileOffset = noffH.initData.inFileAddr + PageSize * badvpn - noffH.initData.virtualAddr;
                dataVirtAddr = PageSize * badvpn ;
                dataSize = PageSize;
                executable->ReadAt(&(machine->mainMemory[Translate(dataVirtAddr)]), dataSize, dataFileOffset);
            }
           else if (dataPageNum == badvpn){
                dataFileOffset = noffH.initData.inFileAddr + PageSize * badvpn - noffH.initData.virtualAddr;
                dataVirtAddr = PageSize * badvpn ;
                dataSize = (noffH.initData.virtualAddr + noffH.initData.size) % PageSize;
                executable->ReadAt(&(machine->mainMemory[Translate(dataVirtAddr)]), dataSize, dataFileOffset);
                bzero(&(machine->mainMemory[pageTable[badvpn].physicalPage*PageSize + dataSize]), sizeof(PageSize-dataSize));
            }
        }
        if (dataPageNum < badvpn)
            bzero(&(machine->mainMemory[pageTable[badvpn].physicalPage*PageSize]), sizeof(PageSize));
            
    }
    else {
        backingStore->PageIn(&pageTable[badvpn]);
    }
    return true; 
}
  
/////////////////////////////////////////////////////////////////////////////////

void
AddrSpace::setBit(int vpn){
    pageTable[vpn].valid = TRUE;
}


void
AddrSpace::Evict(){
//printf("Evict has been called\n"); 
int i=0;
while(!pageTable[i].valid) {
    //evictPhysPage = Rand();
    i++;
}
//printf("%d",pageTable.size());
pageTable[i].valid = FALSE;
TranslationEntry *pte = &pageTable[i];
if(pageTable[i].dirty)
{
// printf("PTE to Evict is :  %d\n",  pte);
    backingStore->PageOut(pte);
    pageTable[i].dirty = FALSE; 
    pageTable[i].isStore = TRUE;   
}
memoryManager->FreePage(pageTable[i].physicalPage);
}



////////////////////////////////////////////////////////////////////////////
int
AddrSpace::Translate(int addr){
    int phypage = addr / PageSize;
    int offset = addr % PageSize;
    int phyaddr = pageTable[phypage].physicalPage * PageSize + offset;
    return phyaddr;
}

int
AddrSpace::getPteIndex(TranslationEntry *pte) {
   // printf("numPages is :  %d\n", numPages);
    for (unsigned int i = 0;i < numPages; i++) {
        if(pte == &pageTable[i])
            return i;
    }
    return -1;
}

////////////////////////////////////////////////////////////////////////////
// void AddrSpace::ReadIntoMem(OpenFile *exec, int addr, int size, int infile){
//     unsigned int phyaddr;
//     while (size>PageSize) {
//         Translate(addr);
//        // DEBUG('c',"Physical addr = 0x%x\n",phyaddr);
//         exec->ReadAt(&(machine->mainMemory[phyaddr]),PageSize, infile);
//         size-=PageSize;        
//         addr+=PageSize;
//         infile+=PageSize;
//         }
//         if(size) {
//             Translate(addr);
//            // DEBUG('c',"Physical addr = 0x%x\n",phyaddr);
//             exec->ReadAt(&(machine->mainMemory[phyaddr]), size, infile);
//         }              
// }

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{   
    // fileSystem->Remove(swap_name);
    for (unsigned int i = 0; i < numPages; i ++) 
        memoryManager->FreePage(pageTable[i].physicalPage);

    if(pageTable != NULL) 
        delete [] pageTable;

    delete executableFile;
    delete backingStore;
}   

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
        machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

    // Set the stack register to the end of the address space, where we
    // allocated the stack; but subtract off a bit, to make sure we don't
    // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState()
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState()
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}


// BackingStore class implementation //

BackingStore::BackingStore(AddrSpace *as) {
    int pn;
    this->space = as;
    int pid = currentThread->spaceID;
    BSFile = new FileSystem(true);
    pn = space->getNumPages();
    sprintf (fileName, "%d", pid);
    BSFile->Create(fileName,pn*PageSize);
    swap = BSFile->Open(fileName);
}


void
BackingStore::PageOut(TranslationEntry *pte){
    printf("PageOut has been called\n"); 
    stats->incrNumPageOuts(); 
    int pageNum;
    int physAddr;
    pageNum = space->getPteIndex(pte);
    //printf("pageout has been called %d\n", stats->numPageOuts); 
    physAddr = space->Translate(pageNum*PageSize);
    swap->WriteAt(&machine->mainMemory[physAddr], PageSize, pageNum*PageSize);
}


void
BackingStore::PageIn(TranslationEntry *pte){
    printf("PageIn has been called\n");
    stats->incrNumPageIns();
    int pageNum;
    int physAddr;
    pageNum = space->getPteIndex(pte);
    physAddr = space->Translate(pageNum*PageSize);
    swap->ReadAt(&machine->mainMemory[physAddr], PageSize, pageNum*PageSize);
}
