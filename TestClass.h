#pragma once
#include <iostream>
#include <vector>
#include "refrection.h"
using namespace std;

class TestScript {
public:
	SET_AS_REFRECTABLE_CLASS(TestScript)
private:
	DEFINE_PRIVATE_MEMBER(TestScript, m_private)
	int m_private;
protected:
	DEFINE_PROTECTED_MEMBER(TestScript, m_protected)
	int m_protected;
public:
	std::string test_str;
	int m_a;


	TestScript() {
		m_a = 10;
		m_c = 5.0f;
		m_private = 3;
		m_protected = 6;
		test_str = "tes";
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

