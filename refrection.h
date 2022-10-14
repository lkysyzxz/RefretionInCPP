#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <stdarg.h>

typedef void* (*PInstanceGenerator)();
typedef void* (*PAbstractInstanceGenerator)(const std::string& deriedClassName);

class BaseType;

typedef BaseType* PBaseType;

class ClassType;

typedef ClassType* PClassType;

class Type;

typedef Type* PType;

class AbstractType;

typedef AbstractType* PAbstractType;

class StructType;

typedef StructType* PStructType;

class FieldInfo;

typedef FieldInfo* PFieldInfo;

class MethodInfo;

typedef MethodInfo* PMethodInfo;

class FunctionInfo;

typedef FunctionInfo* PFunctionInfo;

class GlobalRefrector {
public:
	static GlobalRefrector& GetRefrector();
public:
	std::map<std::string, PBaseType> class_map;
	std::map<std::string, PFunctionInfo> function_map;
};

class BaseType {
protected:
	std::map<std::string, PFieldInfo> field_map;
public:
	std::string class_type;
	std::string class_name;

	BaseType() {

	}

	BaseType(const std::string& ct, const std::string& cn) {
		class_type = ct;
		class_name = cn;
	}

	virtual PFieldInfo GetFieldInfo(std::string field_name) {
		return field_map.find(field_name) != field_map.end() ? field_map.find(field_name)->second : NULL;
	}

	void AddFieldInfo(std::string field_name, PFieldInfo field_info) {
		if (field_map.find(field_name) == field_map.end()) {
			field_map.insert(std::pair<std::string, PFieldInfo>(field_name, field_info));
		}
	}

	virtual void* CreateInstance() {
		return NULL;
	}
};

class ClassType :public BaseType {
protected:
	std::map<std::string, PMethodInfo> method_map;
	std::vector<std::string> parent_class_names;
public:
	ClassType() {

	}

	ClassType(const std::string& ct, const std::string& cn):BaseType(ct,cn) {
	}

	PFieldInfo GetFieldInfoInParents(std::string& field_name) {
		for (int i = 0; i < parent_class_names.size(); i++)
		{
			std::string parent_name = parent_class_names[i];
			auto it = GlobalRefrector::GetRefrector().class_map.find(parent_name);
			if (it != GlobalRefrector::GetRefrector().class_map.end()) {
				PBaseType ptype = it->second;
				PFieldInfo pfield = ptype->GetFieldInfo(field_name);
				if (pfield != NULL) {
					return pfield;
				}
			}
		}
		return NULL;
	}

	virtual PFieldInfo GetFieldInfo(std::string field_name) {
		return field_map.find(field_name) != field_map.end() ? field_map.find(field_name)->second : GetFieldInfoInParents(field_name);
	}

	PMethodInfo GetMethodInfoInParents(std::string& method_name) {
		for (int i = 0; i < parent_class_names.size(); i++)
		{
			std::string parent_name = parent_class_names[i];
			auto it = GlobalRefrector::GetRefrector().class_map.find(parent_name);
			if (it != GlobalRefrector::GetRefrector().class_map.end()) {
				PClassType ptype = (PClassType)it->second;
				PMethodInfo pmethod = ptype->GetMethodInfo(method_name);
				if (pmethod != NULL) {
					return pmethod;
				}
			}
		}
		return NULL;
	}

	PMethodInfo GetMethodInfo(std::string method_name) {
		return method_map.find(method_name) != method_map.end() ? method_map.find(method_name)->second : GetMethodInfoInParents(method_name);
	}

	void AddMethodInfo(std::string method_name, PMethodInfo method_info) {
		method_map.insert(std::pair<std::string, PMethodInfo>(method_name, method_info));
	}

	virtual void* CreateInstance(const std::string& derivedClassName) {
		return NULL;
	}
};

class Type :public ClassType {
private:
	PInstanceGenerator generator;
public:
	Type() {

	}

	Type(const std::string& ct, const std::string& cn, PInstanceGenerator pg, int parent_count, ...)
		:ClassType(ct, cn) {
		generator = pg;
		GlobalRefrector::GetRefrector().class_map.insert(std::map<std::string, Type*>::value_type(class_name, this));
		va_list ap;
		va_start(ap, parent_count);
		for (int i = 0; i < parent_count; i++) {
			char* parent_name = va_arg(ap, char*);
			parent_class_names.push_back(std::string(parent_name));
		}
		va_end(ap);
	}

