#include "TestStruct.h"


REGISTER_STRUCT(TestStruct);
REGISTER_FIELD(TestStruct, TestStruct, int, x);

REGISTER_STRUCT(NestStruct);
REGISTER_FIELD(NestStruct, NestStruct, TestStruct*, _ts);

REGISTER_FUNCTION_WITHOUT_RES(PInitializeInitializeNestStructTS, InitializeNestStructTS, NestStruct*, int)
void InitializeNestStructTS(NestStruct* self, int x) {
	self->_ts = new TestStruct();
	self->_ts->x = x;
}

REGISTER_FUNCTION_WITHOUT_RES(PPrintTestStructX, PrintTestStructX, TestStruct*)
void PrintTestStructX(TestStruct* self) {
	printf("%d", self->x);
}

