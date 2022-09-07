#include <iostream>

#include "refrection.h"

using namespace std;

void Debug(int x, ...) {
	va_list ap;
	va_start(ap, x);
	float y = va_arg(ap, float);
	va_end(ap);
}


int main() {
	PType type = GlobalRefrector::GetRefrector().class_map["test_script"];
	void* pins = type->CreateInstance();
	PFieldInfo fieldInfo = type->GetFieldInfo("m_a");
	PMethodInfo methodInfo = type->GetMethodInfo("TestFunc");
	PMethodInfo sayMethod = type->GetMethodInfo("Say");
	PMethodInfo PFloatAddIntToDouble = type->GetMethodInfo("FloatAddIntToDouble");
	int x = methodInfo->InvokeWithRes<int, int, int, int>(pins,1, 2, 3);
	sayMethod->InvokeWithoutRes(pins, std::string("Hello"),3.5f);
	double res = PFloatAddIntToDouble->InvokeWithRes<double,float,int>(pins, 0.5f, 2);
	return 0;
}


