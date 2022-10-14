#pragma once
#include "refrection.h"

struct TestStruct {
	int x;
};

SET_AS_REFRECTABLE_STRUCT(TestStruct)

struct NestStruct {
	TestStruct *_ts;
	float y;
};


SET_AS_REFRECTABLE_STRUCT(NestStruct)


DEFINE_FUNCTION_POINTER(void, PInitializeInitializeNestStructTS, NestStruct*, int)
void InitializeNestStructTS(NestStruct *self, int x) {
	self->_ts = new TestStruct();
	self->_ts->x = x;
}
