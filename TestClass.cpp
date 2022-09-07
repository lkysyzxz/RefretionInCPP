#include "TestClass.h"

REGISTER_CLASS(TestScript, test_script)
REGISTER_FIELD(TestScript, test_script, int, m_a)
REGISTER_FIELD(TestScript, test_script, double, m_c)
REGISTER_FIELD(TestScript, test_script, int, m_b)
REGISTER_METHOD_WITH_RES(int, TestScript, test_script, PTestFunc, TestFunc, int, int, int)
REGISTER_METHOD_WITHOUT_RES(TestScript, test_script, PSay, Say, std::string, double)
REGISTER_METHOD_WITH_RES(double, TestScript, test_script, PFloatAddIntToDouble, FloatAddIntToDouble, double,int)