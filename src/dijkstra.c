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
        char *to = zmsg_popstr (request);
        self->from = atoi (from);
        self->to = atoi (to);
        zstr_free (&from);
        zstr_free (&to);
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
        zactor_t *dijkstra = zactor_new (dijkstra_actor, d);
        zstr_sendx (dijkstra, "TASK", "0", "3", NULL);
        assert (dijkstra);
        zactor_destroy (&dijkstra);
        matrix_destroy (&d);
    }
    //  @end

    printf ("OK\n");
}
