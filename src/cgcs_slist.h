/*!
    \file       cgcs_slist.h
    \brief      Header file for singly linked list

    \author     Gemuele Aludino
    \date       24 May 2021

    TODO: Documentation, more comments
 */

#ifndef CGCS_SLIST_H
#define CGCS_SLIST_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef void *voidptr;

struct cgcs_snode {
    voidptr m_data;
    struct cgcs_snode *m_next;
};

#define CGCS_SNODE_INITIALIZER { NULL, (struct cgcs_snode *)(0) }

static void cgcs_sninit(struct cgcs_snode *self, const void *data);
static void cgcs_sndeinit(struct cgcs_snode *self);

static void cgcs_snhookaft(struct cgcs_snode *self, struct cgcs_snode *pos);
static void cgcs_snunhookaft(struct cgcs_snode *self);

struct cgcs_snode *cgcs_snnew(const void *data);
struct cgcs_snode *cgcs_snnew_allocfn(const void *data,
                                          void *(*allocfn)(size_t));
static void cgcs_sndelete(struct cgcs_snode *node);
static void cgcs_sndelete_freefn(struct cgcs_snode *node,
                                     void (*freefn)(void *));

static struct cgcs_snode *cgcs_sninsertaft(struct cgcs_snode *x, const void *data);
static struct cgcs_snode *cgcs_sninsertaft_allocfn(struct cgcs_snode *x, const void *data, void *(*allocfn)(size_t));
static struct cgcs_snode *cgcs_sneraseaft(struct cgcs_snode *x);
static struct cgcs_snode *cgcs_sneraseaft_freefn(struct cgcs_snode *x, void (*freefn)(void *));

struct cgcs_snode *cgcs_snclearaft(struct cgcs_snode *x, struct cgcs_snode *y);
struct cgcs_snode *cgcs_snclearaft_freefn(struct cgcs_snode *x, struct cgcs_snode *y, void (*freefn)(void *));

struct cgcs_snode *cgcs_snfind(struct cgcs_snode *x, struct cgcs_snode *y, const void *data, int (*cmpfn)(const void *, const void *));
struct cgcs_snode *cgcs_snfind_b(struct cgcs_snode *x, struct cgcs_snode *y, const void *data, int (^cmp_b)(const void *, const void *));
struct cgcs_snode *cgcs_snadvance(struct cgcs_snode **x, int index);
struct cgcs_snode *cgcs_sngetnode(struct cgcs_snode *x, int index);

struct cgcs_snode *cgcs_sntransaft(struct cgcs_snode *x, struct cgcs_snode *start);
struct cgcs_snode *cgcs_sntransaft_range(struct cgcs_snode *x, struct cgcs_snode *start, struct cgcs_snode *finish);
void cgcs_snreverseaft(struct cgcs_snode *x);

static inline void
cgcs_sninit(struct cgcs_snode *self, const void *data) {
    self->m_next = (struct cgcs_snode *)(0);
    // data is the address of a (T *).
    // Therefore, we pass &(self->m_data) to memcpy --
    // which is the address of a (void *).
    memcpy(&(self->m_data), data, sizeof(void *));
}

static inline void
cgcs_sndeinit(struct cgcs_snode *self) {
    self->m_next = (struct cgcs_snode *)(0);
}

static inline void
cgcs_snhookaft(struct cgcs_snode *self,
                struct cgcs_snode *pos) {
    // "Hook" node self after node pos.
    self->m_next = pos->m_next;
    pos->m_next = self;
    // Node self's new neighbor is now pos's former neighbor.
    // pos's current neighbor is now self.
}

static inline void
cgcs_snunhookaft(struct cgcs_snode *self) {
    self->m_next = self->m_next->m_next;
    // The node formerly after self (formerly self->m_next)
    // is now "unhooked" from the list.
    // The caller should save self->m_next prior to this code.
}

static inline void
cgcs_sndelete(struct cgcs_snode *node) {
    // Destruction of node->m_data should be handled by the caller
    // prior to calling this function.
    cgcs_sndeinit(node);
    free(node);
}

static inline void
cgcs_sndelete_freefn(struct cgcs_snode *node, void (*freefn)(void *)) {
    cgcs_sndeinit(node);
    freefn(node);
}

static inline struct cgcs_snode *
cgcs_sninsertaft(struct cgcs_snode *x, const void *data) {
    struct cgcs_snode *node = cgcs_snnew(data);
    cgcs_snhookaft(node, x);
    return x->m_next;
}
static inline 
struct cgcs_snode *cgcs_sninsertaft_allocfn(struct cgcs_snode *x, const void *data, void *(*allocfn)(size_t)) {
    struct cgcs_snode *node = cgcs_snnew_allocfn(data, allocfn);
    cgcs_snhookaft(node, x);
    return x->m_next;
}

