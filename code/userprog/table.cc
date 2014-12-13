#include "table.h"
#include "system.h"


/* Create a table to hold at most "size" entries. */
Table::Table(int size){
	tableSize = size;
	tableLock = new Lock("tableLock");
	table = (void**) new int[size];
	for(int i = 0; i < size;i++){
		table[i] = NULL;
	}
}

Table::~Table(){
	for(int i = 0; i < tableSize; i++) {
		delete (int*)table[i];
	}
	if (tableLock) 
		delete tableLock;
	if(table)
		delete table;
}

int Table::getTableSize(){
	return tableSize;
}

/* Allocate a table slot for "object", returning the "index" of the
allocated entry; otherwise, return -1 if no free slots are available. */
int Table::Alloc(void *object) {
	int id = 0;
	tableLock->Acquire();
	for(int i = 0; i < tableSize; i++) {
		if(table[i] == NULL){
			table[i] = object;
			id = (i + 1);
			break;
		}
	}
	tableSize--;
	tableLock->Release();
	return id;
}
/* Retrieve the object from table slot at "index", or NULL if that
slot has not been allocated. */
void* Table::Get(int index) {
	void* f;
	tableLock->Acquire();
	if(index > 0 && index < tableSize) 
		f = table[(index - 1)];
	else 
		f = NULL;
	tableLock->Release();
	return f;
}
/* Free the table slot at index. */
void Table::Release(int index) {
	tableLock->Acquire();
	if(index > 0 && index < tableSize) {
		table[(index - 1)] = NULL;
		tableSize++;
	}
	else  { // leave like this now, may need ASSERT(FALSE)
		printf("index bigger or less than the table");
		ASSERT(FALSE);
	}
	tableLock->Release();
}

void Table::ReleaseAll(){
	tableLock->Acquire();
	for(int i = 0; i <tableSize; i++)
		table[i] = NULL;
	tableLock->Release();
}

bool Table::AnyExist(){
	int anyExist = 0;
	tableLock->Acquire();
	for (int i = 0; i < tableSize; i++ ){
		if (table[i] != NULL)
			anyExist++;
	}
	ASSERT(anyExist != 0);
	tableLock->Release();
	return anyExist;
}






