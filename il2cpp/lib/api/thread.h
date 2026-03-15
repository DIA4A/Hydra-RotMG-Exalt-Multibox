#pragma once
#include "../base.h"
#include "Domain.h"

namespace Il2Cpp {
    struct Thread : Il2CppThread {
        inline static Thread* Attach(Il2CppDomain* domain) {
            static void* fn = GetExport("il2cpp_thread_attach");
            return reinterpret_cast<Thread * (*)(Il2CppDomain*)>(fn)(domain);
        }

        inline void Detach() {
            static void* fn = GetExport("il2cpp_thread_detach");
            return reinterpret_cast<void(*)(Thread*)>(fn)(this);
        }
    };

    struct ThreadAttacher {
        Thread* thread = nullptr;
        ThreadAttacher(Il2CppDomain* domain = nullptr)
        {
            thread = Thread().Attach(domain ? domain : Domain::Get());
        }
        ~ThreadAttacher()
        {
            if (thread)
            {
                thread->Detach();
                thread = nullptr;
            }
        }
	};
}