	virtual void* CreateInstance() {
		return generator != NULL ? generator() : NULL;
	}
};

class StructType:public BaseType{
private:
	PInstanceGenerator generator;
public:

	StructType() {

	}

	StructType(const std::string& sn, PInstanceGenerator pg):BaseType(sn,sn){
		generator = pg;
		GlobalRefrector::GetRefrector().class_map.insert(std::map<std::string, PBaseType>::value_type(sn, this));
	}

	virtual void* CreateInstance() {
		return generator != NULL ? generator() : NULL;
	}
};

class AbstractType :public ClassType {
private:
	PAbstractInstanceGenerator generator;
public:
	AbstractType() {

	}

	AbstractType(const std::string& ct, const std::string& cn, PAbstractInstanceGenerator pg, int parent_count, ...)
		:ClassType(ct, cn)
	{
		generator = pg;
		GlobalRefrector::GetRefrector().class_map.insert(std::map<std::string, AbstractType*>::value_type(class_name, this));
		va_list ap;
		va_start(ap, parent_count);
		for (int i = 0; i < parent_count; i++) {
			char* parent_name = va_arg(ap, char*);
			parent_class_names.push_back(std::string(parent_name));
		}
		va_end(ap);
	}

	virtual void* CreateInstance(const std::string& derivedClassName) {
		return generator != NULL ? generator(derivedClassName) : NULL;
	}
};


class FieldInfo {
public:
	std::string field_type;
	std::string field_name;
	uint64_t offset;

	FieldInfo(const std::string& class_name, const std::string& ft, const std::string& fn, uint64_t offt) {
		field_type = ft;
		field_name = fn;
		offset = offt;
		PBaseType type = GlobalRefrector::GetRefrector().class_map.find(class_name)->second;
		type->AddFieldInfo(field_name, this);
	}

	void* GetValueAddress(void* pins) {
		return (void*)(((uint64_t)pins) + offset);
	}

	template<typename T>
	T GetValue(void *pins) {
		return *((T*)(((uint64_t)pins) + offset));
	}

	void* GetPointerValue(void* pins) {
		return (void*)(*(uint64_t*)(((uint64_t)pins) + offset));
	}
};

class MethodInfo {
public:
	std::string method_name;
	bool is_static_method;
	MethodInfo(const std::string& cn, const std::string& mn, bool is_static = false) {
		method_name = mn;
		is_static_method = is_static;
		PClassType type = (PClassType)GlobalRefrector::GetRefrector().class_map.find(cn)->second;
		type->AddMethodInfo(method_name, this);
	}

	template<typename ResType, typename ...Params>
	ResType InvokeWithRes(void* pins, Params...params) {
		ResType res;
		InvokeWithResDerive(&res, pins, params...);
		return res;
	}

	template<typename ResType, typename ...Params>
	ResType InvokeStaticWithRes(Params...params) {
		ResType res;
		InvokeStaticMethodWithResDerive(&res, params...);
		return res;
	}

	virtual void InvokeWithResDerive(void* res_address, void* pins, ...) {

	}

	virtual void InvokeWithoutResDerive(void* pins, ...) {

	}

	virtual void InvokeStaticMethodWithResDerive(void* res_address, ...) {

	}

	virtual void InvokeStaticMethodWithoutResDerive(void* place_holder, ...) {

	}

	template<typename ...Params>
	void InvokeWithoutRes(void* pins, Params...params) {
		InvokeWithoutResDerive(pins, params...);
	}

	template<typename ...Params>
	void InvokeStaticWithoutRes(Params...params) {
		InvokeStaticMethodWithoutResDerive(NULL, params...);
	}
};

template<typename ResType, typename ClassType, typename MethodType, typename ...Params>
class DerivedMethodWithRes :public MethodInfo {
private:
	void EmptyFunc(...) {

	}
	void JumpToEnd(va_list& ap) {
		EmptyFunc(va_arg(ap, Params)...);
	}

