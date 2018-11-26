/*  =========================================================================
    graphs - generated layer of public API

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.

################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
    =========================================================================
*/

#ifndef GRAPHS_LIBRARY_H_INCLUDED
#define GRAPHS_LIBRARY_H_INCLUDED

//  Set up environment for the application

//  External dependencies
#include <czmq.h>

//  GRAPHS version macros for compile-time API detection
#define GRAPHS_VERSION_MAJOR 1
#define GRAPHS_VERSION_MINOR 0
#define GRAPHS_VERSION_PATCH 0

#define GRAPHS_MAKE_VERSION(major, minor, patch) \
    ((major) * 10000 + (minor) * 100 + (patch))
#define GRAPHS_VERSION \
    GRAPHS_MAKE_VERSION(GRAPHS_VERSION_MAJOR, GRAPHS_VERSION_MINOR, GRAPHS_VERSION_PATCH)

#if defined (__WINDOWS__)
#   if defined GRAPHS_STATIC
#       define GRAPHS_EXPORT
#   elif defined GRAPHS_INTERNAL_BUILD
#       if defined DLL_EXPORT
#           define GRAPHS_EXPORT __declspec(dllexport)
#       else
#           define GRAPHS_EXPORT
#       endif
#   elif defined GRAPHS_EXPORTS
#       define GRAPHS_EXPORT __declspec(dllexport)
#   else
#       define GRAPHS_EXPORT __declspec(dllimport)
#   endif
#   define GRAPHS_PRIVATE
#elif defined (__CYGWIN__)
#   define GRAPHS_EXPORT
#   define GRAPHS_PRIVATE
#else
#   define GRAPHS_EXPORT
#   if (defined __GNUC__ && __GNUC__ >= 4) || defined __INTEL_COMPILER
#       define GRAPHS_PRIVATE __attribute__ ((visibility ("hidden")))
#   else
#       define GRAPHS_PRIVATE
#   endif
#endif

//  Project has no stable classes, so we build the draft API
#undef  GRAPHS_BUILD_DRAFT_API
#define GRAPHS_BUILD_DRAFT_API

//  Opaque class structures to allow forward references
//  These classes are stable or legacy and built in all releases
//  Draft classes are by default not built in stable releases
#ifdef GRAPHS_BUILD_DRAFT_API
typedef struct _matrix_t matrix_t;
#define MATRIX_T_DEFINED
#endif // GRAPHS_BUILD_DRAFT_API


//  Public classes, each with its own header file
#ifdef GRAPHS_BUILD_DRAFT_API
#include "matrix.h"
#endif // GRAPHS_BUILD_DRAFT_API

#ifdef GRAPHS_BUILD_DRAFT_API

#ifdef __cplusplus
extern "C" {
#endif

//  Self test for private classes
GRAPHS_EXPORT void
    graphs_private_selftest (bool verbose, const char *subtest);

#ifdef __cplusplus
}
#endif
#endif // GRAPHS_BUILD_DRAFT_API

#endif
/*
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
*/
