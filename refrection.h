#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
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
	std::vector<std::string> parent_class_names;
public:
	std::string class_type;
	std::string class_name;

	Type() {

	}

	Type(const std::string& ct, const std::string& cn, PInstanceGenerator pg, int parent_count, ...) {
		class_type = ct;
		class_name = cn;
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

	void AddFieldInfo(std::string field_name, PFieldInfo field_info) {
		if (field_map.find(field_name) == field_map.end()) {
			field_map.insert(std::pair<std::string, PFieldInfo>(field_name, field_info));
		}
	}

	PFieldInfo GetFieldInfoInParents(std::string &field_name) {
		for(int i = 0;i<parent_class_names.size();i++)
		{
			std::string parent_name = parent_class_names[i];
			auto it = GlobalRefrector::GetRefrector().class_map.find(parent_name);
			if (it != GlobalRefrector::GetRefrector().class_map.end()) {
				PType ptype = it->second;
				PFieldInfo pfield = ptype->GetFieldInfo(field_name);
				if (pfield != NULL) {
					return pfield;
				}
			}
		}
		return NULL;
	}

	PMethodInfo GetMethodInfoInParents(std::string& method_name) {
		for (int i = 0; i < parent_class_names.size(); i++)
		{
			std::string parent_name = parent_class_names[i];
			auto it = GlobalRefrector::GetRefrector().class_map.find(parent_name);
			if (it != GlobalRefrector::GetRefrector().class_map.end()) {
				PType ptype = it->second;
				PMethodInfo pmethod = ptype->GetMethodInfo(method_name);
				if (pmethod != NULL) {
					return pmethod;
				}
			}
		}
		return NULL;
	}

	PFieldInfo GetFieldInfo(std::string field_name) {
		return field_map.find(field_name) != field_map.end() ? field_map.find(field_name)->second : GetFieldInfoInParents(field_name);
	}

	PMethodInfo GetMethodInfo(std::string method_name) {
		return method_map.find(method_name) != method_map.end() ? method_map.find(method_name)->second : GetMethodInfoInParents(method_name);
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
	bool is_static_method;
	MethodInfo(const std::string &cn, const std::string& mn,bool is_static = false) {
		method_name = mn;
		is_static_method = is_static;
		PType type = GlobalRefrector::GetRefrector().class_map.find(cn)->second;
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
		InvokeStaticMethodWithoutResDerive(NULL,params...);
	}
};

template<typename ResType, typename ClassType, typename MethodType, typename ...Params>
class DerivedMethodWithRes :public MethodInfo {
private:
	void EmptyFunc(...) {

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

template<typename ResType, typename MethodType, typename ...Params>
class StaticDerivedMethodWithRes :public MethodInfo {
private:
	void EmptyFunc(...) {

	}
	void JumpToEnd(va_list& ap) {
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
	StaticDerivedMethodWithRes(const std::string& cn, const std::string& mn, MethodType fc) :MethodInfo(cn, mn,true) {
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

template<typename MethodType, typename ...Params>
class StaticDerivedMethodWithoutRes :public MethodInfo {
private:
	void EmptyFunc(...) {

	}
	void JumpToEnd(va_list& ap) {
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
	StaticDerivedMethodWithoutRes(const std::string& cn, const std::string& mn, MethodType fc) :MethodInfo(cn, mn, true) {
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
	StaticDerivedMethodWithoutRes<class_type::method_pointer,##__VA_ARGS__> g_method_##class_type_##class_name_##method_pointer(#class_name,#method_name, &class_type::method_name);\

#define REGISTER_NON_PUBLIC_STATIC_METHOD_WITHOUT_RES(class_type, class_name, method_pointer, method_name, ...)\
	StaticDerivedMethodWithoutRes<class_type::method_pointer,##__VA_ARGS__> g_method_##class_type_##class_name_##method_pointer(#class_name,#method_name, class_type::GetStaticMethodPointer_##method_name());\

#define REGISTER_STATIC_METHOD_WITH_RES(res_type,class_type, class_name, method_pointer, method_name, ...)\
	StaticDerivedMethodWithRes<class_type::method_pointer,##__VA_ARGS__> g_method_##class_type_##class_name_##method_pointer(#class_name,#method_name, &class_type::method_name);\

#define REGISTER_NON_PUBLIC_METHOD_WITH_RES(res_type,class_type, class_name, method_pointer, method_name, ...)\
	StaticDerivedMethodWithRes<class_type::method_pointer,##__VA_ARGS__> g_method_##class_type_##class_name_##method_pointer(#class_name,#method_name, class_type::GetStaticMethodPointer_##method_name());\