	template<typename T>
	T GetReverse(va_list& ap) {
		//va_arg -> (*(t*)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t))) ����Ҫ�����������������ap�м���ȥ��
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

template<typename ResType, typename MethodType, typename ...Params>
class StaticMethodWithRes :public MethodInfo {
private:
	void EmptyFunc(...) {

	}
	void JumpToEnd(va_list& ap) {
		EmptyFunc(va_arg(ap, Params)...);
	}

	template<typename T>
	T GetReverse(va_list& ap) {
		//va_arg -> (*(t*)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t))) ����Ҫ�����������������ap�м���ȥ��
		T x = *(T*)(ap - _INTSIZEOF(T));
		ap -= _INTSIZEOF(T);
		return x;
	}
public:
	MethodType func;
	StaticMethodWithRes(const std::string& cn, const std::string& mn, MethodType fc) :MethodInfo(cn, mn, true) {
		func = fc;
	}

	virtual void InvokeStaticMethodWithResDerive(void* res_address, void* pins, ...) {
		va_list ap;
		va_start(ap, pins);
		JumpToEnd(ap);
		*(ResType*)res_address = (*func)(GetReverse<Params>(ap)...);
		va_end(ap);
	}
};


template<typename ClassType, typename MethodType, typename ...Params>
class DerivedMethodWithoutRes :public MethodInfo {
private:
	void EmptyFunc(...) {

	}
	void JumpToEnd(va_list& ap) {
		EmptyFunc(va_arg(ap, Params)...);
	}

	template<typename T>
	T GetReverse(va_list& ap) {
		//va_arg -> (*(t*)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t))) ����Ҫ�����������������ap�м���ȥ��
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

template<typename MethodType, typename ...Params>
class StaticMethodWithoutRes :public MethodInfo {
private:
	void EmptyFunc(...) {

	}
	void JumpToEnd(va_list& ap) {
		EmptyFunc(va_arg(ap, Params)...);
	}

	template<typename T>
	T GetReverse(va_list& ap) {
		//va_arg -> (*(t*)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t))) ����Ҫ�����������������ap�м���ȥ��
		T x = *(T*)(ap - _INTSIZEOF(T));
		ap -= _INTSIZEOF(T);
		return x;
	}
public:
	MethodType func;
	StaticMethodWithoutRes(const std::string& cn, const std::string& mn, MethodType fc) :MethodInfo(cn, mn, true) {
		func = fc;
	}

	virtual void InvokeStaticMethodWithoutResDerive(void* placeholder, ...) {
		va_list ap;
		va_start(ap, placeholder);
		JumpToEnd(ap);
		(*func)(GetReverse<Params>(ap)...);
		va_end(ap);
	}
};

class FunctionInfo {
public:
	std::string function_name;
	FunctionInfo(const std::string& fn) {
		function_name = fn;
		if (GlobalRefrector::GetRefrector().function_map.find(fn) == GlobalRefrector::GetRefrector().function_map.end()) {
			GlobalRefrector::GetRefrector().function_map.insert(std::pair<std::string, PFunctionInfo>(fn, this));
		}
	}

	template<typename ResType, typename ...Params>
	ResType InvokeWithRes(Params...params) {
		ResType res;
		InvokeWithResDerive(&res, params...);
		return res;
	}

	virtual void InvokeWithResDerive(void* res_address, ...) {

	}

	virtual void InvokeWithoutResDerive(void* placeholder, ...) {

	}

	template<typename ...Params>
	void InvokeWithoutRes(Params...params) {
		InvokeWithoutResDerive(NULL, params...);
	}
};

template<typename ResType, typename FunctionType, typename ...Params>
class FunctionWithRes :public FunctionInfo {
private:
	void EmptyFunc(...) {

	}
	void JumpToEnd(va_list& ap) {
		EmptyFunc(va_arg(ap, Params)...);
	}

	template<typename T>
	T GetReverse(va_list& ap) {
		//va_arg -> (*(t*)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t))) ����Ҫ�����������������ap�м���ȥ��
		T x = *(T*)(ap - _INTSIZEOF(T));
		ap -= _INTSIZEOF(T);
		return x;
	}
public:
	FunctionType func;
	FunctionWithRes(const std::string& fn, FunctionType fc) :FunctionInfo(fn) {
		func = fc;
	}

	virtual void InvokeWithResDerive(void* res_address, ...) {
		va_list ap;
		va_start(ap, res_address);
		JumpToEnd(ap);
		*(ResType*)res_address = (*func)(GetReverse<Params>(ap)...);
		va_end(ap);
	}
};

template<typename FunctionType, typename ...Params>
class FunctionWithoutRes :public FunctionInfo {
private:
	void EmptyFunc(...) {

	}
	void JumpToEnd(va_list& ap) {
		EmptyFunc(va_arg(ap, Params)...);
	}

	template<typename T>
	T GetReverse(va_list& ap) {
		//va_arg -> (*(t*)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t))) ����Ҫ�����������������ap�м���ȥ��
		T x = *(T*)(ap - _INTSIZEOF(T));
		ap -= _INTSIZEOF(T);
		return x;
	}
public:
	FunctionType func;
	FunctionWithoutRes(const std::string& fn, FunctionType fc) :FunctionInfo(fn) {
		func = fc;
	}

	virtual void InvokeWithoutResDerive(void* placeholder, ...) {
		va_list ap;
		va_start(ap, placeholder);
		JumpToEnd(ap);
		(*func)(GetReverse<Params>(ap)...);
		va_end(ap);
	}
};

//	��ȡ�ṹ�����ĳ����Ա������ƫ��
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

#define SET_AS_REFRECTABLE_STRUCT(struct_type)\
	void *CreateInstance##struct_type(){\
		return new struct_type();\
	}\

#define SET_AS_ABSTRACT_REFRECTALE_CLASS()\
	static void* CreateInstance(const std::string& class_name) {\
		auto it = GlobalRefrector::GetRefrector().class_map.find(class_name);\
		if (it != GlobalRefrector::GetRefrector().class_map.end()) {\
			PBaseType derivedType = GlobalRefrector::GetRefrector().class_map[class_name];\
			return derivedType->CreateInstance();\
		}\
		return NULL;\
	}\


#define	DEFINED_METHOD_POINTER(res_type, class_type, define_name, ...) typedef res_type(class_type::* define_name)(__VA_ARGS__);

#define	REFRECT_PRIVATE_METHOD(res_type, class_type, method_name,define_name, ...) \
	public:\
		typedef res_type(class_type::* define_name)(__VA_ARGS__);\
		static define_name GetMethodPointer_##method_name(){\
			return &class_type::method_name;\
		}\
	private:\

#define	REFRECT_PROTECTED_METHOD(res_type, class_type, method_name,define_name, ...) \
	public:\
		typedef res_type(class_type::* define_name)(__VA_ARGS__);\
		static define_name GetMethodPointer_##method_name(){\
			return &class_type::method_name;\
		}\
	protected:\

#define	DEFINED_METHOD_POINTER(res_type, class_type, define_name, ...) typedef res_type(class_type::* define_name)(__VA_ARGS__);

#define REGISTER_CLASS(class_type,class_name,parent_count,...)\
	Type g_type_##class_name(#class_type,#class_name, class_type::CreateInstance,parent_count, ##__VA_ARGS__);

#define REGISTER_ABSTRACT_CLASS(class_type, class_name, parent_count,...)\
	AbstractType g_type_##class_name(#class_type,#class_name, class_type::CreateInstance,parent_count, ##__VA_ARGS__);

#define REGISTER_FIELD(class_type,class_name, field_type, field_name)\
	FieldInfo g_field_##class_name_##field_type_##field_name(#class_name, #field_type, #field_name, MEMBER_OFFSET(class_type, field_name));

#define REGISTER_NON_PUBLIC_FIELD(class_type,class_name, field_type, field_name)\
	FieldInfo g_field_##class_name_##field_type_##field_name(#class_name, #field_type, #field_name, NON_PUBLIC_MEMBER_OFFSET(class_type,field_name));

#define REGISTER_METHOD_WITHOUT_RES(class_type, class_name, method_pointer, method_name, ...)\
	DerivedMethodWithoutRes<class_type,class_type::method_pointer,##__VA_ARGS__> g_method_##class_type_##class_name_##method_pointer(#class_name,#method_name, &class_type::method_name);

#define REGISTER_NON_PUBLIC_METHOD_WITHOUT_RES(class_type, class_name, method_pointer, method_name, ...)\
	DerivedMethodWithoutRes<class_type,class_type::method_pointer,##__VA_ARGS__> g_method_##class_type_##class_name_##method_pointer(#class_name,#method_name, class_type::GetMethodPointer_##method_name());

#define REGISTER_METHOD_WITH_RES(res_type,class_type, class_name, method_pointer, method_name, ...)\
	DerivedMethodWithRes<res_type,class_type,class_type::method_pointer,##__VA_ARGS__> g_method_##res_type_##class_type_##class_name_##method_pointer(#class_name,#method_name, &class_type::method_name);

#define REGISTER_NON_PUBLIC_METHOD_WITH_RES(res_type,class_type, class_name, method_pointer, method_name, ...)\
	DerivedMethodWithRes<res_type,class_type,class_type::method_pointer,##__VA_ARGS__> g_method_##res_type_##class_type_##class_name_##method_pointer(#class_name,#method_name, class_type::GetMethodPointer_##method_name());

#define	DEFINED_STATIC_METHOD_POINTER(res_type, class_type, method_name, define_name, ...) typedef res_type (*define_name)(__VA_ARGS__);

#define	REFRECT_PRIVATE_STATIC_METHOD_POINTER(res_type, class_type, method_name, define_name, ...) \
	public:\
		typedef res_type(* define_name)(__VA_ARGS__);\
		static define_name GetStaticMethodPointer_##method_name(){\
			return &class_type::method_name;\
		}\
	private:\

