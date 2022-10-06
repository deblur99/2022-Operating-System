// 학번: 32184893
// 학과: 모바일시스템공학과
// 이름: 한현민

#pragma once

#include "defines.h"

INPUT_DATA *parseInputData(int argc, char *argv[]);

bool freeInputData(INPUT_DATA *input_data);

int getSumFromInputData(INPUT_DATA *input_data);