static inline 
struct cgcs_snode *cgcs_sneraseaft(struct cgcs_snode *x) {
    if (x->m_next == NULL) {
        return NULL;
    }

    struct cgcs_snode *victim = x->m_next;

    cgcs_snunhookaft(x);
    cgcs_sndelete(victim);

    return x->m_next;
}

static inline 
struct cgcs_snode *cgcs_sneraseaft_freefn(struct cgcs_snode *x, void (*freefn)(void *)) {
    if (x->m_next == NULL) {
        return NULL;
    }

    struct cgcs_snode *victim = x->m_next;

    cgcs_snunhookaft(x);
    cgcs_sndelete_freefn(victim, freefn);

    return x->m_next; 
}

typedef struct cgcs_slist cgcs_slist;
typedef struct cgcs_snode *cgcs_slist_iterator;

struct cgcs_slist {
    struct cgcs_snode m_impl;
};

#define CGCS_SLIST_INITIALIZER { CGCS_SNODE_INITIALIZER }

static void cgcs_slinit(cgcs_slist *self);

void cgcs_sldeinit(cgcs_slist *self);
void cgcs_sldeinit_freefn(cgcs_slist *self,
                          void (*freefn)(void *));

static voidptr cgcs_slfront(cgcs_slist *self);
static bool cgcs_slempty(cgcs_slist *self);

static cgcs_slist_iterator cgcs_slbefbegin(cgcs_slist *self);
static cgcs_slist_iterator cgcs_slbegin(cgcs_slist *self);
static cgcs_slist_iterator cgcs_slend(cgcs_slist *self);

cgcs_slist_iterator cgcs_slinsertaft(cgcs_slist *self,
                                     cgcs_slist_iterator it,
                                     const void *data);

cgcs_slist_iterator cgcs_slinsertaft_allocfn(cgcs_slist *self,
                                             cgcs_slist_iterator it,
                                             const void *data,
                                             void *(*allocfn)(size_t));

cgcs_slist_iterator cgcs_sleraseaft(cgcs_slist *self,
                                    cgcs_slist_iterator it);

cgcs_slist_iterator cgcs_sleraseaft_freefn(cgcs_slist *self,
                                           cgcs_slist_iterator it,
                                           void (*freefn)(void *));

static void cgcs_slpushf(cgcs_slist *self, const void *data);

static void cgcs_slpushf_allocfn(cgcs_slist *self,
                                 const void *data,
                                 void *(*allocfn)(size_t));

static void cgcs_slpopf(cgcs_slist *self);
static void cgcs_slpopf_freefn(cgcs_slist *self, void (*freefn)(void *));

void cgcs_slforeach(cgcs_slist *self, void (*func)(void *));

void cgcs_slforeach_range(cgcs_slist *self, void (*func)(void *),
                          cgcs_slist_iterator beg,
                          cgcs_slist_iterator end);

cgcs_slist_iterator cgcs_slfind(cgcs_slist *self,
                                  int (*cmpfn)(const void *, const void *),
                                  const void *data);

cgcs_slist_iterator cgcs_slfind_range(cgcs_slist *self,
                                        int (*cmpfn)(const void *, const void *),
                                        const void *data,
                                        cgcs_slist_iterator beg,
                                        cgcs_slist_iterator end);

cgcs_slist *cgcs_slnew();
cgcs_slist *cgcs_slnew_allocfn(void *(*allocfn)(size_t));

void cgcs_sldelete(cgcs_slist *sl);
void cgcs_sldelete_freefn(cgcs_slist *sl, void (*freefn)(void *));

static inline void
cgcs_slinit(cgcs_slist *self) {
    self->m_impl.m_next = cgcs_slend(self);
    self->m_impl.m_data = NULL;
}

static inline voidptr
cgcs_slfront(cgcs_slist *self) {
    return &(cgcs_slbegin(self)->m_data);
}

static inline bool cgcs_slempty(cgcs_slist *self) {
    return cgcs_slbegin(self) == cgcs_slend(self);
}

static inline cgcs_slist_iterator
cgcs_slbefbegin(cgcs_slist *self) {
    return ((cgcs_slist_iterator)(&(self->m_impl)));
}

static inline cgcs_slist_iterator
cgcs_slbegin(cgcs_slist *self) {
    return ((cgcs_slist_iterator)(self->m_impl.m_next));
}

static inline cgcs_slist_iterator
cgcs_slend(cgcs_slist *self) {
    return ((cgcs_slist_iterator)(0));
}

static inline void
cgcs_slpushf(cgcs_slist *self, const void *data) {
    cgcs_slinsertaft(self, cgcs_slbefbegin(self), data);
}

