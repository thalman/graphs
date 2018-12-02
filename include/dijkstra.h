/*  =========================================================================
    dijkstra - Dijkstra method

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef DIJKSTRA_H_INCLUDED
#define DIJKSTRA_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create new dijkstra actor instance.
//  @TODO: Describe the purpose of this actor!
//
//      zactor_t *dijkstra = zactor_new (dijkstra, NULL);
//
//  Destroy dijkstra instance.
//
//      zactor_destroy (&dijkstra);
//
//  Enable verbose logging of commands and activity:
//
//      zstr_send (dijkstra, "VERBOSE");
//
//  Start dijkstra actor.
//
//      zstr_sendx (dijkstra, "START", NULL);
//
//  Stop dijkstra actor.
//
//      zstr_sendx (dijkstra, "STOP", NULL);
//
//  This is the dijkstra constructor as a zactor_fn;
GRAPHS_EXPORT void
    dijkstra_actor (zsock_t *pipe, void *args);

//  Self test of this actor
GRAPHS_EXPORT void
    dijkstra_test (bool verbose);

typedef struct _dnode_t {
    int parent;
    int distance;
} dnode_t;

//  @end

#ifdef __cplusplus
}
#endif

#endif
