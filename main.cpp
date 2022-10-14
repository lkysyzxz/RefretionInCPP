#include <iostream>

#include "refrection.h"
#include "TestClass.h"

using namespace std;

//void Debug(int x, ...) {
//	va_list ap;
//	va_start(ap, x);
//	string y = va_arg(ap, std::string);
//	va_end(ap);
//}

int main() {
	PStructType ptype = GlobalRefrector::GetRefrector().struct_map["TestStruct"];
	void* testStruct = ptype->CreateInstance();
	
	PStructType pnestStructType = GlobalRefrector::GetRefrector().struct_map["NestStruct"];
	void* testNestStruct = pnestStructType->CreateInstance();
	
	PFunctionInfo func = GlobalRefrector::GetRefrector().function_map["InitializeNestStructTS"];
	func->InvokeWithoutRes<void*, int>(testNestStruct, 10);

	return 0;
}