static inline void
cgcs_slpushf_allocfn(cgcs_slist *self,
                     const void *data,
                     void *(*allocfn)(size_t)) {
    cgcs_slinsertaft_allocfn(self, cgcs_slbefbegin(self), data, allocfn);
}

static inline void
cgcs_slpopf(cgcs_slist *self) {
    cgcs_sleraseaft(self, cgcs_slbefbegin(self));
}

static inline void
cgcs_slpopf_freefn(cgcs_slist *self, void (*freefn)(void *)) {
    cgcs_sleraseaft_freefn(self, cgcs_slbefbegin(self), freefn);
}

/*!
    \def        USE_CGCS_SLIST
    \brief      Macro to omit 'cgcs_' namespace prefix for all public cgcs_slist identifiers
    \details    Use '#define USE_CGCS_SLIST' before #include "cgcs_list.h"
 */
#ifdef USE_CGCS_SLIST
# define USE_CGCS_SLIST

typedef struct cgcs_snode cgcs_snode;
typedef struct cgcs_snode snode;

#define sninit(self, data)                      cgcs_sninit(self, data)
#define sndeinit(self)                          cgcs_sndeinit(self)

#define snnew(data)                             cgcs_snnew(data)
#define snnew_allocfn(data, allocfn)            cgcs_snnew_allocfn(data, allocfn)

#define sndelete(node)                          cgcs_sndelete(node)
#define sndelete_freefn(node, freefn)           cgcs_sndelete_freefn(node, freefn)

#define sninsertaft(x, data)                    cgcs_sninsertaft(x, data)
#define sninsertaft_allocfn(x, data, allocfn)   cgcs_sninsertaft_allocfn(x, data, allocfn)
#define sneraseaft(x)                           cgcs_sneraseaft(x)
#define sneraseaft_freefn(x, freefn)

#define snclearaft(x, y)                        cgcs_snclearaft(x, y)
#define snclearaft_freefn(x, y, freefn)         cgcs_snclearaft(x, y, freefn)

#define snfind(x, y, data, cmpfn)               cgcs_snfind(x, y, data, cmpfn)
#define snfind_b(x, y, data, cmp_b)             cgcs_snfind_b(x, y, data, cmp_b)
#define snadvance(x, index)                     cgcs_snadvance(x, index)
#define sngetnode(x, index)                     cgcs_sngetnode(x, index)

#define sntransaft(x, start) c                  cgcs_sntransaft(x, start)
#define sntransaft_range(x, start, finish)      cgcs_sntransaft_range(x, start, finish)
#define snreverseaft(x)                         cgcs_snreverseaft(x)

typedef cgcs_slist slist;
typedef cgcs_slist_iterator slist_iterator;

#define slinit(self)                                cgcs_slinit(self)
#define sldeinit(self)                              cgcs_sldeinit(self)
#define sldeinit_freefn(self, freefn)               cgcs_sldeinit_freefn(self, freefn)

#define slfront(self)                               cgcs_slfront(self)
#define slempty(self)                               cgcs_slempty(self)

#define slbefbegin(self)                            cgcs_slbefbegin(self)
#define slbegin(self)                               cgcs_slbegin(self)
#define slend(self)                                 cgcs_slend(self)

#define slinsertaft(self, it, data)                 cgcs_slinsertaft(self, it, data)
#define slinsertaft_allocfn(self, i, d, a)          cgcs_slinsertaft_allocfn(self, i, d, a)
#define sleraseaft(self, it)                        cgcs_sleraseaft(self, it)
#define sleraseaft_freefn(self, it, freefn)         cgcs_sleraseaft_freefn(self, it, freefn)

#define slpushf(self, data)                         cgcs_slpushf(self, data)
#define slpushf_allocfn(self, data, allocfn)        cgcs_slpushf_allocfn(self, data, allocfn)
#define slpopf(self)                                cgcs_slpopf(self)
#define slpopf_freefn(self, freefn)                 cgcs_slpopf(self, freefn)

#define slforeach(self, func)                       cgcs_slforeach(self, func)
#define slforeach_range(self, func, beg, end)       cgcs_slforeach_range(self, func, beg, end)

#define slfind(self, cmpfn, data)                   cgcs_slfind(self, cmpfn, data)
#define slfind_range(self, cmpfn, data, beg, end)   cgcs_slfind_range(self, cmpfn, data, beg, end)

#define slnew()                                     cgcs_slnew()
#define slnew_allocfn(allocfn)                      cgcs_slnew_allocfn(allocfn)
#define sldelete(sl)                                cgcs_sldelete(sl)
#define sldelete_freefn(sl, freefn)                 cgcs_sldelete_freenf(sl, freefn)

// TODO: More shorthand notation, as per cgcs_vector

#endif /* USE_CGCS_SLIST */

#endif /* CGCS_LIST_H */
