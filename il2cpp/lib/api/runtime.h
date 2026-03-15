#pragma once
#include "../base.h"
#include "Domain.h"

namespace Il2Cpp {
    struct Runtime {
        inline static Il2CppObject* Invoke(const MethodInfo* method, void* obj, void** params, Il2CppException** exc) {
            static void* fn = GetExport("il2cpp_runtime_invoke");
            return reinterpret_cast<Il2CppObject * (*)(const MethodInfo*, void*, void**, Il2CppException**)>(fn)(method, obj, params, exc);
        }
    };
}