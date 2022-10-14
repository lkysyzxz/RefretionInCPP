#include "TestStruct.h"


REGISTER_STRUCT(TestStruct);
REGISTER_FIELD(TestStruct, TestStruct, int, x);

REGISTER_STRUCT(NestStruct);
REGISTER_FIELD(NestStruct, NestStruct, TestStruct*, _ts);
REGISTER_FIELD(NestStruct, NestStruct, double, y);

REGISTER_FUNCTION_WITHOUT_RES(PInitializeInitializeNestStructTS, InitializeNestStructTS, NestStruct*, int)
void InitializeNestStructTS(NestStruct* self, int x) {
	self->_ts = new TestStruct();
	self->_ts->x = x;
}

REGISTER_FUNCTION_WITHOUT_RES(PSetY, SetY, NestStruct*, double);
void SetY(NestStruct *self, double y) {
	self->y = y;
}

REGISTER_FUNCTION_WITH_RES(double, PGetY, GetY, NestStruct*);
double GetY(NestStruct *self) {
	return self->y;
}

REGISTER_FUNCTION_WITHOUT_RES(PPrintTestStructX, PrintTestStructX, TestStruct*)
void PrintTestStructX(TestStruct* self) {
	printf("%d", self->x);
}

