#include <iostream>

#include "refrection.h"

using namespace std;

void Debug(int x, ...) {
	va_list ap;
	va_start(ap, x);
	float y = va_arg(ap, float);
	va_end(ap);
}

class TestOffset {
private:
	int m_a;
protected:
	int m_b;
private:
	static int m_c;

};

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

	
	PMethodInfo methodInfo = type->GetMethodInfo("TestFunc");
	PMethodInfo sayMethod = type->GetMethodInfo("Say");
	PMethodInfo PFloatAddIntToDouble = type->GetMethodInfo("FloatAddIntToDouble");
	int x = methodInfo->InvokeWithRes<int, int, int, int>(pins,1, 2, 3);
	sayMethod->InvokeWithoutRes(pins, std::string("Hello"),3.5f);
	double res = PFloatAddIntToDouble->InvokeWithRes<double,double,int>(pins, 0.5, 2);
	return 0;
}


