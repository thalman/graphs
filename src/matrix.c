/*  =========================================================================
    matrix - Matrix

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    matrix - Matrix
@discuss
@end
*/

#include "graphs_classes.h"

//  Structure of our class

struct _matrix_t {
    unsigned int x;
    unsigned int y;
    pthread_mutex_t mutex;
    size_t element_size;
    uint8_t *elements;
};


//  --------------------------------------------------------------------------
//  Create a new matrix

matrix_t *
matrix_new (unsigned int x, unsigned int y, size_t element_size)
{
    if (!x || !y || !element_size) return NULL;

    matrix_t *self = (matrix_t *) zmalloc (sizeof (matrix_t));
    assert (self);
    self->x = x;
    self->y = y;
    self->element_size = element_size;
    self->elements = (uint8_t *) zmalloc (self->x * self->y * self->element_size);
    assert (self->elements);
    int res = pthread_mutex_init (&self->mutex, NULL);
    assert (res == 0);
    return self;
}


//  --------------------------------------------------------------------------
//  set element

void
matrix_set (matrix_t *self, unsigned int x, unsigned int y, void *element)
{
    if (!self || x >= self->x || y >= self->y || !element) return;
    pthread_mutex_lock (&self->mutex);
    uint8_t *dest = &(self->elements [(self->x * y + x) * self->element_size]);
    memcpy (dest, element, self->element_size);
    pthread_mutex_unlock (&self->mutex);
}

//  --------------------------------------------------------------------------
//  set int element
void
matrix_set_int (matrix_t *self, unsigned int x, unsigned int y, int element)
{
    if (self->element_size != sizeof (int)) return;
    matrix_set (self, x, y, &element);
}

//  --------------------------------------------------------------------------
//  get element

void
matrix (matrix_t *self, unsigned int x, unsigned int y, void *dest)
{
    if (!self || x >= self->x || y >= self->y || !dest) return;
    pthread_mutex_lock (&self->mutex);
    void *element = matrix_get_ptr (self, x, y);
    if (element) {
        memcpy (dest, element, self->element_size);
    }
    pthread_mutex_unlock (&self->mutex);
}

//  --------------------------------------------------------------------------
//  get element

void *
matrix_get_ptr (matrix_t *self, unsigned int x, unsigned int y)
{
    if (!self || x >= self->x || y >= self->y) return NULL;
    uint8_t *element = &(self->elements [(self->x * y + x) * self->element_size]);
    return (void *)element;
}


//  --------------------------------------------------------------------------
//  get int element
int
matrix_as_int (matrix_t *self, unsigned int x, unsigned int y)
{
    if (self->element_size != sizeof (int)) return 0;
    int result;
    matrix (self, x, y, &result);
    return result;
}

//  --------------------------------------------------------------------------
//  Get matrix width
int
matrix_x (matrix_t *self) {
    if (!self) return 0;
    return self->x;
}

//  --------------------------------------------------------------------------
//  Get matrix height
int
matrix_y (matrix_t *self) {
    if (!self) return 0;
    return self->y;
}

//  --------------------------------------------------------------------------
//  Destroy the matrix

void
matrix_destroy (matrix_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        matrix_t *self = *self_p;
        if (self->elements) free (self->elements);
        pthread_mutex_destroy (&self->mutex);
        free (self);
        *self_p = NULL;
    }
}

zchunk_t *
matrix_as_chunk (matrix_t *self)
{
    zchunk_t *chunk = zchunk_new (&(self->x), sizeof (unsigned int));
    zchunk_extend (chunk, &(self->y), sizeof (unsigned int));
    zchunk_extend (chunk, &(self->element_size), sizeof (size_t));
    zchunk_extend (chunk, self->elements, self->x * self->y * self->element_size);
    return chunk;
}

matrix_t *
matrix_from_chunk (zchunk_t **chunk_ptr)
{
    if (! chunk_ptr || ! *chunk_ptr) return NULL;

    zchunk_t *chunk = *chunk_ptr;
    unsigned int x, y;
    size_t element_size;
    byte *data = zchunk_data (chunk);
    memcpy (&x, &data[0], sizeof (unsigned int));
    memcpy (&y, &data[sizeof (unsigned int)], sizeof (unsigned int));
    memcpy (&element_size, &data[2*sizeof (unsigned int)], sizeof (size_t));

    matrix_t *result = matrix_new (x, y, element_size);
    if (result) {
        memcpy (result->elements, &data[2*sizeof (unsigned int) + sizeof (size_t)], x * y * element_size);
    }
    zchunk_destroy (chunk_ptr);
    return result;
}

void matrix_print_int (matrix_t *self)
{
    if (!self || self->element_size != sizeof (int)) return;

    printf ("\n---\n");
    for (int y = 0; y < self->y; y++) {
        for (int x = 0; x < self->x; x++) {
            printf ("%i\t", matrix_as_int (self, x, y));
        }
        printf ("\n");
    }
    printf ("---\n");
}

//  --------------------------------------------------------------------------
//  Self test of this class

// If your selftest reads SCMed fixture data, please keep it in
// src/selftest-ro; if your test creates filesystem objects, please
// do so under src/selftest-rw.
// The following pattern is suggested for C selftest code:
//    char *filename = NULL;
//    filename = zsys_sprintf ("%s/%s", SELFTEST_DIR_RO, "mytemplate.file");
//    assert (filename);
//    ... use the "filename" for I/O ...
//    zstr_free (&filename);
// This way the same "filename" variable can be reused for many subtests.
#define SELFTEST_DIR_RO "src/selftest-ro"
#define SELFTEST_DIR_RW "src/selftest-rw"

void
matrix_test (bool verbose)
{
    printf (" * matrix: ");

    //  @selftest
    //  Simple create/destroy test
    matrix_t *self = matrix_new (5, 3, sizeof(int));
    assert (self);
    assert (matrix_x (self) == 5);
    assert (matrix_y (self) == 3);
    int x = 5;
    matrix_set (self, 0, 1, &x);
    matrix (self, 0, 2, &x);
    assert (x == 0);
    matrix (self, 0, 1, &x);
    assert (x == 5);

    matrix_set_int (self, 0, 0, -3);
    assert (matrix_as_int (self, 0, 0) == -3);

    zchunk_t *chunk = matrix_as_chunk (self);
    matrix_t *copy = matrix_from_chunk (&chunk);
    for (int y = 0; y < matrix_y (self); y++) {
        for (int x = 0; x < matrix_x (self); x++) {
            assert (matrix_as_int (self, x, y) == matrix_as_int (copy, x, y));
        }
    }
    //matrix_print_int (self);
    //matrix_print_int (copy);
    matrix_destroy (&copy);
    matrix_destroy (&self);
    //  @end
    printf ("OK\n");
}
