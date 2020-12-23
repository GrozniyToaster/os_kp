#pragma once

#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <error.h>
#include <assert.h>
#include <ncurses.h>
#include <pthread.h>
#include <zmq.h>

#include "structs.h"

#define SQ_HEIGHT 7
#define SQ_WIDTH 9
#define BOARD_SIZE 9

#define NO_WIN 2
#define WIN 1
#define CANT_SET 0

void interface(void* information);

void router_module(void* information);

