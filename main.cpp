#include <iostream>

#include "refrection.h"
#include "TestClass.h"
#include "TestStruct.h"

using namespace std;

//void Debug(int x, ...) {
//	va_list ap;
//	va_start(ap, x);
//	string y = va_arg(ap, std::string);
//	va_end(ap);
//}

int main() {
	//PStructType ptype = GlobalRefrector::GetRefrector().struct_map["TestStruct"];
	//void* testStruct = ptype->CreateInstance();
	
	PStructType pnestStructType = (PStructType)GlobalRefrector::GetRefrector().class_map["NestStruct"];
	void* testNestStruct = pnestStructType->CreateInstance();
	
	//PFunctionInfo func = GlobalRefrector::GetRefrector().function_map["InitializeNestStructTS"];
	//func->InvokeWithoutRes<NestStruct*, int>((NestStruct*)testNestStruct, 10);

	//PFieldInfo ptestStructFieldInfo = pnestStructType->GetFieldInfo("_ts");
	//void* testStruct = ptestStructFieldInfo->GetPointerValue(testNestStruct);

	//PFunctionInfo pprintTestStructXFunctionInfo = GlobalRefrector::GetRefrector().function_map["PrintTestStructX"];
	//pprintTestStructXFunctionInfo->InvokeWithoutRes<TestStruct*>((TestStruct*)testStruct);

	PFunctionInfo sy = GlobalRefrector::GetRefrector().function_map["SetY"];
	sy->InvokeWithoutRes<void*, double>(testNestStruct, 20.0f);

	PFunctionInfo func = GlobalRefrector::GetRefrector().function_map["InitializeNestStructTS"];
	func->InvokeWithoutRes<void*, int>(testNestStruct, 10);

	PFieldInfo ptestStructFieldInfo = pnestStructType->GetFieldInfo("_ts");
	PFieldInfo py = pnestStructType->GetFieldInfo("y");
	
	//void* testStruct = ptestStructFieldInfo->GetPointerValue(testNestStruct);

	PFunctionInfo pprintTestStructXFunctionInfo = GlobalRefrector::GetRefrector().function_map["PrintTestStructX"];
	//pprintTestStructXFunctionInfo->InvokeWithoutRes<void*>(testStruct);

	PFunctionInfo pgetY = GlobalRefrector::GetRefrector().function_map["GetY"];

	TestStruct* ts = ptestStructFieldInfo->GetValue<TestStruct*>(testNestStruct);
	pprintTestStructXFunctionInfo->InvokeWithoutRes<TestStruct*>(ts);

	//double y = py->GetValue<double>(testNestStruct);

	double y = pgetY->InvokeWithRes<double, void*>(testNestStruct);



	return 0;
}


