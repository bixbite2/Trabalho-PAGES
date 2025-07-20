#ifndef MAIN_H
#define MAIN_H

#pragma once
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_SIZE 4096
#define MAX_PAGE_NAME 16

typedef struct {
    char type;
    char page[MAX_PAGE_NAME];
} Access;

int main(int argc, char *argv[]);

#endif
