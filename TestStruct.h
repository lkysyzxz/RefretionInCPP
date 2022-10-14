#pragma once
#include "refrection.h"

struct TestStruct {
	int x;
};

struct NestStruct {
	TestStruct *_ts;
	float y;
};


void InitializeNestStructTS(NestStruct *self, int x);

void PrintTestStructX(TestStruct* self);
