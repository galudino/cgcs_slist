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

struct cgcs_slist_node {
    voidptr m_data;
    struct cgcs_slist_node *m_next;
};

#define CGCS_SNODE_INITIALIZER { NULL, (struct cgcs_slist_node *)(0) }

static void slist_node_init(struct cgcs_slist_node *self, const void *data);
static void slist_node_deinit(struct cgcs_slist_node *self);

static void slist_node_hook_after(struct cgcs_slist_node *self, struct cgcs_slist_node *pos);
static void slist_node_unhook_after(struct cgcs_slist_node *self);

struct cgcs_slist_node *slist_node_new(const void *data);
struct cgcs_slist_node *slist_node_alloc_fn(const void *data,
                                          void *(*allocfn)(size_t));
static void slist_node_delete(struct cgcs_slist_node *node);
static void slist_node_free_fn(struct cgcs_slist_node *node,
                                     void (*freefn)(void *));

static struct cgcs_slist_node *slist_node_insert_after(struct cgcs_slist_node *x, const void *data);
static struct cgcs_slist_node *slist_node_insert_after_alloc_fn(struct cgcs_slist_node *x, const void *data, void *(*allocfn)(size_t));
static struct cgcs_slist_node *slist_node_erase_after(struct cgcs_slist_node *x);
static struct cgcs_slist_node *slist_node_erase_after_freefn(struct cgcs_slist_node *x, void (*freefn)(void *));

struct cgcs_slist_node *slist_node_clear_after(struct cgcs_slist_node *x, struct cgcs_slist_node *y);
struct cgcs_slist_node *slist_node_clear_after_free_fn(struct cgcs_slist_node *x, struct cgcs_slist_node *y, void (*freefn)(void *));

struct cgcs_slist_node *slist_node_find(struct cgcs_slist_node *x, struct cgcs_slist_node *y, const void *data, int (*cmpfn)(const void *, const void *));
struct cgcs_slist_node *slist_node_find_b(struct cgcs_slist_node *x, struct cgcs_slist_node *y, const void *data, int (^cmp_b)(const void *, const void *));
struct cgcs_slist_node *slist_node_advance(struct cgcs_slist_node **x, int index);
struct cgcs_slist_node *slist_node_get(struct cgcs_slist_node *x, int index);

struct cgcs_slist_node *slist_node_transfer_after(struct cgcs_slist_node *x, struct cgcs_slist_node *start);
struct cgcs_slist_node *slist_node_transfer_after_range(struct cgcs_slist_node *x, struct cgcs_slist_node *start, struct cgcs_slist_node *finish);
void cgcs_snreverseaft(struct cgcs_slist_node *x);

static inline void
slist_node_init(struct cgcs_slist_node *self, const void *data) {
    self->m_next = (struct cgcs_slist_node *)(0);
    // data is the address of a (T *).
    // Therefore, we pass &(self->m_data) to memcpy --
    // which is the address of a (void *).
    memcpy(&(self->m_data), data, sizeof(void *));
}

static inline void
slist_node_deinit(struct cgcs_slist_node *self) {
    self->m_next = (struct cgcs_slist_node *)(0);
}

static inline void
slist_node_hook_after(struct cgcs_slist_node *self,
                struct cgcs_slist_node *pos) {
    // "Hook" node self after node pos.
    self->m_next = pos->m_next;
    pos->m_next = self;
    // Node self's new neighbor is now pos's former neighbor.
    // pos's current neighbor is now self.
}

static inline void
slist_node_unhook_after(struct cgcs_slist_node *self) {
    self->m_next = self->m_next->m_next;
    // The node formerly after self (formerly self->m_next)
    // is now "unhooked" from the list.
    // The caller should save self->m_next prior to this code.
}

static inline void
slist_node_delete(struct cgcs_slist_node *node) {
    // Destruction of node->m_data should be handled by the caller
    // prior to calling this function.
    slist_node_deinit(node);
    free(node);
}

static inline void
slist_node_free_fn(struct cgcs_slist_node *node, void (*freefn)(void *)) {
    slist_node_deinit(node);
    freefn(node);
}

static inline struct cgcs_slist_node *
slist_node_insert_after(struct cgcs_slist_node *x, const void *data) {
    struct cgcs_slist_node *node = slist_node_new(data);
    slist_node_hook_after(node, x);
    return x->m_next;
}
static inline 
struct cgcs_slist_node *slist_node_insert_after_alloc_fn(struct cgcs_slist_node *x, const void *data, void *(*allocfn)(size_t)) {
    struct cgcs_slist_node *node = slist_node_alloc_fn(data, allocfn);
    slist_node_hook_after(node, x);
    return x->m_next;
}

