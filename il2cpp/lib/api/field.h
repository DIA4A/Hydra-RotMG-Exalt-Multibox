#pragma once
#include "../base.h"
#include "property.h"

namespace Il2Cpp {
    struct Field : FieldInfo {
        inline size_t GetOffset() {
            static void* fn = GetExport("il2cpp_field_get_offset");
            return reinterpret_cast<size_t(*)(Field*)>(fn)(this);
        }
    };
}