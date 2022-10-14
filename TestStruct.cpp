#include "TestStruct.h"

REGISTER_STRUCT(TestStruct);

REGISTER_STRUCT(NestStruct);

REGISTER_FUNCTION_WITHOUT_RES(PInitializeInitializeNestStructTS, InitializeNestStructTS, NestStruct*, int);
