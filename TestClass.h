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

	REFRECT_PRIVATE_METHOD(void, TestScript, TestPrivateMethod,PTestPrivateMethod)
	void TestPrivateMethod() {
		std::cout << "Private Method" << std::endl;
	}

	REFRECT_PRIVATE_STATIC_METHOD_POINTER(void, TestScript, TestPrivateStaticMethod, PTestPrivateStaticMethod,int,int,int)
	static void TestPrivateStaticMethod(int x,int y,int z) {
		cout << "TestPrivateStaticMethod" << endl;
	}

protected:
	DEFINE_PROTECTED_MEMBER(TestScript, m_protected)
	int m_protected;

	REFRECT_PROTECTED_METHOD(void, TestScript, TestProtectedMethod,PTestProtectedMethod)
	void TestProtectedMethod() {
		std::cout << "Protected Method" << std::endl;
	}

	REFRECT_PROTECTED_STATIC_METHOD_POINTER(void, TestScript, TestProtectedStaticMethod, PTestProtectedStaticMethod)
	static void TestProtectedStaticMethod() {
		cout << "TestProtectedStaticMethod" << endl;
	}

public:

	DEFINED_STATIC_METHOD_POINTER(void, TestScript, TestStaticMethod,PTestStaticMethod)
	static void TestStaticMethod() {
		cout << "TestStaticMethod" << endl;
	}

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
	virtual int TestFunc(int x,int y,int z) {
		cout << "Call Teset Func in Base Instance" << endl;
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

