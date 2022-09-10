#include <iostream>

#include "refrection.h"
#include "TestClass.h"

using namespace std;

void Debug(int x, ...) {
	va_list ap;
	va_start(ap, x);
	string y = va_arg(ap, std::string);
	va_end(ap);
}

int main() {
	PBaseType type = GlobalRefrector::GetRefrector().class_map["test_script"];
	void* pins = type->CreateInstance("DerivedTestScript");
	PMethodInfo pureVirtualMethod = type->GetMethodInfo("PureVirtualMethod");
	pureVirtualMethod->InvokeWithoutRes(pins);

	return 0;
}


