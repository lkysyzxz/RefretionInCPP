#include <iostream>

#include "refrection.h"

using namespace std;


int main() {
	PAbstractType ptestScript = (PAbstractType)GlobalRefrector::GetRefrector().class_map["test_script"];
	PMethodInfo pTestPrivateMethod = ptestScript->GetMethodInfo("TestPrivateMethod");
	PMethodInfo pTestPrivateStaticMethod = ptestScript->GetMethodInfo("TestPrivateStaticMethod");
	PMethodInfo pTestProtectedMethod = ptestScript->GetMethodInfo("TestProtectedMethod");
	PMethodInfo pTestProtectedStaticMethod = ptestScript->GetMethodInfo("TestProtectedStaticMethod");
	PMethodInfo pTestStaticMethod = ptestScript->GetMethodInfo("TestStaticMethod");
	PMethodInfo pTestFunc = ptestScript->GetMethodInfo("TestFunc");
	PMethodInfo pSay = ptestScript->GetMethodInfo("Say");
	PMethodInfo pFloatAddIntToDouble = ptestScript->GetMethodInfo("FloatAddIntToDouble");
	PMethodInfo pPureVirtualMethod = ptestScript->GetMethodInfo("PureVirtualMethod");

	void* pins = ptestScript->CreateInstance("DerivedTestScript");
	pTestPrivateMethod->InvokeWithoutRes(pins);
	pTestPrivateStaticMethod->InvokeStaticWithoutRes();
	pTestProtectedMethod->InvokeWithoutRes(pins);
	pTestPrivateStaticMethod->InvokeStaticWithoutRes();
	pTestStaticMethod->InvokeStaticWithoutRes();
	int res = pTestFunc->InvokeWithRes<int, int, int, int>(pins, 1, 2, 3);
	pSay->InvokeWithoutRes < std::string,double>(pins,"Hello", 0.5f);
	double resDouble = pFloatAddIntToDouble->InvokeWithRes<double, double, int>(pins, 0.5f, 1);
	pPureVirtualMethod->InvokeWithoutRes(pins);
	
	PStructType pNestStructType = (PStructType)GlobalRefrector::GetRefrector().class_map["NestStruct"];
	void* pNestStruct = pNestStructType->CreateInstance();
	PFunctionInfo pInitializeNestStructTS = GlobalRefrector::GetRefrector().function_map["InitializeNestStructTS"];
	PFunctionInfo pSetY = GlobalRefrector::GetRefrector().function_map["SetY"];
	PFunctionInfo pGetY = GlobalRefrector::GetRefrector().function_map["GetY"];
	PFunctionInfo pPrintTestStructX = GlobalRefrector::GetRefrector().function_map["PrintTestStructX"];
	PFunctionInfo pMoreParams = GlobalRefrector::GetRefrector().function_map["MoreParams"];
	
	pInitializeNestStructTS->InvokeWithoutRes(pNestStruct, 2);
	pSetY->InvokeWithoutRes(pNestStruct, 0.25f);
	double y = pGetY->InvokeWithRes<double>(pNestStruct);
	PFieldInfo testStructField = pNestStructType->GetFieldInfo("_ts");
	void* pTestStruct = testStructField->GetValue<void*>(pNestStruct);
	pPrintTestStructX->InvokeWithoutRes(pTestStruct);
	pMoreParams->InvokeWithoutRes(1, 2, 3, pNestStruct, 0.8, 244.56f, 3442.43f);
	return 0;
}


