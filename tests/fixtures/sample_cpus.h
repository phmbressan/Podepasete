#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "../../src/pdp7_cpu.h"

PDP7_cpu create_empty_cpu(void);
PDP7_cpu create_cpu_with_memory(void);