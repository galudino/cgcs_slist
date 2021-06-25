/*!
    \file       cgcs_slist.c
    \brief      Source file for singly linked list

    \author     Gemuele Aludino
    \date       24 May 2021

    TODO: Documentation, more comments
 */

#include "cgcs_slist.h"

#include <stdio.h>

struct cgcs_slist_node *
slist_node_new(const void *data) {
    struct cgcs_slist_node *new_node = malloc(sizeof *new_node);
    assert(new_node);
    slist_node_init(new_node, data);
    return new_node;
}

struct cgcs_slist_node *
slist_node_alloc_fn(const void *data, void *(*allocfn)(size_t)) {
    struct cgcs_slist_node *new_node = allocfn(sizeof *new_node);
    assert(new_node);
    slist_node_init(new_node, data);
    return new_node;
}

struct cgcs_slist_node *slist_node_clear_after(struct cgcs_slist_node *x, struct cgcs_slist_node *y) {
    while (x->m_next != y) {
        slist_node_erase_after(x);
    }

    return y;
}

struct cgcs_slist_node *slist_node_clear_after_free_fn(struct cgcs_slist_node *x, struct cgcs_slist_node *y, void (*freefn)(void *)) {
    while (x->m_next != y) {
        slist_node_erase_after_freefn(y, freefn);
    }

    return y;
}

struct cgcs_slist_node *slist_node_find(struct cgcs_slist_node *x, struct cgcs_slist_node *y, const void *data, int (*cmpfn)(const void *, const void *)) {
    for (struct cgcs_slist_node *curr = x; curr != y; curr = curr->m_next) {
        if (cmpfn(curr->m_data, data) == 0) {
            return curr;
        }
    }

    return NULL;
}

struct cgcs_slist_node *slist_node_find_b(struct cgcs_slist_node *x, struct cgcs_slist_node *y, const void *data, int (^cmp_b)(const void *, const void *)) {
    for (struct cgcs_slist_node *curr = x; curr != y; curr = curr->m_next) {
        if (cmp_b(curr->m_data, data) == 0) {
            return curr;
        }
    }

    return NULL;    
}

struct cgcs_slist_node *slist_node_advance(struct cgcs_slist_node **x, int index) {
    for (int i = 0; i < index; i++) {
        (*x) = (*x)->m_next;
    }

    return (*x);
}

struct cgcs_slist_node *slist_node_get(struct cgcs_slist_node *x, int index) {
    return slist_node_advance(&x, index);
}

struct cgcs_slist_node *slist_node_transfer_after(struct cgcs_slist_node *x, struct cgcs_slist_node *start) {
    struct cgcs_slist_node *finish = start->m_next;
    while (finish && finish->m_next) {
        finish = finish->m_next;
    }

    return slist_node_transfer_after_range(x, start, finish);
}

struct cgcs_slist_node *slist_node_transfer_after_range(struct cgcs_slist_node *x, struct cgcs_slist_node *start, struct cgcs_slist_node *finish) {
    struct cgcs_slist_node *keep = start->m_next;

    if (finish) {
        start->m_next = finish->m_next;
        finish->m_next = x->m_next;
    } else {
        start->m_next = NULL;
    }

    x->m_next = keep;
    return finish;
}

void cgcs_snreverseaft(struct cgcs_slist_node *x) {
    struct cgcs_slist_node *tail = x->m_next;
    struct cgcs_slist_node *temp = NULL;

    if (!tail) {
        return;
    }

    while ((temp = tail->m_next)) {
        struct cgcs_slist_node *keep = x->m_next;
        x->m_next = temp;

        tail->m_next = temp->m_next;
        x->m_next->m_next = keep;
    }
}

void slist_deinit(slist_t *self) {
    slist_iterator_t it = slist_before_begin(self);

    while (it != slist_end(self)) {
        it = slist_erase_after(self, it);
    }
}

void slist_deinit_free_fn(slist_t *self, void (*freefn)(void *)) {
    slist_iterator_t it = slist_before_begin(self);

    while (it != slist_end(self)) {
        it = slist_erase_after_free_fn(self, it, freefn);
    }
}

