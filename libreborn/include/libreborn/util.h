#pragma once

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>

#include "log.h"

// Check Memory Allocation
#define ALLOC_CHECK(obj) \
    { \
        if (obj == NULL) { \
            ERR("(%s:%i) Memory Allocation Failed", __FILE__, __LINE__); \
        } \
    }

// Hook Library Function
#define HOOK(name, return_type, args) \
    typedef return_type (*name##_t)args; \
    static name##_t real_##name = NULL; \
    \
    __attribute__((__unused__)) static void ensure_##name() { \
        if (!real_##name) { \
            dlerror(); \
            real_##name = (name##_t) dlsym(RTLD_NEXT, #name); \
            if (!real_##name) { \
                ERR("Error Resolving Symbol: "#name": %s", dlerror()); \
            } \
        } \
    } \
    \
    __attribute__((__used__)) return_type name args

// Macro To Reset Environmental Variables To Pre-MCPI State
#define RESET_ENVIRONMENTAL_VARIABLE(name) \
    { \
        char *original_env_value = getenv("ORIGINAL_" name); \
        if (original_env_value != NULL) { \
            setenv(name, original_env_value, 1); \
        } else { \
            unsetenv(name); \
        } \
    }

// Safe Version Of pipe()
static inline void safe_pipe2(int pipefd[2], int flags) {
    if (pipe2(pipefd, flags) != 0) {
        ERR("Unable To Create Pipe: %s", strerror(errno));
    }
}
