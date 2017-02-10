#include "../libdill.h"

#include <assert.h>

void test_ring_new() {
    struct coro_ring *ring = ring_new(0, NULL);
    assert(ring != NULL);
    assert(ring_capacity(ring) == 0);
    ring_delete(&ring);

    ring = ring_new(1024, NULL);
    assert(ring != NULL);
    assert(ring_capacity(ring) == 1024);
    ring_delete(&ring);
}

void test_ring_capacity() {
    struct coro_ring *ring = ring_new(0, NULL);
    assert(ring != NULL);
    assert(ring_capacity(ring) == 0);
    ring_delete(&ring);

    ring = ring_new(1, NULL);
    assert(ring != NULL);
    assert(ring_capacity(ring) == 1);
    ring_delete(&ring);

    ring = ring_new(1024, NULL);
    assert(ring != NULL);
    assert(ring_capacity(ring) == 1024);
    ring_delete(&ring);
}

void test_ring_full() {
    struct coro_ring *ring = ring_new(1, NULL);
    assert(ring != NULL);
    int rc = ring_push(ring, 8);
    assert(rc == 0);
    assert(ring_full(ring));
    ring_delete(&ring);

    ring = ring_new(8, NULL);
    assert(ring != NULL);
    for (int i = 0; i < 8; ++i) {
        rc = ring_push(ring, i);
        assert(rc == 0);
    }
    assert(ring_full(ring));

    ring_delete(&ring);
}

void test_ring_push_pop_tail() {
    int vals[] = {1,2,3,4,5,6,7,8};
    struct coro_ring *ring = ring_new(8, NULL);
    assert(ring != NULL);
    for (int i = 0; i < sizeof(vals) / sizeof(vals[0]); ++i) {
        int rc = ring_push(ring, vals[i]);
        assert(rc == 0);
    }
    assert(ring_full(ring));
    for (int i = 0; i < sizeof(vals) / sizeof(vals[0]); ++i) {
        int v = ring_tail(ring);
        int *pv = ring_tail_p(ring);
        assert(v == *pv);
        assert(v == vals[i]);
        ring_pop(ring);
    }
    ring_delete(&ring);

    ring = ring_new(0, NULL);
    assert(ring != NULL);
    int rc = ring_push(ring, 1);
    assert(rc == -1);
    rc = ring_pop(ring);
    assert(rc == -1);
    ring_delete(&ring);
}

int main() {
    test_ring_new();
    test_ring_full();
    test_ring_capacity();
    test_ring_push_pop_tail();

}
