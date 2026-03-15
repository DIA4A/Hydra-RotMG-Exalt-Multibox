#pragma once
#include <functional>
#include <string>
#include "../il2cpp/il2cpp.h"

__declspec(noinline) inline void RaiseError(const std::string& strMessage)
{
	MessageBoxA(NULL, strMessage.c_str(), "Error", MB_ICONERROR | MB_OK);
	__debugbreak();
}

__declspec(noinline) inline std::string MakeNullClassError(const char* szClass)
{
	char error[256];
	sprintf_s(error, "Class [%s] is null, possibly not initialized anywhere", szClass);
	return error;
}

__declspec(noinline) inline std::string MakeFieldNotFoundError(const char* szClass, const char* szField)
{
	char error[256];
	sprintf_s(error, "Failed to find field [%s] via the name in the class [%s]", szField, szClass);
	return error;
}

__declspec(noinline) inline std::string MakeMethodNotFoundError(const char* szClass, const char* szMethod)
{
	char error[256];
	sprintf_s(error, "Failed to find method [%s] via the name in the class [%s]", szMethod, szClass);
	return error;
}

#define NULL_CLASS_ERROR(_class) RaiseError(MakeNullClassError(#_class))
#define FIELD_NOT_FOUND_ERROR(_class, _field) RaiseError(MakeFieldNotFoundError(#_class, #_field))
#define METHOD_NOT_FOUND_ERROR(_class, _method) RaiseError(MakeMethodNotFoundError(#_class, #_method))

#define ASSERT_CLASS_FIELD(_class, _field) \
	if (!_class) NULL_CLASS_ERROR(_class); \
	if (!_field) FIELD_NOT_FOUND_ERROR(_class, _field);

#define ASSERT_CLASS_METHOD(_class, _method) \
	if (!_class) NULL_CLASS_ERROR(_class); \
	if (!_method) METHOD_NOT_FOUND_ERROR(_class, _method);

#define BASE_FIELD_FIND(_class, name) \
	static auto field = _class->GetFieldFromName(name); \
	ASSERT_CLASS_FIELD(name, field); \
	static size_t offset = field->GetOffset(); 

#define FIELD(type, name, baseclass) \
type& name() \
{ \
	BASE_FIELD_FIND(baseclass, NameMapping::baseclass::##name); \
	return *(type*)(uintptr_t(this) + offset); \
}

#define FIELD2(type, name, baseclass, beebytename) \
type& name() \
{ \
	BASE_FIELD_FIND(baseclass, beebytename); \
	return *(type*)(uintptr_t(this) + offset); \
}

#define PARAMETERS(...) __VA_ARGS__

template <typename T>
struct function_traits;

template <typename R, typename... Args>
struct function_traits<R(*)(Args...)> {
	using return_type = R;
	using args_tuple = std::tuple<Args...>;

	static constexpr size_t arg_count = sizeof...(Args);

	template <size_t N>
	using arg_t = std::tuple_element_t<N, args_tuple>;
};

template <typename R, typename... Args, size_t... I>
auto BaseMakeClassCallableWithMethod(R(*funcptr)(void*, Args...), void* thisptr, const MethodInfo* methodInfo, std::index_sequence<I...>)
{
	return [funcptr, thisptr, methodInfo](std::tuple_element_t<I, std::tuple<Args...>>... params) -> R { return funcptr(thisptr, params..., methodInfo); };
}

template <typename R, typename... Args, size_t... I>
auto BaseMakeCallableWithMethod(R(*funcptr)(Args...), const MethodInfo* methodInfo, std::index_sequence<I...>)
{
	return [funcptr, methodInfo](std::tuple_element_t<I, std::tuple<Args...>>... params) -> R { return funcptr(params..., methodInfo); };
}

template <typename R, typename... Y>
inline auto MakeClassCallableWithMethod(R(*funcptr)(void*, Y...), void* thisptr, const MethodInfo* methodInfo)
{
	return BaseMakeClassCallableWithMethod(funcptr, thisptr, methodInfo, std::make_index_sequence<sizeof...(Y) - 1>{});
}

template <typename R, typename... Y>
inline auto MakeCallableWithMethod(R(*funcptr)(Y...), const MethodInfo* methodInfo)
{
	return BaseMakeCallableWithMethod(funcptr, methodInfo, std::make_index_sequence<sizeof...(Y) - 1>{});
}

#define METHOD(name, baseclass, rettype, ...) \
static const MethodInfo* name##MethodInfo() \
{ \
	static auto method = baseclass->GetMethod(NameMapping::baseclass::Method_##name, NameMapping::baseclass::Method_##name##_ArgCount); \
	ASSERT_CLASS_METHOD(name##MethodInfo, method); \
	return method; \
} \
using name##Type = rettype(*)(void*, __VA_ARGS__, const MethodInfo* method); \
auto name() \
{ \
	static const auto baseMethod = name##MethodInfo(); \
	static auto method = (name##Type)baseMethod->methodPointer; \
	return MakeClassCallableWithMethod<rettype>(method, this, baseMethod); \
}

#define STATIC_METHOD(name, baseclass, rettype, ...) \
static const MethodInfo* name##MethodInfo() \
{ \
	static auto method = baseclass->GetMethod(NameMapping::baseclass::Method_##name, NameMapping::baseclass::Method_##name##_ArgCount); \
	ASSERT_CLASS_METHOD(name##MethodInfo, method); \
	return method; \
} \
using name##Type = rettype(*)(__VA_ARGS__, const MethodInfo* method); \
static auto name() \
{ \
	static const auto baseMethod = name##MethodInfo(); \
	static auto method = (name##Type)baseMethod->methodPointer;\
	return MakeCallableWithMethod<rettype>(method, baseMethod);\
}

#define STATIC_METHOD_NO_PARAM(name, baseclass, rettype) \
static const MethodInfo* name##MethodInfo() \
{ \
	static auto method = baseclass->GetMethod(NameMapping::baseclass::Method_##name, NameMapping::baseclass::Method_##name##_ArgCount); \
	ASSERT_CLASS_METHOD(name##MethodInfo, method); \
	return method; \
} \
using name##Type = rettype(*)(const MethodInfo* method); \
static auto name() \
{ \
	static const auto baseMethod = name##MethodInfo(); \
	static auto method = (name##Type)baseMethod->methodPointer;\
	return MakeCallableWithMethod<rettype>(method, baseMethod);\
}
