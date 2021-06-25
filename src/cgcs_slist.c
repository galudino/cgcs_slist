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

struct cgcs_snode *cgcs_snclearaft(struct cgcs_snode *x, struct cgcs_snode *y) {
    while (x->m_next != y) {
        cgcs_sneraseaft(x);
    }

    return y;
}

struct cgcs_snode *cgcs_snclearaft_freefn(struct cgcs_snode *x, struct cgcs_snode *y, void (*freefn)(void *)) {
    while (x->m_next != y) {
        cgcs_sneraseaft_freefn(y, freefn);
    }

    return y;
}

struct cgcs_snode *cgcs_snfind(struct cgcs_snode *x, struct cgcs_snode *y, const void *data, int (*cmpfn)(const void *, const void *)) {
    for (struct cgcs_snode *curr = x; curr != y; curr = curr->m_next) {
        if (cmpfn(curr->m_data, data) == 0) {
            return curr;
        }
    }

    return NULL;
}

struct cgcs_snode *cgcs_snfind_b(struct cgcs_snode *x, struct cgcs_snode *y, const void *data, int (^cmp_b)(const void *, const void *)) {
    for (struct cgcs_snode *curr = x; curr != y; curr = curr->m_next) {
        if (cmp_b(curr->m_data, data) == 0) {
            return curr;
        }
    }

    return NULL;    
}

struct cgcs_snode *cgcs_snadvance(struct cgcs_snode **x, int index) {
    for (int i = 0; i < index; i++) {
        (*x) = (*x)->m_next;
    }

    return (*x);
}

struct cgcs_snode *cgcs_sngetnode(struct cgcs_snode *x, int index) {
    return cgcs_snadvance(&x, index);
}

struct cgcs_snode *cgcs_sntransaft(struct cgcs_snode *x, struct cgcs_snode *start) {
    struct cgcs_snode *finish = start->m_next;
    while (finish && finish->m_next) {
        finish = finish->m_next;
    }

    return cgcs_sntransaft_range(x, start, finish);
}

struct cgcs_snode *cgcs_sntransaft_range(struct cgcs_snode *x, struct cgcs_snode *start, struct cgcs_snode *finish) {
    struct cgcs_snode *keep = start->m_next;

    if (finish) {
        start->m_next = finish->m_next;
        finish->m_next = x->m_next;
    } else {
        start->m_next = NULL;
    }

    x->m_next = keep;
    return finish;
}

void cgcs_snreverseaft(struct cgcs_snode *x) {
    struct cgcs_snode *tail = x->m_next;
    struct cgcs_snode *temp = NULL;

    if (!tail) {
        return;
    }

    while ((temp = tail->m_next)) {
        struct cgcs_snode *keep = x->m_next;
        x->m_next = temp;

        tail->m_next = temp->m_next;
        x->m_next->m_next = keep;
    }
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
