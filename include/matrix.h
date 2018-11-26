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
    matrix_new (void);

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
