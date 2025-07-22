#ifndef PARSER_H
#define PARSER_H

#pragma once
#include "main.h"

int load_accesses(const char *path, Access **accesses, int *count);
int parse_memory_size(const char *size_str);

#endif
