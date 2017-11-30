#pragma once
#include <stdint.h>
#define IRUSTTYPE(n) typedef int ## n ## _t i ## n
#define URUSTTYPE(n) typedef uint ## n ## _t u ## n
IRUSTTYPE(8);
IRUSTTYPE(16);
IRUSTTYPE(32);
IRUSTTYPE(64);
URUSTTYPE(8);
URUSTTYPE(16);
URUSTTYPE(32);
URUSTTYPE(64);
typedef size_t usize;

