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

#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new matrix
GRAPHS_EXPORT matrix_t *
    matrix_new (unsigned int x, unsigned int y, size_t element_size);

//  set element
GRAPHS_EXPORT void
    matrix_set (matrix_t *self, unsigned int x, unsigned int y, void *element);

//  set int element
GRAPHS_EXPORT void
    matrix_set_int (matrix_t *self, unsigned int x, unsigned int y, int element);

//  get element
GRAPHS_EXPORT void
    matrix (matrix_t *self, unsigned int x, unsigned int y, void *dest);

GRAPHS_EXPORT void *
    matrix_get_ptr (matrix_t *self, unsigned int x, unsigned int y);

//  get int element
GRAPHS_EXPORT int
    matrix_as_int (matrix_t *self, unsigned int x, unsigned int y);

//  Get matrix width
GRAPHS_EXPORT int
    matrix_x (matrix_t *self);

//  Get matrix height
GRAPHS_EXPORT int
    matrix_y (matrix_t *self);

//  Convert matrix to chunk
GRAPHS_EXPORT zchunk_t *
    matrix_as_chunk (matrix_t *self);

//  Convert chunk to matrix
GRAPHS_EXPORT matrix_t *
    matrix_from_chunk (zchunk_t **chunk_ptr);

//  Print matrix of integers
GRAPHS_EXPORT void
    matrix_print_int (matrix_t *self);

#define vector_new(X,E) matrix_new(X, 1, E)
#define vector_destroy(S) matrix_destroy(S)
#define vector_as_int(S,X) matrix_as_int(S,X,0)
#define vector_set_int(S,X,V) matrix_set_int(S,X,0,V)

//  Destroy the matrix
GRAPHS_EXPORT void
    matrix_destroy (matrix_t **self_p);

//  Self test of this class
GRAPHS_EXPORT void
    matrix_test (bool verbose);

//  @end

#ifdef __cplusplus
}
#endif

#endif
