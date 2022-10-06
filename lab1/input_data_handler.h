#pragma once

#include "defines.h"

INPUT_DATA *parseInputData(int argc, char *argv[]);

bool freeInputData(INPUT_DATA *input_data);

int getSumFromInputData(INPUT_DATA *input_data);