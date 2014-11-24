#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H
#include "bitmap.h"
#include "system.h"
#include "thread.h"


class Lock;
class Thread;  // may need it later for mult-thread
#include "synch.h"
class MemoryManager {
public:
	MemoryManager(int numPages);
	~MemoryManager();
	/* Allocate a free page, returning its physical page number or -1
   if there are no free pages available. */
	int AllocPage();
	/* Free the physical page and make it available for future allocation. */
	void FreePage(int physPageNum);
	/* True if the physical page is allocated, false otherwise. */
	bool PageIsAllocated(int physPageNum); 
	/*return how many bits are free*/
	int getFreePageNum();
private:
	int freePage;
	BitMap *bitmap;
	Lock *memLock;
};

#endif

