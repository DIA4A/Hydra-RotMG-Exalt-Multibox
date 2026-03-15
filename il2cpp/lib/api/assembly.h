#pragma once
#include "../base.h"

namespace Il2Cpp {
    struct Assembly : Il2CppAssembly {
        Image* GetImage() {
            static void* fn = GetExport("il2cpp_assembly_get_image");
            return reinterpret_cast<Image*(*)(Assembly*)>(fn)(this);
        }
    };
}