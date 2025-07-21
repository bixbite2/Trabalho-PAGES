#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#pragma once
#include "main.h"

int optimal(Access *accesses, int count, int frame_count);
int working_set(Access *accesses, int count, int frame_count, int tau);
#endif
