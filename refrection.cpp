#include "refrection.h"

GlobalRefrector& GlobalRefrector::GetRefrector() {
	static GlobalRefrector instance;
	return instance;
}