#include "yjdef.h"

__attribute__((constructor)) void _init(void) { DEBUG_PRINT("_init"); }

__attribute__((destructor)) void _fini(void) { DEBUG_PRINT("_fini"); }