slist_iterator_t 
slist_insert_after(slist_t *self,
                 slist_iterator_t it,
                 const void *data) {
    struct cgcs_slist_node *new_node = slist_node_new(data);
    slist_node_hook_after(new_node, it);
    return it->m_next;
}

slist_iterator_t
slist_insert_after_alloc_fn(slist_t *self,
                         slist_iterator_t it,
                         const void *data,
                         void *(*allocfn)(size_t)) {
    struct cgcs_slist_node *new_node = slist_node_alloc_fn(data, allocfn);
    slist_node_hook_after(new_node, it); 
    // new_node->m_next == it->m_next
    // it->m_next == new_node

    return it->m_next;
}

slist_iterator_t
slist_erase_after(slist_t *self, slist_iterator_t it) {
    if (slist_empty(self)) {
        return slist_end(self);
    }

    slist_iterator_t old_node = it->m_next;

    slist_node_unhook_after(it);
    // it->m_next == it->m_next->m_next == old_node->m_next
    slist_node_delete(old_node);

    return it->m_next;
}

slist_iterator_t
slist_erase_after_free_fn(slist_t *self,
                       slist_iterator_t it,
                       void (*freefn)(void *)) {
    if (slist_empty(self)) {
        return slist_end(self);
    }

    slist_node_unhook_after(it);
    // it->m_next == it->m_next->m_next == old_node->m_next
    slist_node_free_fn(it, freefn);

    return it->m_next;
}

void slist_foreach(slist_t *self, void (*func)(void *)) {
    for (slist_iterator_t it = slist_begin(self);
         it != slist_end(self);
         it = it->m_next) {
        func(&(it->m_data));
    }
}

void slist_foreach_range(slist_t *self, void (*func)(void *),
                          slist_iterator_t beg,
                          slist_iterator_t end) {
    for (slist_iterator_t it = beg;
         it != end;
         it = it->m_next) {
        func(&(it->m_data));
    }
}

slist_iterator_t slist_find(slist_t *self,
                                int (*cmpfn)(const void *, const void *),
                                const void *data) {
    for (slist_iterator_t it = slist_begin(self);
         it != slist_end(self);
         it = it->m_next) {
        if (cmpfn(data, &(it->m_data)) == 0) {
            return it;
        }
    }
    
    return slist_end(self);
}

slist_iterator_t slist_find_b(slist_t *self,
                                  int (^cmp_b)(const void *, const void *),
                              const void *data) {
    for (slist_iterator_t it = slist_begin(self);
         it != slist_end(self);
         it = it->m_next) {
        if (cmp_b(data, &(it->m_data)) == 0) {
            return it;
        }
    }
    
    return slist_end(self);
}

slist_iterator_t slist_find_range(slist_t *self,
                                      int (*cmpfn)(const void *, const void *),
                                      const void *data,
                                      slist_iterator_t beg,
                                      slist_iterator_t end) {
    for (slist_iterator_t it = beg;
         it != end;
         it = it->m_next) {
        if (cmpfn(data, &(it->m_data)) == 0) {
            return it;
        }
    }
    
    return slist_end(self);
}

slist_iterator_t slist_find_range_b(slist_t *self,
                                        int (^cmp_b)(const void *, const void *),
                                        const void *data,
                                        slist_iterator_t beg,
                                    slist_iterator_t end) {
    for (slist_iterator_t it = beg;
         it != end;
         it = it->m_next) {
        if (cmp_b(data, &(it->m_data)) == 0) {
            return it;
        }
    }
    
    return slist_end(self);
}

slist_t *slist_new() {
    slist_t *sl = malloc(sizeof *sl);
    assert(sl);

    slist_init(sl);
    return sl;
}

slist_t *slist_new_alloc_fn(void *(*allocfn)(size_t)) {
    slist_t *sl = allocfn(sizeof *sl);
    assert(sl);

    slist_init(sl);
    return sl;
}

void slist_delete(slist_t *sl) {
    slist_deinit(sl);
    free(sl);
}

void slist_delete_free_fn(slist_t *sl, void (*freefn)(void *)) {
    slist_deinit_free_fn(sl, freefn);
    freefn(sl);
}
