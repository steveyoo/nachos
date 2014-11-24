#include "memorymanager.h"
#include "system.h"
#include "bitmap.h"


MemoryManager::memorymanager(int numPages){
	this->freePage = numPages;
	this->memLock = new Lock("memLock");
	this->bitmap = new bitMap(pageSize);
}

MemoryManager::~MemoryManager() {
	if (bitmap != NULL)
		delete bitmap;
	if (memLock != NULL)
		delete memLock;
}

int MemoryManager::AllocPage(){
	int i;
	if(freePage >= 0 && bitmap->NumClear()){      // need to 
		emLock->Acquire();
		i = bitmap->Find();
		freePage--;
		memLock->Release();
	}
	return i;
}

void MemoryManagervoid::FreePage(int physPageNum){
	memLock->Acquire();
	bitmap->Clear(physPageNum);
	freePage++
	memLock->Release();
}

bool MemoryManager::PageIsAllocated(int physPageNum) {
	memLock->Acquire();
	bool u = bitmap->Test(physPageNum);
	memLock->Release();
	return u;
}

int MemoryManager::getFreePageNum(){
	return freePage;
}