#define	REFRECT_PROTECTED_STATIC_METHOD_POINTER(res_type, class_type, method_name, define_name, ...) \
	public:\
		typedef res_type(*define_name)(__VA_ARGS__);\
		static define_name GetStaticMethodPointer_##method_name(){\
			return &class_type::method_name;\
		}\
	protected:\

#define REGISTER_STATIC_METHOD_WITHOUT_RES(class_type, class_name, method_pointer, method_name, ...)\
	StaticMethodWithoutRes<class_type::method_pointer,##__VA_ARGS__> g_method_##class_type_##class_name_##method_pointer(#class_name,#method_name, &class_type::method_name);\

#define REGISTER_NON_PUBLIC_STATIC_METHOD_WITHOUT_RES(class_type, class_name, method_pointer, method_name, ...)\
	StaticMethodWithoutRes<class_type::method_pointer,##__VA_ARGS__> g_method_##class_type_##class_name_##method_pointer(#class_name,#method_name, class_type::GetStaticMethodPointer_##method_name());\

#define REGISTER_STATIC_METHOD_WITH_RES(res_type,class_type, class_name, method_pointer, method_name, ...)\
	StaticMethodWithRes<class_type::method_pointer,##__VA_ARGS__> g_method_##class_type_##class_name_##method_pointer(#class_name,#method_name, &class_type::method_name);\

