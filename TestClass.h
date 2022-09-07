#pragma once
#include <iostream>
#include <vector>
#include "refrection.h"
using namespace std;

class TestScript {
public:
	SET_AS_REFRECTABLE_CLASS(TestScript)
public:
	int m_a;

	TestScript() {
		m_a = 10;
		m_c = 5.0f;
	}

	double m_c;
	int m_b;

	DEFINED_METHOD_POINTER(int, TestScript, PTestFunc,int,int,int)
	int TestFunc(int x,int y,int z) {
		cout << m_c << endl;
		return x+y+z;
	}

	DEFINED_METHOD_POINTER(void, TestScript, PSay, std::string,double)
	void Say(std::string message, double x) {
		cout << message << endl;
		cout << "x:" << x << endl;
	}

	DEFINED_METHOD_POINTER(double, TestScript, PFloatAddIntToDouble, double,int)
	double FloatAddIntToDouble(double x, int y) {
		return double(x + (double)y);
	}
};

