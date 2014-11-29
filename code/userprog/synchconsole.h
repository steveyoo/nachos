// synchconsole.h
// description

#include "copyright.h"

#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "console.h"
#include "synch.h"

// The following class defines a "synchronous" disk abstraction.
// As with other I/O devices, the raw physical disk is an asynchronous device --
// requests to read or write portions of the disk return immediately,
// and an interrupt occurs later to signal that the operation completed.
// (Also, the physical characteristics of the disk device assume that
// only one operation can be requested at a time).
//
// This class provides the abstraction that for any individual thread
// making a request, it waits around until the operation finishes before
// returning.
class SynchConsole {
public:
    SynchConsole(char* readFile, char *writeFile);
    ~SynchConsole();

    void ReadConsole(char* data);
    void WriteConsole(char* data);
    void ReadAvail();
    void WriteDone();

private:
    Console *console;            
    Semaphore *writeDone;
    Semaphore *readAvail;
    Lock *lock;
};

#endif // SYNCHCONSOLE_H