static inline 
struct cgcs_slist_node *slist_node_erase_after(struct cgcs_slist_node *x) {
    if (x->m_next == NULL) {
        return NULL;
    }

    struct cgcs_slist_node *victim = x->m_next;

    slist_node_unhook_after(x);
    slist_node_delete(victim);

    return x->m_next;
}

static inline 
struct cgcs_slist_node *slist_node_erase_after_freefn(struct cgcs_slist_node *x, void (*freefn)(void *)) {
    if (x->m_next == NULL) {
        return NULL;
    }

    struct cgcs_slist_node *victim = x->m_next;

    slist_node_unhook_after(x);
    slist_node_free_fn(victim, freefn);

    return x->m_next; 
}

typedef struct cgcs_slist slist_t;
typedef struct cgcs_slist_node *slist_iterator_t;

struct cgcs_slist {
    struct cgcs_slist_node m_impl;
};

#define CGCS_SLIST_INITIALIZER { CGCS_SNODE_INITIALIZER }

static void slist_init(slist_t *self);

void slist_deinit(slist_t *self);
void slist_deinit_free_fn(slist_t *self,
                          void (*freefn)(void *));

static voidptr slist_front(slist_t *self);
static bool slist_empty(slist_t *self);

static slist_iterator_t slist_before_begin(slist_t *self);
static slist_iterator_t slist_begin(slist_t *self);
static slist_iterator_t slist_end(slist_t *self);

slist_iterator_t slist_insert_after(slist_t *self,
                                     slist_iterator_t it,
                                     const void *data);

slist_iterator_t slist_insert_after_alloc_fn(slist_t *self,
                                             slist_iterator_t it,
                                             const void *data,
                                             void *(*allocfn)(size_t));

slist_iterator_t slist_erase_after(slist_t *self,
                                    slist_iterator_t it);

slist_iterator_t slist_erase_after_free_fn(slist_t *self,
                                           slist_iterator_t it,
                                           void (*freefn)(void *));

static void slist_push_front(slist_t *self, const void *data);

static void slist_push_front_alloc_fn(slist_t *self,
                                 const void *data,
                                 void *(*allocfn)(size_t));

static void slist_pop_front(slist_t *self);
static void slist_pop_front_free_fn(slist_t *self, void (*freefn)(void *));

void slist_foreach(slist_t *self, void (*func)(void *));

void slist_foreach_range(slist_t *self, void (*func)(void *),
                          slist_iterator_t beg,
                          slist_iterator_t end);

slist_iterator_t slist_find(slist_t *self,
                                  int (*cmpfn)(const void *, const void *),
                                  const void *data);

slist_iterator_t slist_find_b(slist_t *self,
                                  int (^cmp_b)(const void *, const void *),
                                  const void *data);

slist_iterator_t slist_find_range(slist_t *self,
                                        int (*cmpfn)(const void *, const void *),
                                        const void *data,
                                        slist_iterator_t beg,
                                        slist_iterator_t end);

slist_iterator_t slist_find_range_b(slist_t *self,
                                        int (^cmp_b)(const void *, const void *),
                                        const void *data,
                                        slist_iterator_t beg,
                                        slist_iterator_t end);

slist_t *slist_new();
slist_t *slist_new_alloc_fn(void *(*allocfn)(size_t));

void slist_delete(slist_t *sl);
void slist_delete_free_fn(slist_t *sl, void (*freefn)(void *));

static inline void
slist_init(slist_t *self) {
    self->m_impl.m_next = slist_end(self);
    self->m_impl.m_data = NULL;
}

static inline voidptr
slist_front(slist_t *self) {
    return &(slist_begin(self)->m_data);
}

static inline bool slist_empty(slist_t *self) {
    return slist_begin(self) == slist_end(self);
}

static inline slist_iterator_t
slist_before_begin(slist_t *self) {
    return ((slist_iterator_t)(&(self->m_impl)));
}

static inline slist_iterator_t
slist_begin(slist_t *self) {
    return ((slist_iterator_t)(self->m_impl.m_next));
}

static inline slist_iterator_t
slist_end(slist_t *self) {
    return ((slist_iterator_t)(0));
}

static inline void
slist_push_front(slist_t *self, const void *data) {
    slist_insert_after(self, slist_before_begin(self), data);
}

static inline void
slist_push_front_alloc_fn(slist_t *self,
                     const void *data,
                     void *(*allocfn)(size_t)) {
    slist_insert_after_alloc_fn(self, slist_before_begin(self), data, allocfn);
}

static inline void
slist_pop_front(slist_t *self) {
    slist_erase_after(self, slist_before_begin(self));
}

static inline void
slist_pop_front_free_fn(slist_t *self, void (*freefn)(void *)) {
    slist_erase_after_free_fn(self, slist_before_begin(self), freefn);
}

#endif /* CGCS_LIST_H */
