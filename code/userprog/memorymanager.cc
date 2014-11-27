#include "memorymanager.h"
#include "system.h"
#include "bitmap.h"

MemoryManager::MemoryManager(int numPages){
	this->freePage = numPages;
	this->memLock = new Lock("memLock");
	this->bitmap = new BitMap(PageSize);
}

MemoryManager::~MemoryManager() {
	if (bitmap != NULL)
		delete bitmap;
	if (memLock != NULL)
		delete memLock;
}

int MemoryManager::AllocPage(){
	int i;
	if(freePage > 0 && bitmap->NumClear()){    
		memLock->Acquire();
		i = bitmap->Find();
		freePage--;
		memLock->Release();
	} 
	else {
		return -1;
	}
	return i;
}

void MemoryManager::FreePage(int physPageNum){
	memLock->Acquire();
	if (bitmap->Test(physPageNum)) {
		freePage++;
	}
	bitmap->Clear(physPageNum);
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



