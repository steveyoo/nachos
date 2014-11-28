
#ifndef TABLE_H
#define TABLE_H

class Thread;
class Lock;
#include "synch.h"

class Table{
public:
	/* Create a table to hold at most "size" entries. */
	Table(int size);
	~Table();
	/* Allocate a table slot for "object", returning the "index" of the
    allocated entry; otherwise, return -1 if no free slots are available. */
	int Alloc(void *object);
	/* Retrieve the object from table slot at "index", or NULL if that
   slot has not been allocated. */
	void *Get(int index);
	/* Free the table slot at index. */
	void Release(int index);

	int getTableSize();

	void ReleaseAll();

	bool AnyExist();
private:
	int tableSize;
	void **table;
	Lock* tableLock;

};
#endif