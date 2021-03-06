#pragma once

#include <signal.h>
#include <stdalign.h>

#include "qmpi.h"

_Static_assert(sizeof(sig_atomic_t) == sizeof(int),
               "sig_atomic_t has to be int for MPI to work!");
_Static_assert(alignof(sig_atomic_t) == alignof(int),
               "sig_atomic_t has to be int for MPI to work!");

sig_atomic_t _Atomic counters[NUM_MPI_FUNCS] = {0};
