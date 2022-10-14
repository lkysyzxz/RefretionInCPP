#pragma once
#include "refrection.h"

struct TestStruct {
	int x;
};

struct NestStruct {
	TestStruct *_ts;
	double y;
};


void InitializeNestStructTS(NestStruct *self, int x);

void SetY(NestStruct *self, double y);

void PrintTestStructX(TestStruct* self);
