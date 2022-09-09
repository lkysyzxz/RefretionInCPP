#include <iostream>

#include "refrection.h"

using namespace std;

void Debug(int x, ...) {
	va_list ap;
	va_start(ap, x);
	string y = va_arg(ap, std::string);
	va_end(ap);
}

int main() {


	PType type = GlobalRefrector::GetRefrector().class_map["test_script"];
	void* pins = type->CreateInstance();
	PFieldInfo fieldInfo = type->GetFieldInfo("m_a");
	PFieldInfo privateFieldInfo = type->GetFieldInfo("m_private");
	PFieldInfo protectedFieldInfo = type->GetFieldInfo("m_protected");
	PFieldInfo strFieldInfo = type->GetFieldInfo("test_str");
	int m_private = *(int*)privateFieldInfo->GetValueAddress(pins);
	int m_protected = *(int*)protectedFieldInfo->GetValueAddress(pins);
	std::string str = *(std::string*)strFieldInfo->GetValueAddress(pins);

	PMethodInfo privateMethod = type->GetMethodInfo("TestPrivateMethod");
	PMethodInfo protectedMethod = type->GetMethodInfo("TestProtectedMethod");
	privateMethod->InvokeWithoutRes(pins);
	protectedMethod->InvokeWithoutRes(pins);

	
	PMethodInfo methodInfo = type->GetMethodInfo("TestFunc");
	PMethodInfo sayMethod = type->GetMethodInfo("Say");
	PMethodInfo PFloatAddIntToDouble = type->GetMethodInfo("FloatAddIntToDouble");
	int x = methodInfo->InvokeWithRes<int, int, int, int>(pins,1, 2, 3);
	sayMethod->InvokeWithoutRes(pins, std::string("Helloo000oooo"), 3.5);
	double res = PFloatAddIntToDouble->InvokeWithRes<double,double,int>(pins, 0.5, 2);

	PType pDerivedType = GlobalRefrector::GetRefrector().class_map["DerivedTestScript"];
	void* pDerivedIns = pDerivedType->CreateInstance();
	PMethodInfo sayMethodInDerived = pDerivedType->GetMethodInfo("Say");
	sayMethodInDerived->InvokeWithoutRes(sayMethodInDerived, std::string("Hello InDerived"), 3.5);

	PMethodInfo ptestFuncFromDerived = pDerivedType->GetMethodInfo("TestFunc");
	int z = ptestFuncFromDerived->InvokeWithRes<int,int,int,int>(pDerivedIns, 1, 2, 4);

	PFieldInfo fieldFromDerived = pDerivedType->GetFieldInfo("m_a");
	int xx = *(int*)fieldInfo->GetValueAddress(pins);
	int mm = *(int*)fieldFromDerived->GetValueAddress(pDerivedIns);
	return 0;
}


