

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
   
    if (numPages > NumPhysPages ||  numPages > (unsigned int)memoryManager->getFreePageNum()) {
        printf(" file is too big to be load\n");
        //return -1;
    }
   // create a pageTable (amount of page entires) for the current process 

    DEBUG('4',"Initializing address space, num pages %d, size %d\n", numPages, size);
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
    // counters for putting code in to physcial menmory
//     unsigned int codeSize = noffH.code.size;
//     unsigned int codeVirtualAddr = noffH.code.virtualAddr;
//     unsigned int codeInFileAddrt = noffH.code.inFileAddr;
//     unsigned int offset = codeVirtualAddr & (PageSize - 1); //bitwise multiplicationt
//     unsigned int physicalPageNum;

// // then, copy code into memory
//     while(codeSize >= (PageSize - offset)) {
//         physicalPageNum = pageTable[codeVirtualAddr/PageSize].physicalPage;
//         // printf("code.........................................: \n");
//         // printf("code size: %u\n", codeSize);
//         // printf("virtual addr: %u\n", codeVirtualAddr);
//         // printf("in file addr: %u\n", codeInFileAddrt);
//         // printf("offset: %u\n", offset);
//         // printf("page number: %u\n", physicalPageNum);
//         // printf("page size: %u\n", PageSize);
//         executable->ReadAt(&(machine->mainMemory[physicalPageNum*PageSize + offset]),
//                            PageSize - offset, codeInFileAddrt);
//         // update conuters      
//         codeSize = codeSize - (PageSize - offset);
//         codeVirtualAddr = codeVirtualAddr + (PageSize - offset);
//         codeInFileAddrt = codeInFileAddrt + (PageSize - offset);
//         offset = codeVirtualAddr & (PageSize - 1);
//     }
//     // corner case, last part of the code segment where 
//     // code size may be less then the pageSize - offset
//     if(codeSize > 0) {
//         physicalPageNum = pageTable[codeVirtualAddr/PageSize].physicalPage;
//         executable->ReadAt(&(machine->mainMemory[physicalPageNum*PageSize + offset]),
//                            codeSize, codeInFileAddrt);
//         // printf("code.........................................: \n");
//         // printf("code size: %u\n", codeSize);
//         // printf("virtual addr: %u\n", codeVirtualAddr);
//         // printf("in file addr: %u\n", codeInFileAddrt);
//         // printf("offset: %u\n", offset);
//         // printf("physical page number: %u\n", physicalPageNum);
//         // printf("page size: %u\n", PageSize);
//     }
//    // printf(">>>>>>>>>>>>>>>>>>>>>end of code <<<<<<<<<<<<<<<<<<<\n");

//     // copy data segments into memory
//     codeSize = noffH.initData.size;
//     codeVirtualAddr = noffH.initData.virtualAddr;
//     codeInFileAddrt = noffH.initData.inFileAddr;
//     offset = codeVirtualAddr & (PageSize - 1); 
    
//     while(codeSize >= (PageSize - offset)) {
//         physicalPageNum = pageTable[codeVirtualAddr/PageSize].physicalPage;
//         // printf("data.........................................: \n");
//         // printf("data size: %u\n", codeSize);
//         // printf("virtual addr: %u\n", codeVirtualAddr);
//         // printf("in file addr: %u\n", codeInFileAddrt);
//         // printf("offset: %u\n", offset);
//         // printf("physical page number: %u\n", physicalPageNum);
//         // printf("page size: %u\n", PageSize);
//         executable->ReadAt(&(machine->mainMemory[physicalPageNum*PageSize + offset]),
//                            PageSize - offset, codeInFileAddrt);
//         // update conuters      
//         codeSize = codeSize - (PageSize - offset);
//         codeVirtualAddr = codeVirtualAddr + (PageSize - offset);
//         codeInFileAddrt = codeInFileAddrt + (PageSize - offset);
//         offset = codeVirtualAddr & (PageSize - 1); 
//     }
//     // corner case, last part of the initcode segment where 
//     // code size may be less then the pageSize - offset
//     if(codeSize > 0) {
//         physicalPageNum = pageTable[codeVirtualAddr/PageSize].physicalPage;
//         executable->ReadAt(&(machine->mainMemory[physicalPageNum*PageSize + offset]),
//                            codeSize, codeInFileAddrt);
//         // printf("data.........................................: \n");
//         // printf("data size: %u\n", codeSize);
//         // printf("virtual addr: %u\n", codeVirtualAddr);
//         // printf("in file addr: %u\n", codeInFileAddrt);
//         // printf("offset: %u\n", offset);
//         // printf("physical page number: %u\n", physicalPageNum);
//         // printf("page size: %u\n", PageSize);
//     }

    //printf(">>>>>>>>>>>>>>>>>>>>>end of data <<<<<<<<<<<<<<<<<<<\n");

