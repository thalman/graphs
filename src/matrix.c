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
    return self;
}


//  --------------------------------------------------------------------------
//  set element

void
matrix_set (matrix_t *self, unsigned int x, unsigned int y, void *element)
{
    if (!self || x >= self->x || y >= self->y || !element) return;
    uint8_t *dest = &(self->elements [(self->x * y + y) * self->element_size]);
    memcpy (dest, element, self->element_size);
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

void *
matrix_get (matrix_t *self, unsigned int x, unsigned int y)
{
    if (!self || x >= self->x || y >= self->y) return NULL;
    uint8_t *dest = &(self->elements [(self->x * y + y) * self->element_size]);
    return dest;
}


//  --------------------------------------------------------------------------
//  get int element
int
matrix_get_int (matrix_t *self, unsigned int x, unsigned int y)
{
    if (self->element_size != sizeof (int)) return 0;
    return *(int *) matrix_get (self, x, y);
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
        free (self);
        *self_p = NULL;
    }
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
    x = *(int *)matrix_get (self, 0, 2);
    assert (x == 0);
    x = *(int *)matrix_get (self, 0, 1);
    assert (x == 5);

    matrix_set_int (self, 0, 0, -3);
    assert (matrix_get_int (self, 0, 0) == -3);

    matrix_destroy (&self);
    //  @end
    printf ("OK\n");
}
