#pragma once

#include <iostream>
#include <string>
#include <map>
#include <functional>
#include <stdarg.h>

typedef void* (*PInstanceGenerator)();

class Type;

typedef  Type* PType;

class FieldInfo;

typedef FieldInfo* PFieldInfo;

class MethodInfo;

typedef MethodInfo* PMethodInfo;

class GlobalRefractor;

class GlobalRefrector {
public:
	static GlobalRefrector& GetRefrector();
public:
	std::map<std::string, PType> class_map;
};

class Type {
private:
	PInstanceGenerator generator;
	std::map<std::string, PFieldInfo> field_map;
	std::map<std::string, PMethodInfo> method_map;
public:
	std::string class_type;
	std::string class_name;

	Type() {

	}

	Type(const std::string& ct, const std::string& cn, PInstanceGenerator pg) {
		class_type = ct;
		class_name = cn;
		generator = pg;
		GlobalRefrector::GetRefrector().class_map.insert(std::map<std::string, Type*>::value_type(class_name, this));
	}

	void AddFieldInfo(std::string field_name, PFieldInfo field_info) {
		if (field_map.find(field_name) == field_map.end()) {
			field_map.insert(std::pair<std::string, PFieldInfo>(field_name, field_info));
		}
	}

	PFieldInfo GetFieldInfo(std::string field_name) {
		return field_map.find(field_name) != field_map.end() ? field_map.find(field_name)->second : NULL;
	}

	PMethodInfo GetMethodInfo(std::string method_name) {
		return method_map.find(method_name) != method_map.end() ? method_map.find(method_name)->second : NULL;
	}

	void AddMethodInfo(std::string method_name, PMethodInfo method_info) {
		method_map.insert(std::pair<std::string, PMethodInfo>(method_name, method_info));
	}
	

	void* CreateInstance()const {
		return generator != NULL ? generator() : NULL;
	}
};


class FieldInfo {
public:
	std::string field_type;
	std::string field_name;
	uint64_t offset;
	
	FieldInfo(const std::string &class_name, const std::string& ft, const std::string& fn, uint64_t offt) {
		field_type = ft;
		field_name = fn;
		offset = offt;
		PType type = GlobalRefrector::GetRefrector().class_map.find(class_name)->second;
		type->AddFieldInfo(field_name, this);
	}

	void* GetValueAddress(void* pins) {
		return (void*)(((uint64_t)pins) + offset);
	}
};

class MethodInfo {
public:
	std::string method_name;
	MethodInfo(const std::string &cn, const std::string& mn) {
		method_name = mn;
		PType type = GlobalRefrector::GetRefrector().class_map.find(cn)->second;
		type->AddMethodInfo(method_name, this);
	}
	
	template<typename ResType, typename ...Params>
	ResType InvokeWithRes(void* pins, Params...params) {
		ResType res;
		InvokeWithResDerive(&res, pins, params...);
		return res;
	}

	virtual void InvokeWithResDerive(void* res_address, void* pins, ...) {

	}

	virtual void InvokeWithoutResDerive(void* pins, ...) {

	}

	template<typename ...Params>
	void InvokeWithoutRes(void* pins, Params...params) {
		InvokeWithoutResDerive(pins, params...);
	}
};

template<typename ResType, typename ClassType, typename MethodType, typename ...Params>
class DerivedMethodWithRes :public MethodInfo {
private:
	void EmptyFunc(Params ...params) {

	}
	void JumpToEnd(va_list &ap) {
		EmptyFunc(va_arg(ap, Params)...);
	}

	template<typename T>
	T GetReverse(va_list& ap) {
		//va_arg -> (*(t*)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t))) 这里要逆着这个运算来，把ap有减回去。
		T x = *(T*)(ap - _INTSIZEOF(T));
		ap -= _INTSIZEOF(T);
		return x;
	}
public:
	MethodType func;
	DerivedMethodWithRes(const std::string& cn, const std::string& mn, MethodType fc) :MethodInfo(cn, mn) {
		func = fc;
	}

	virtual void InvokeWithResDerive(void* res_address, void* pins, ...) {
		va_list ap;
		va_start(ap, pins);
		JumpToEnd(ap);
		*(ResType*)res_address = ((ClassType*)pins->*func)(GetReverse<Params>(ap)...);
		va_end(ap);
	}
};

template<typename ClassType, typename MethodType, typename ...Params>
class DerivedMethodWithoutRes :public MethodInfo {
private:
	void EmptyFunc(Params ...params) {

	}
	void JumpToEnd(va_list &ap) {
		EmptyFunc(va_arg(ap, Params)...);
	}

	template<typename T>
	T GetReverse(va_list& ap) {
		//va_arg -> (*(t*)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t))) 这里要逆着这个运算来，把ap有减回去。
		T x = *(T*)(ap - _INTSIZEOF(T));
		ap -= _INTSIZEOF(T);
		return x;
	}
public:
	MethodType func;
	DerivedMethodWithoutRes(const std::string& cn, const std::string& mn, MethodType fc) :MethodInfo(cn, mn) {
		func = fc;
	}

	virtual void InvokeWithoutResDerive(void* pins, ...) {
		va_list ap;
		va_start(ap, pins);
		JumpToEnd(ap);
		((ClassType*)pins->*func)(GetReverse<Params>(ap)...);
		va_end(ap);
	}
};


//	获取结构体或类某个成员变量的偏移
#define MEMBER_OFFSET(classtype, member) (uint64_t)(&(((classtype*)0)->member))

#define DEFINE_PRIVATE_MEMBER(classtype, member)\
	public:\
	static uint64_t GetMemberOffset_##member(){\
		return (uint64_t)(&(((classtype*)0)->member));\
	}\
	private:\

#define DEFINE_PROTECTED_MEMBER(classtype, member)\
	public:\
	static uint64_t GetMemberOffset_##member(){\
		return (uint64_t)(&(((classtype*)0)->member));\
	}\
	protected:\

#define NON_PUBLIC_MEMBER_OFFSET(classtype, member)\
	classtype::GetMemberOffset_##member()

#define SET_AS_REFRECTABLE_CLASS(class_type)\
	static void* CreateInstance() {\
		return new class_type();\
	}\
	

#define	DEFINED_METHOD_POINTER(res_type, class_type, define_name, ...) typedef res_type(class_type::* define_name)(__VA_ARGS__);


#define REGISTER_CLASS(class_type,class_name)\
		Type g_type_##class_name(#class_type,#class_name, class_type::CreateInstance);

#define REGISTER_FIELD(class_type,class_name, field_type, field_name)\
	FieldInfo g_field_##class_name_##field_type_##field_name(#class_name, #field_type, #field_name, MEMBER_OFFSET(class_type, field_name));

#define REGISTER_NON_PUBLIC_FIELD(class_type,class_name, field_type, field_name)\
	FieldInfo g_field_##class_name_##field_type_##field_name(#class_name, #field_type, #field_name, NON_PUBLIC_MEMBER_OFFSET(class_type,field_name));

#define REGISTER_METHOD_WITHOUT_RES(class_type, class_name, method_pointer, method_name, ...)\
	DerivedMethodWithoutRes<class_type,class_type::method_pointer,##__VA_ARGS__> g_method_##class_type_##class_name_##method_pointer(#class_name,#method_name, &class_type::method_name);

#define REGISTER_METHOD_WITH_RES(res_type,class_type, class_name, method_pointer, method_name, ...)\
	DerivedMethodWithRes<res_type,class_type,class_type::method_pointer,##__VA_ARGS__> g_method_##res_type_##class_type_##class_name_##method_pointer(#class_name,#method_name, &class_type::method_name);