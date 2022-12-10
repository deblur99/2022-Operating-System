// 학번: 32184893
// 학과: 모바일시스템공학과
// 이름: 한현민

#pragma once

#include "defines.h"

#define QUEUE_SUCCESS 0
#define QUEUE_REJECTED 1

void execute(void (*func)(void *param), void *param);
int pool_submit(void (*func)(void *param), void *param);
void *worker(void *param);
void pool_init(void);
void pool_shutdown(void);
