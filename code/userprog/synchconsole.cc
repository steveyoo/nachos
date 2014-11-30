// synchconsole.cc
// description

#include "copyright.h"
#include "synchconsole.h"

static void
ConsoleReadAvail (int arg) {
    SynchConsole* console = (SynchConsole *)arg;

    console->ReadAvail();
}

static void
ConsoleWriteDone (int arg) {
    SynchConsole* console = (SynchConsole *)arg;

    console->WriteDone();
}


SynchConsole::SynchConsole(char *readFile, char *writeFile) {
    lock = new Lock("synch console lock");
    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);
    console = new Console(readFile, writeFile, ConsoleReadAvail, ConsoleWriteDone, (int) this);
}
SynchConsole::~SynchConsole() {
    delete console;
    delete lock;
    delete readAvail;
    delete writeDone;
}


void
SynchConsole::ReadConsole(char* data) {
    lock->Acquire();
    readAvail->P();
    *data = console->GetChar();
    lock->Release();
}
void
SynchConsole::WriteConsole(char* data) {
    lock->Acquire();
    console->PutChar(*data);
    writeDone->P();
    lock->Release();
}


void 
SynchConsole::ReadAvail() {
    readAvail->V();
}
void 
SynchConsole::WriteDone() {
    writeDone->V();
}