#define REGISTER_NON_PUBLIC_METHOD_WITH_RES(res_type,class_type, class_name, method_pointer, method_name, ...)\
	StaticMethodWithRes<class_type::method_pointer,##__VA_ARGS__> g_method_##class_type_##class_name_##method_pointer(#class_name,#method_name, class_type::GetStaticMethodPointer_##method_name());\

#define DEFINE_FUNCTION_POINTER(res_type, define_name, ...) typedef res_type (*define_name)(__VA_ARGS__);

#define REGISTER_STRUCT(struct_name)\
	SET_AS_REFRECTABLE_STRUCT(struct_name)\
	StructType g_type_##struct_name(#struct_name, CreateInstance##struct_name);

#define REGISTER_FUNCTION_WITH_RES(res_type,function_type, function_name, ...)\
	DEFINE_FUNCTION_POINTER(res_type, function_type, __VA_ARGS__)\
	FunctionWithRes<res_type,function_type, ##__VA_ARGS__> g_function_##res_type_##function_type_##function_name(#function_name, &function_name);

#define REGISTER_FUNCTION_WITHOUT_RES(function_type, function_name, ...)\
	DEFINE_FUNCTION_POINTER(void, function_type, __VA_ARGS__)\
	FunctionWithoutRes<function_type, ##__VA_ARGS__> g_function_##function_type_##function_name(#function_name, &function_name);