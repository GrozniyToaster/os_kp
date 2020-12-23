#pragma once

#include <assert.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <zmq.h>
#include "structs.h"

void interface(void* information);

void router_module(void* information);