//}
//////////////////////////////////////////////////////////////////////////
bool
AddrSpace::HandlePageFault(OpenFile *executable, int badvpn)
{   
    // backingStore = new BackingStore(currentThread->space);
    int codePageNum, codeVirtAddr, codeFileOffset, codeSize;
    int dataPageNum, dataVirtAddr, dataFileOffset,  dataSize;

    int n;
    n = memoryManager->AllocPage();
    if(n==-1)
        return false;
    else{//allocate pyscial addr
        pageTable[badvpn].physicalPage = n;
    
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
    else 
        backingStore->PageIn(&pageTable[badvpn]);   
    }
     return true; 
}
  
/////////////////////////////////////////////////////////////////////////////////

void AddrSpace::setBit(int vpn){
    pageTable[vpn].valid = TRUE;
}


void
AddrSpace::Evict(){
//printf("Evict has been called\n"); 
int i=0;
while(!pageTable[i].valid)
{
  i++;
}
    pageTable[i].valid = FALSE;
    TranslationEntry *pte;
    pte = &pageTable[i];
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
int AddrSpace::Translate(int addr){
    int phypage = addr / PageSize;
    int offset = addr % PageSize;
    int phyaddr = pageTable[phypage].physicalPage * PageSize + offset;
    return phyaddr;
}


////////////////////////////////////////////////////////////////////////////
void AddrSpace::ReadIntoMem(OpenFile *exec, int addr, int size, int infile){
    unsigned int phyaddr;
    while (size>PageSize) {
        Translate(addr);
       // DEBUG('c',"Physical addr = 0x%x\n",phyaddr);
        exec->ReadAt(&(machine->mainMemory[phyaddr]),PageSize, infile);
        size-=PageSize;        
        addr+=PageSize;
        infile+=PageSize;
        }
        if(size) {
            Translate(addr);
           // DEBUG('c',"Physical addr = 0x%x\n",phyaddr);
            exec->ReadAt(&(machine->mainMemory[phyaddr]), size, infile);
        }              
}

int AddrSpace::getPteIndex(TranslationEntry *pte){
   // printf("numPages is :  %d\n", numPages);
    for (unsigned int i = 0;i < numPages; i++) {
        if(pte == &pageTable[i])
            return i;
    }
    return -1;
}

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




/////////////////////////////////////////////////////////////////////////////////
//                              BackingStore class  implementatoin ///////////////

BackingStore::BackingStore(AddrSpace *as){   
    int pn;
    this->space = as;
    int pid = currentThread->spaceID;
    BSFile = new FileSystem(true);
    pn = space->getNumPages();
    sprintf (fileName, "%d", pid);
    BSFile->Create(fileName,pn*PageSize);
    swap = BSFile->Open(fileName);
}
BackingStore::~BackingStore(){
    delete BSFile;
    delete space;
    delete swap;
}


void
BackingStore::PageOut(TranslationEntry *pte){
   // printf("pageout has been called\n"); 
    stats->incrNumPageOuts(); 
    int pagenum;
    int phys_addr;
    
    pagenum = space->getPteIndex(pte);
   // printf("pageout has been called %d\n", stats->numPageOuts); 
    phys_addr = space->Translate(pagenum*PageSize);
    swap->WriteAt(&machine->mainMemory[phys_addr], PageSize, pagenum*PageSize);
}


void
BackingStore::PageIn(TranslationEntry *pte){
    // printf("pagein has been called\n"); 
     stats->incrNumPageIns();
     int pagenum;
     int phys_addr;
     pagenum = space->getPteIndex(pte);
     phys_addr = space->Translate(pagenum*PageSize);
     swap->ReadAt(&machine->mainMemory[phys_addr], PageSize, pagenum*PageSize);
}













