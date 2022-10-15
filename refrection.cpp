#include "refrection.h"

template<>
float DecodeArgReverse<float>(va_list& ap) {
	ap -= _INTSIZEOF(double);
	float res = (float)(*((double*)ap));
	return res;
}

template<>
short DecodeArgReverse<short>(va_list& ap) {
	ap -= _INTSIZEOF(int);
	short res = (short)(*((int*)ap));
	return res;
}

template<>
int CaculateSum<float>(int& sum) {
	sum += _INTSIZEOF(double);
	return sum;
}

template<>
int CaculateSum<short>(int& sum) {
	sum += _INTSIZEOF(int);
	return sum;
}


void EmptyFunc(...) {

}

GlobalRefrector& GlobalRefrector::GetRefrector() {
	static GlobalRefrector instance;
	return instance;
}