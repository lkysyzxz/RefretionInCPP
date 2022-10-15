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

void SetY(NestStruct *self, float y);

double GetY(NestStruct *self);

void PrintTestStructX(TestStruct* self);

void MoreParams(short x, short y, int z, NestStruct* self, double dx, float fy, float fz);
