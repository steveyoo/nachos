// synchconsole.cc
// description

#include "copyright.h"
#include "synchconsole.h"

static void
ConsoleReadAvail (int arg)
{
    SynchConsole* console = (SynchConsole *)arg;

    console->ReadAvail();
}

static void
ConsoleWriteDone (int arg)
{
    SynchConsole* console = (SynchConsole *)arg;

    console->WriteDone();
}

//----------------------------------------------------------------------
// SynchConsole::SynchConsole
// 	Initialize the synchronous interface to the physical console, in turn
//	initializing the physical console.
//  Put NULL as both parameters so that it reads and writes to stin and stout
//  when making a new "synchconsole" in StartProcess
//----------------------------------------------------------------------

SynchConsole::SynchConsole(char *readFile, char *writeFile) {
    lock = new Lock("synch console lock");
    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);
    console = new Console(readFile, writeFile, ConsoleReadAvail, ConsoleWriteDone, (int) this);
}

//----------------------------------------------------------------------
// SynchConsole::~SynchConsole
// 	De-allocate data structures needed for the synchronous console
//	abstraction.
//----------------------------------------------------------------------

SynchConsole::~SynchConsole() {
    delete console;
    delete lock;
    delete readAvail;
    delete writeDone;
}


//----------------------------------------------------------------------
// SynchDisk::ReadSector
// 	Read the contents of a disk sector into a buffer.  Return only
//	after the data has been read.
//
//	"sectorNumber" -- the disk sector to read
//	"data" -- the buffer to hold the contents of the disk sector
//----------------------------------------------------------------------

void
SynchConsole::ReadConsole(char* data) {
    lock->Acquire();

    *data = console->GetChar();
    readAvail->P();

    lock->Release();
}

//----------------------------------------------------------------------
// SynchDisk::WriteSector
// 	Write the contents of a buffer into a disk sector.  Return only
//	after the data has been written.
//
//----------------------------------------------------------------------

void
SynchConsole::WriteConsole(char* data) {
    lock->Acquire();

    console->PutChar(*data);
    writeDone->P();

    lock->Release();
}

//----------------------------------------------------------------------
// SynchDisk::RequestDone
// 	Disk interrupt handler.  Wake up any thread waiting for the disk
//	request to finish.
//----------------------------------------------------------------------
void 
SynchConsole::ReadAvail() {
    readAvail->V();
}
void 
SynchConsole::WriteDone() {
    writeDone->V();
}