#include "TestClass.h"

REGISTER_CLASS(TestScript, test_script,0)
REGISTER_FIELD(TestScript, test_script, int, m_a)
REGISTER_FIELD(TestScript, test_script, double, m_c)
REGISTER_FIELD(TestScript, test_script, int, m_b)
REGISTER_FIELD(TestScript, test_script, std::string, test_str);
REGISTER_NON_PUBLIC_FIELD(TestScript, test_script, int, m_private);
REGISTER_NON_PUBLIC_FIELD(TestScript, test_script, int, m_protected);

REGISTER_METHOD_WITH_RES(int, TestScript, test_script, PTestFunc, TestFunc, int, int, int)
REGISTER_METHOD_WITHOUT_RES(TestScript, test_script, PSay, Say, std::string, double)
REGISTER_METHOD_WITH_RES(double, TestScript, test_script, PFloatAddIntToDouble, FloatAddIntToDouble, double, int)
REGISTER_NON_PUBLIC_METHOD_WITHOUT_RES(TestScript, test_script, PTestPrivateMethod, TestPrivateMethod)
REGISTER_NON_PUBLIC_METHOD_WITHOUT_RES(TestScript, test_script, PTestProtectedMethod, TestProtectedMethod)

REGISTER_STATIC_METHOD_WITHOUT_RES(TestScript, test_script, PTestStaticMethod, TestStaticMethod)
REGISTER_NON_PUBLIC_STATIC_METHOD_WITHOUT_RES(TestScript, test_script, PTestProtectedStaticMethod, TestProtectedStaticMethod)
REGISTER_NON_PUBLIC_STATIC_METHOD_WITHOUT_RES(TestScript, test_script, PTestPrivateStaticMethod, TestPrivateStaticMethod,int,int,int)