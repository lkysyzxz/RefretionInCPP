#pragma once
#include "refrection.h"
#include "TestClass.h"

class DerivedTestScript :public TestScript {
public:
	SET_AS_REFRECTABLE_CLASS(DerivedTestScript)

	virtual int TestFunc(int x, int y, int z) {
		cout << "Call Test Func in Derived Instance" << endl;
		return x + y + z;
	}

	virtual void PureVirtualMethod() {
		std::cout << "Call pure virtual method" << std::endl;
	}
};