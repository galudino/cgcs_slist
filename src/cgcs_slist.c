/*!
    \file       cgcs_slist.c
    \brief      Source file for singly linked list

    \author     Gemuele Aludino
    \date       24 May 2021

    TODO: Documentation, more comments
 */

#include "cgcs_slist.h"

#include <stdio.h>

struct cgcs_snode *
cgcs_snnew(const void *data) {
    struct cgcs_snode *new_node = malloc(sizeof *new_node);
    assert(new_node);
    cgcs_sninit(new_node, data);
    return new_node;
}

struct cgcs_snode *
cgcs_snnew_allocfn(const void *data, void *(*allocfn)(size_t)) {
    struct cgcs_snode *new_node = allocfn(sizeof *new_node);
    assert(new_node);
    cgcs_sninit(new_node, data);
    return new_node;
}

void cgcs_sldeinit(cgcs_slist *self) {
    cgcs_slist_iterator it = cgcs_slbefbegin(self);

    while (it != cgcs_slend(self)) {
        it = cgcs_sleraseaft(self, it);
    }
}

void cgcs_sldeinit_freefn(cgcs_slist *self, void (*freefn)(void *)) {
    cgcs_slist_iterator it = cgcs_slbefbegin(self);

    while (it != cgcs_slend(self)) {
        it = cgcs_sleraseaft_freefn(self, it, freefn);
    }
}

cgcs_slist_iterator 
cgcs_slinsertaft(cgcs_slist *self,
                 cgcs_slist_iterator it,
                 const void *data) {
    struct cgcs_snode *new_node = cgcs_snnew(data);
    cgcs_snhookaft(new_node, it);
    return it->m_next;
}

cgcs_slist_iterator
cgcs_slinsertaft_allocfn(cgcs_slist *self,
                         cgcs_slist_iterator it,
                         const void *data,
                         void *(*allocfn)(size_t)) {
    struct cgcs_snode *new_node = cgcs_snnew_allocfn(data, allocfn);
    cgcs_snhookaft(new_node, it); 
    // new_node->m_next == it->m_next
    // it->m_next == new_node

    return it->m_next;
}

cgcs_slist_iterator
cgcs_sleraseaft(cgcs_slist *self, cgcs_slist_iterator it) {
    if (cgcs_slempty(self)) {
        return cgcs_slend(self);
    }

    cgcs_slist_iterator old_node = it->m_next;

    cgcs_snunhookaft(it);
    // it->m_next == it->m_next->m_next == old_node->m_next
    cgcs_sndelete(old_node);

    return it->m_next;
}

cgcs_slist_iterator
cgcs_sleraseaft_freefn(cgcs_slist *self,
                       cgcs_slist_iterator it,
                       void (*freefn)(void *)) {
    if (cgcs_slempty(self)) {
        return cgcs_slend(self);
    }

    cgcs_snunhookaft(it);
    // it->m_next == it->m_next->m_next == old_node->m_next
    cgcs_sndelete_freefn(it, freefn);

    return it->m_next;
}

void cgcs_slforeach(cgcs_slist *self, void (*func)(void *)) {
    for (cgcs_slist_iterator it = cgcs_slbegin(self);
         it != cgcs_slend(self);
         it = it->m_next) {
        func(&(it->m_data));
    }
}

void cgcs_slforeach_range(cgcs_slist *self, void (*func)(void *),
                          cgcs_slist_iterator beg,
                          cgcs_slist_iterator end) {
    for (cgcs_slist_iterator it = beg;
         it != end;
         it = it->m_next) {
        func(&(it->m_data));
    }
}

cgcs_slist_iterator cgcs_slfind(cgcs_slist *self,
                                int (*cmpfn)(const void *, const void *),
                                const void *data) {
    for (cgcs_slist_iterator it = cgcs_slbegin(self);
         it != cgcs_slend(self);
         it = it->m_next) {
        if (cmpfn(data, &(it->m_data)) == 0) {
            return it;
        }
    }
    
    return cgcs_slend(self);
}

cgcs_slist_iterator cgcs_slfind_range(cgcs_slist *self,
                                      int (*cmpfn)(const void *, const void *),
                                      const void *data,
                                      cgcs_slist_iterator beg,
                                      cgcs_slist_iterator end) {
    for (cgcs_slist_iterator it = beg;
         it != end;
         it = it->m_next) {
        if (cmpfn(data, &(it->m_data)) == 0) {
            return it;
        }
    }
    
    return cgcs_slend(self);
}

cgcs_slist *cgcs_slnew() {
    cgcs_slist *sl = malloc(sizeof *sl);
    assert(sl);

    cgcs_slinit(sl);
    return sl;
}

cgcs_slist *cgcs_slnew_allocfn(void *(*allocfn)(size_t)) {
    cgcs_slist *sl = allocfn(sizeof *sl);
    assert(sl);

    cgcs_slinit(sl);
    return sl;
}

void cgcs_sldelete(cgcs_slist *sl) {
    cgcs_sldeinit(sl);
    free(sl);
}

void cgcs_sldelete_freefn(cgcs_slist *sl, void (*freefn)(void *)) {
    cgcs_sldeinit_freefn(sl, freefn);
    freefn(sl);
}
