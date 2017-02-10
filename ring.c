#include "libdill.h"

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

struct coro_ring {
    int *buf;
    size_t size;
    size_t start;
    size_t end;

    int (*dtor)(int);
};

struct coro_ring *ring_new(size_t size, int (*dtor_func)(int)) {
    struct coro_ring *r = calloc(1, sizeof(*r));
    if (!r)
        goto error1;
    r->buf = calloc(size, sizeof(int) + 1);
    if (!r->buf)
        goto error2;
    r->size = size + 1;
    r->start = 0;
    r->end = 0;
    r->dtor = dtor_func;
    return r;
    error3:
    free(r->buf);
    error2:
    free(r);
    error1:
    return NULL;
}

void ring_delete(struct coro_ring **ring) {
    if (!ring)
        return;
    while (!ring_pop(*ring)) {}
    free((*ring)->buf);
    free(*ring);
    *ring = NULL;
}

static inline size_t ring_wrap(size_t value, size_t mod) {
    if (value >= mod)
        return value - mod;
    else
        return value;
}

inline int ring_full(const struct coro_ring *ring) {
    return ring_wrap(ring->start + 1, ring->size) == ring->end;
}

inline int ring_empty(const struct coro_ring *ring) {
    return ring->end == ring->start;
}

inline int ring_tail(const struct coro_ring *ring) {
    return ring->buf[ring->end];
}

inline int *ring_tail_p(const struct coro_ring *ring) {
    return &ring->buf[ring->end];
}

int ring_pop(struct coro_ring *ring) {
    int rc = 0;
    if (ring_empty(ring))
        return -1;
    int handle = ring_tail(ring);
    if (ring->dtor)
        rc = ring->dtor(handle);
    ring->end = ring_wrap(ring->end + 1, ring->size);
    if (rc != 0)
        return handle;
    else
        return 0;
}

int ring_push(struct coro_ring *ring, int new_handle) {
    if (!ring->dtor && ring_full(ring))
        return -1;
    int rc = 0;
    if (ring_full(ring)) {
        rc = ring_pop(ring);
    }
    assert(!ring_full(ring));
    ring->buf[ring->start] = new_handle;
    ring->start = ring_wrap(ring->start + 1, ring->size);
    return rc;
}

inline size_t ring_capacity(const struct coro_ring *ring) {
    return ring->size - 1;
}

inline size_t ring_size(const struct coro_ring *ring) {
    if (ring->start >= ring->end)
        return ring->start - ring->end;
    else
        return ring->size - (ring->end - ring->start);
}
