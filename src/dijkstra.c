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

/*
@header
    dijkstra - Dijkstra method
@discuss
@end
*/

#include "graphs_classes.h"


//  Structure of our actor

struct _dijkstra_t {
    zsock_t *pipe;              //  Actor command pipe
    zpoller_t *poller;          //  Socket poller
    bool terminated;            //  Did caller ask us to quit?
    bool verbose;               //  Verbose logging enabled?

    matrix_t *distances;
    int from;                   // search for path from node
    int to;                     // search for path to node
};


//  --------------------------------------------------------------------------
//  Create a new dijkstra instance

static dijkstra_t *
dijkstra_new (zsock_t *pipe, void *args)
{
    dijkstra_t *self = (dijkstra_t *) zmalloc (sizeof (dijkstra_t));
    assert (self);

    self->pipe = pipe;
    self->terminated = false;
    self->poller = zpoller_new (self->pipe, NULL);
    self->distances = (matrix_t *) args;
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the dijkstra instance

static void
dijkstra_destroy (dijkstra_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        dijkstra_t *self = *self_p;
        //  Free object itself
        zpoller_destroy (&self->poller);
        free (self);
        *self_p = NULL;
    }
}

static void add_to_queue (matrix_t *queue, int *len, int value)
{
    for (int i = 0; i < *len; i++) {
        if (vector_as_int (queue, i) == value) return;
    }
    *len += 1;
    vector_set_int (queue, *len, value);
}

static int pop_from_queue (matrix_t *queue, int *len, int idx)
{
    int result = matrix_as_int (queue, idx, 0);
    if (idx < matrix_x (queue) - 1) {
        for (int i = idx; i < *len; ++i) {
            vector_set_int (queue, i, vector_as_int (queue, i + 1));
        }
    }
    *len -= 1;
    return result;
}

matrix_t *
dijkstra_find_path (dijkstra_t *self, int from)
{
    int number_of_nodes = matrix_x (self->distances);
    matrix_t *node_visited = vector_new (number_of_nodes, sizeof (int));
    matrix_t *queue = vector_new (number_of_nodes, sizeof (int));
    int queue_len = 0;
    matrix_t *result = vector_new (number_of_nodes, sizeof (dnode_t));
    // matrix_t *result = dijkstra_init_result (self, from);

    for (int i = 0; i < number_of_nodes; ++i) {
        dnode_t n = {
            .parent = -1,
            .distance = (i == from ? 0 : INT_MAX)
        };
        vector_set (result, i, &n);
    }
    add_to_queue (queue, &queue_len, from);

    while (queue_len) {
        int min_dist = INT_MAX;
        int node = -1;
        int node_queue_idx = -1;
        // find nearest node in queue
        for(int i = 0; i < queue_len; i++) {
            int qnode = vector_as_int (queue, i);
            dnode_t *curr_dist = (dnode_t *) vector_get_ptr (result, qnode);
            zsys_debug ("checking node %i - %i", qnode, curr_dist->distance);
            if (curr_dist->distance  < min_dist) {
                min_dist = curr_dist->distance;
                node = qnode;
                node_queue_idx = i;
            }
        }
        if (node != -1) {
            // remove selected node from queue
            pop_from_queue (queue, &queue_len, node_queue_idx);
            vector_set_int (node_visited, node, 1);
        }
        zsys_debug ("node %i", node);
        for (int i = 0; i < number_of_nodes; i++) {
            if (!vector_as_int (node_visited, i)) {

            }
        }
        zsys_debug ("queue_len %i", queue_len);
    }
    vector_destroy (&node_visited);
    vector_destroy (&queue);

    return result;
}

//  Start this actor. Return a value greater or equal to zero if initialization
//  was successful. Otherwise -1.

static int
dijkstra_start (dijkstra_t *self)
{
    assert (self);

    //  TODO: Add startup actions

    return 0;
}


//  Stop this actor. Return a value greater or equal to zero if stopping
//  was successful. Otherwise -1.

static int
dijkstra_stop (dijkstra_t *self)
{
    assert (self);

    //  TODO: Add shutdown actions

    return 0;
}


//  Here we handle incoming message from the node

static void
dijkstra_recv_api (dijkstra_t *self)
{
    //  Get the whole message of the pipe in one go
    zmsg_t *request = zmsg_recv (self->pipe);
    if (!request)
       return;        //  Interrupted

    char *command = zmsg_popstr (request);
    if (streq (command, "START"))
        dijkstra_start (self);
    else
    if (streq (command, "STOP"))
        dijkstra_stop (self);
    else
    if (streq (command, "VERBOSE"))
        self->verbose = true;
    else
    if (streq (command, "TASK")) {
        char *from = zmsg_popstr (request);
        self->from = atoi (from);
        zstr_free (&from);
        matrix_t *result = dijkstra_find_path (self, self->from);
        zchunk_t *chunk = matrix_as_chunk (result);
        zframe_t *frame = zchunk_pack (chunk);
        zstr_sendm (self->pipe, "DONE");
        zframe_send (&frame, self->pipe, 0);
        zchunk_destroy (&chunk);
        matrix_destroy (&result);
    } else
    if (streq (command, "$TERM"))
        //  The $TERM command is send by zactor_destroy() method
        self->terminated = true;
    else {
        zsys_error ("invalid command '%s'", command);
        assert (false);
    }
    zstr_free (&command);
    zmsg_destroy (&request);
}


//  --------------------------------------------------------------------------
//  This is the actor which runs in its own thread.

void
dijkstra_actor (zsock_t *pipe, void *args)
{
    dijkstra_t * self = dijkstra_new (pipe, args);
    if (!self)
        return;          //  Interrupted

    //  Signal actor successfully initiated
    zsock_signal (self->pipe, 0);

    while (!self->terminated) {
        zsock_t *which = (zsock_t *) zpoller_wait (self->poller, 0);
        if (which == self->pipe)
            dijkstra_recv_api (self);
        //  Add other sockets when you need them.
    }
    dijkstra_destroy (&self);
}

//  --------------------------------------------------------------------------
//  Self test of this actor.

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
dijkstra_test (bool verbose)
{
    printf (" * dijkstra: ");
    //  @selftest
    //  Simple create/destroy test
    {
        zactor_t *dijkstra = zactor_new (dijkstra_actor, NULL);
        assert (dijkstra);
        zactor_destroy (&dijkstra);
    }
    //  Search node test
    {
        matrix_t *d = matrix_new (4, 4, sizeof (int));
        for (int x = 1; x < 4; x++) {
            for (int y = 0; y < x; y++) {
                matrix_set_int (d, x, y, x);
                matrix_set_int (d, y, x, x);
            }
        }
        matrix_print_int (d);
        zactor_t *dijkstra = zactor_new (dijkstra_actor, d);
        assert (dijkstra);
        zstr_sendx (dijkstra, "TASK", "0", NULL);
        zmsg_t *msg = zmsg_recv (dijkstra);
        char *str = zmsg_popstr (msg);
        assert (streq (str, "DONE"));
        zstr_free (&str);
        zframe_t *frame = zmsg_pop (msg);
        zchunk_t *chunk = zchunk_unpack (frame);
        zframe_destroy (&frame);
        matrix_t *result = matrix_from_chunk (&chunk);
        matrix_print_int (result);
        matrix_destroy (&result);
        zmsg_destroy (&msg);
        zactor_destroy (&dijkstra);
        matrix_destroy (&d);
    }
    //  @end

    printf ("OK\n");
}
