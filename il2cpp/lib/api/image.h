#pragma once
#include "../base.h"

namespace Il2Cpp {
    struct Image : Il2CppImage {
        Class* GetClass(const char* _namespace, const char* name) {
            static void* fn = GetExport("il2cpp_class_from_name");
            return reinterpret_cast<Class * (*)(Image*, const char*, const char*)>(fn)(this, _namespace, name);
        }
    };
}