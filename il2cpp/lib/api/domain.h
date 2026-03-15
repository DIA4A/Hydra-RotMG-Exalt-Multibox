#pragma once
#include "../base.h"
#include "assembly.h"

namespace Il2Cpp {
    struct Domain : Il2CppDomain {
        static inline Domain* Get() {
            static void* fn = GetExport("il2cpp_domain_get");
            return reinterpret_cast<Domain*(*)()>(fn)();
        }

        Assembly* OpenAssembly(const char* name) {
            static void* fn = GetExport("il2cpp_domain_assembly_open");
            return reinterpret_cast<Assembly *(*)(Domain*, const char*)>(fn)(this, name);
		}
    };
}