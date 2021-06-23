/*!
    \file       cgcs_slist_demo.c
    \brief      Client source file for cgcs_slist

    \author     Gemuele Aludino
    \date       24 May 2021
 */

#define USE_CGCS_SLIST
#include "cgcs_slist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Callback functions for (char *) type
static inline void print_cstr(void *arg) { printf(" %s ", *(char **)(arg)); }
static inline void free_cstr(void *arg) { free(*(char **)(arg)); } 

static inline int cmp_cstr(const void *lhs, const void *rhs) {
    return strcmp(*(char **)(lhs), *(char **)(rhs));
}

// Callback functions for (int) type
static inline void print_int(void *arg) { printf(" %d ", *(int *)(arg)); }


static inline int cmp_int(const void *lhs, const void *rhs) {
    return *(int *)(lhs) - *(int *)(rhs);
}

// Sample usage of slist (ints).
void use_slist_int();

// Sample usage of slist of dynamically-allocated (char *)
void add_strings(slist *sl);
void print_slist_strings(slist *sl);
void deinitialize_slist_strings(slist *sl);
void find_and_replace_string(slist *sl);
void insert_string(slist *sl);

int main(int argc, const char *argv[]) {
    // Sample usage of an slist of int.
    // Population/insertion, search & replace, and deinitialization
    // are much simpler for non-pointer types.
    // (or rather, types that do not require storage on the free-store)
    use_slist_int();

    // USE_CGCS_LIST is defined above #include "cgcs_list.h"
    // cgcs_slist typedef'ed as slist.
    // cgcs_ prefix is now optional for all slist functions. 
    // Initialize an slist.
    slist slist_strings = CGCS_SLIST_INITIALIZER;

    print_slist_strings(&slist_strings);
    
    // Insert strings into slist.
    add_strings(&slist_strings);
    print_slist_strings(&slist_strings);
    
    // Walk the list, find a string.
    // Replace the found string with a new one --
    // then insert a new string after that position.
    find_and_replace_string(&slist_strings);
    print_slist_strings(&slist_strings); 
    
    // Walk the list, and for each node:
    //  - free the string at that node
    //  - free the node itself.
    deinitialize_slist_strings(&slist_strings);
    print_slist_strings(&slist_strings);

    return 0;
}

void use_slist_int() {
    // Create an slist of int.
    slist slist_ints = CGCS_SLIST_INITIALIZER;
    slist *sl = &slist_ints;
    
    // Add numbers [1, 10], pushf will have elements in descending order from the head.
    for (int i = 1; i <= 10; i++) {
        slpushf(sl, &i);
        // equivalent to: slinsertaft(sl, slbefbegin(sl), &i);
    }
   
    slist_iterator it = (slist_iterator)(0);

    // Search for a key. Use the iterator to mutate the data at that position.
    int key = 4; 
    if ((it = slfind(sl, cmp_int, &key)) != slend(sl)) {
        printf("found: %d\n", *(int *)(it));

        // Mutate our found element from 4 to 400.
        *(int *)(it) = 400;
    }
     
    // Insert n after position it.
    int n = 4359;
    it = slinsertaft(sl, it, &n);
    
    // Print the state of slist_ints
    printf("{");
    slforeach(sl, print_int);
    printf("}");
    
    // Destroy all nodes to deinitialize the list.
    while (!slempty(sl)) {
        // Data type does not require manual deallocation,
        // we can pop the node now.
        slpopf(sl);
    }
   
    // This is fine too
    /*
    slist_iterator iter = slbefbegin(sl);
    while (iter != slend(sl)) {
        slist_iterator victim = iter->m_next;
        iter = victim->m_next;
        cgcs_sndelete(victim); 
    }
    */
}

