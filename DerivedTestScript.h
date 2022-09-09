#pragma once
#include "refrection.h"
#include "TestClass.h"

class DerivedTestScript :public TestScript {
public:
	SET_AS_REFRECTABLE_CLASS(DerivedTestScript)
};