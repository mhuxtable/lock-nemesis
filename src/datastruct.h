/*
 * Generic interface for data storage.
 */

#ifndef LN_DATASTRUCT_H
#define LN_DATASTRUCT_H

/* exported commands which any implemented data structure MUST support 
   are as follows.
   
   It is assumed the proper locks are held on these operations BEFORE
   the operation is invoked (i.e. allowing the data structure to be tested
   with multiple locking schemes). */

typedef struct ln_entry
{
	u64 key;
	u64 val;
}

/* Setup. Called from a singly-threaded setup worker. Need not be thread-safe.
   The void * pointer is for future use to pass in any desired arguments.    */
int  ln_datastruct_setup(void *);

/* Store. MAY be invoked concurrently, so an appropriate lock must be held.
   ln_datastruct_get_lock(1) may be invoked to determine which is the identifier
   of the correct lock to take for this particular type of data structure
   and for the requested operation (of course, a more coarse-grained locking
   strategy may ignore the result of get_lock and do its own thing. */
int  ln_datastruct_store(ln_hash_entry_t *);

/* Search. MAY be invoked concurrently, so an appropriate lock must be held.
   ln_datastruct_get_lock(1) may be invoked to determine which is the identifier
   of the correct lock to take for this particular type of data structure
   and for the requested operation (of course, a more coarse-grained locking
   strategy may ignore the result of get_lock and do its own thing. */
ln_entry_t *ln_datastruct_search(char *key);

/* Teardown. Called from a singly-threaded teardown context after all tests
   are completed. Need not be thread-safe. Use this for unsetting any state
   which should be avoided, freeing memory and preparing for the next test
   as necessary. */
   


#endif // LN_DATASTRUCT_H