void add_strings(slist *sl) {
    const char *arr[] = {
        "Beta",
        "Delta",
        "Alpha",
        "Foxtrot",
        "Charlie",
        "Echo",
        "Hotel",
        "Golf"
    };

    // You can only insert after/erase after
    // your current position (node) in the slist.
    // befbegin (before_begin) is the node
    // before the true head of the list.
    slist_iterator it = slbefbegin(sl);
    const size_t count = sizeof(arr) / sizeof *arr;

    for (size_t i = 0; i < count; i++) {
        // Allocate memory for a string.
        char *str = malloc(strlen(arr[i]) + 1);
        // Copy the bytes from arr[i] to str.
        strcpy(str, arr[i]);
        
        // Insert str after position it.
        // An iterator (node)to the newly inserted element is returned.
        it = slinsertaft(sl, it, &str);
    }
}

void print_slist_strings(slist *sl) {
    // Printing an slist involves:
    // Using the slforeach function and providing a (void (*callback)(void *)) parameter.
    // slforeach will invoke callback(&(it->m_data)) for each node in sl.
    // The callback function, print_cstr, is designed to accept a (char **) argument,
    // which will then be dereferenced before it is written to stdout.

    printf("\n{");
    slforeach(sl, print_cstr);
    printf("}\n"); 
    
    // Alternatively, you can iterate over the slist yourself -- this is what
    // slforeach does anyway. An slist_iterator is merely a (struct snode *).
    //printf("\n{");
    //for (slist_iterator it = slbegin(sl); it != slend(sl); it = it->m_next) {
    //    printf(" %s ", *(char **)(it));
    //}
    //printf("}\n");
}

void find_and_replace_string(slist *sl) {
    // Look for a node with the string: "Charlie".
    const char *charlie = "Charlie";
    slist_iterator it = (slist_iterator)(0);
    // Note that an slist_iterator is a (struct snode *),
    // or simply, (snode *). Its layout is the following:
    // struct cgcs_snode { void *m_data; struct cgcs_snode *m_next; }
    //
    // To access a type (T) within it->m_data, do the following:
    // T item = *(T *)(&(it->m_data));
    // or,
    // T item = *(T *)(it);
    // The address of an instance is shared with the address of that instance's first field.
    // The typename T for this slist is (char *).

    if ((it = slfind(sl, cmp_cstr, &charlie)) != slend(sl)) {
        // If "Charlie" is found within sl, an iterator (node) to that element
        // will be returned. Otherwise, sl.end(), or rather, slend(sl) is returned.
        printf("[slfind]: '%s'\n", *(char **)(it));
        
        // Using the iterator (node), we have direct access to the element it holds.
        // We will replace "Charlie", with a new string, "Chase".
        const char *chase = "Chase";
        printf("Now replacing '%s' with '%s'\n", *(char **)(it), chase);

        // Release the previous string's memory.
        free(it->m_data);
         
        // Create the dynamically-allocated string.
        // A shorter version of this would be:
        // char *str = strdup("Chase").
        // Not every system will have strdup.
        char *str = malloc(strlen(chase) + 1);
        strcpy(str, chase);
        
        // Mutate our found element from "Charlie" to "Chase".
        *(char **)(it) = str;

        // What we've done is:
        // -- Find a node with the string "Charlie". Return the node.
        // -- Replace the data at that node with a new string, "Chase".
        //      -- Free the old data
        //      -- Create the new data.
        //      -- Assign the new data.
    
        //
        // Let's insert a new string after "Chase".
        // Note that we already have it, which has element "Chase".
        // We need not walk the list to find "Chase" again
        // to make the desired insertion!
        // 

        // Create and allocate memory for the string to insert. 
        const char *hotel = "Hotel";
        str = malloc(strlen(hotel) + 1);
        strcpy(str, hotel);

        // Insert a new string into sl, after position it.
        it = slinsertaft(sl, it, &str);

        printf("Inserted new string: %s\n", *(char **)(it));
    } else {
        printf("[slfind]: '(failed)'\n");
    }
}

void deinitialize_slist_strings(slist *sl) {
    // Go through each node and:
        // - Free the memory 
        //   addressed by the (void *) field within the node
        // - Free the memory addressed by the node itself
    while (!slempty(sl)) {
        char *front = *(char **)(slfront(sl));
        free_cstr(&front); 
        slpopf(sl);
    }
}

