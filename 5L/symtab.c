//
// This is the interface for a generic symbol table. A table stores
// (symbol, data) pairs.
//
// A symbol is simply a C string (null-terminated char sequence).
//
// The data associated with a symbol is simply a void*.
//
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

typedef struct  {
    char* symbol;
    void* data;
    struct entry* next;

} entry;


typedef struct tableControl {
    int length;
    void** address;
} tableControl;


typedef struct {
    void* table;
    unsigned int nextLevel;
    void* nextEntry;

} iterator;



static unsigned int hash(const char *str)
{
    const unsigned int p = 16777619;
    unsigned int hash = 2166136261u;
    while (*str)
    {
        hash = (hash ^ *str) * p;
        str += 1;
    }
    hash += hash << 13;
    hash ^= hash >> 7;
    hash += hash << 3;
    hash ^= hash >> 17;
    hash += hash << 5;
    return hash;
}

static entry *lookup(tableControl *symtabHandle, const char *symbol)
{
    
    unsigned int hashSymbol = hash(symbol);
    unsigned int tot = hashSymbol % symtabHandle->length;
    entry *node = (entry *)symtabHandle->address[tot];
    if(node == NULL){
        return NULL;
    }

    while(node!=NULL){
        
    if (strcmp(node->symbol, symbol) == 0)
    {
        return node;
    }
    else
    {
        node = (entry*)node->next;
    }
}
return NULL;
}


void *symtabCreate(int sizeHint){
    tableControl *h = malloc(sizeof(tableControl));
    if (h == NULL){
        return NULL;
    }

    h->length = sizeHint;
    h->address = malloc(sizeof(h)*sizeHint);
    if (h->address == NULL){
        free(h);
        return NULL;
    }
    for (int i = 0; i < h->length; i++){
        h->address[i] = NULL;
    }
    return h;

// Creates a symbol table.
// If successful, returns a handle for the new table.
// If memory cannot be allocated for the table, returns NULL.
// The parameter is a hint as to the expected number of (symbol, data)
//   pairs to be stored in the table.
return 0;

}
void symtabDelete(void *symtabHandle){


//     Implement symtabDelete. This requires freeing the strings for all symbols in the table, freeing all
// linked structs in the table, freeing the array of pointers to linked structs, and freeing the control
// struct. Note: do not call free for the data member of the linked structs.
tableControl *tab = symtabHandle;
for (int i = 0; i < tab->length; i++){
    free(tab->address[i]);
}
free(tab);
//free(symtabHandle);

// Deletes a symbol table.
// Reclaims all memory used by the table.
// Note that the memory associate with data items is not reclaimed since
//   the symbol table does not know the actual type of the data. It only
//   manipulates pointers to the data.
// Also note that no validation is made of the symbol table handle passed
//   in. If not a valid handle, then the behavior is undefined (but
//   probably bad).

}
int symtabInstall(void *symtabHandle, const char *symbol, void *data)
{
    tableControl *h = symtabHandle;
    entry *check = lookup(h, symbol);
    if (check != NULL)
    {
        check->data = data;
    }
    else
    {
        entry *install = malloc(sizeof(entry));
        if (install == NULL)
        {
            return 0;
        }
        install->symbol = malloc(sizeof(char) * ((strlen(symbol) + 1)));
        if (install->symbol == NULL)
        {
            return 0;
        }
        strcpy(install->symbol, symbol);
        install->data = data;
        unsigned int hashSymbol = hash(symbol);
        hashSymbol = hashSymbol % h->length;
        if (h->address[hashSymbol] != NULL)
        {
            install->next = h->address[hashSymbol];
            h->address[hashSymbol] = install;
        }
        else
        {
            h->address[hashSymbol] = install;
            
            
        }
        return 1;
    }
    // Install a (symbol, data) pair in the table.
    // If the symbol is already installed in the table, then the data is
    //   overwritten.
    // If the symbol is not already installed, then space is allocated and
    //   a copy is made of the symbol, and the (symbol, data) pair is then
    //   installed in the table.
    // If successful, returns 1.
    // If memory cannot be allocated for a new symbol, then returns 0.
    // Note that no validation is made of the symbol table handle passed
    //   in. If not a valid handle, then the behavior is undefined (but
    //   probably bad).
    return 1;
}
void *symtabLookup(void *symtabHandle, const char *symbol){

    entry* look = lookup(symtabHandle, symbol);

    if (look == NULL){
        return NULL;
    }
    return look->data;
    

return 0;
}
void *symtabCreateIterator(void *symtabHandle){

    tableControl *h = symtabHandle;
    iterator *i = malloc(sizeof(iterator)); 
    if (i == NULL){
        return NULL;
    }
    i->table = h;
    int j = 0;
    while(h->address[j] == NULL){
        j++;
    }
    i->nextLevel = j;
    i->nextEntry = h->address[j];
    return i;

return 0;

}
const char *symtabNext(void *iteratorHandle, void **returnData){
    iterator *i = iteratorHandle;
    if (i->nextEntry == NULL){
        return NULL;
    }
tableControl *tab = i->table;
entry *e = i->nextEntry;
*returnData = e->data;
if (e->next != NULL){
    i->nextEntry = e->next;
    return e->symbol;
} else {
    for (int j = i->nextLevel+1; j < tab->length; j++){
        if (tab->address[j] != NULL){
            i->nextLevel = j;
            i->nextEntry = tab->address[j];
            return e->symbol;
        }
        }
    i->nextEntry = NULL;
}
return e->symbol;
}


void symtabDeleteIterator(void *iteratorHandle){
    free(iteratorHandle);


}