// This file defines important data structures associated with
// Java class objects. These data structures are used to support
// JVM/JNI functionality, such as reflection.
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unordered_map>
#include <string>
#include "jni.h"
#include "class.h"

static constexpr bool kDebug = false;

// For simplicity we store class information in a map.
// If we find this to be too slow, we could allocate extra memory for
// class objects and store the information inline with each instance.
static std::unordered_map<jclass, const JavaClassInfo*> classes;
static std::unordered_map<std::string, const jclass> cnames;

extern "C" {

void RegisterJavaClass(jclass cls, const JavaClassInfo* info) {

    if (kDebug) {

        printf("loading class %s with super class %s\n",
            info->name,
            info->super_ptr
                ? GetJavaClassInfo(*info->super_ptr)->name
                : "[none]");

        for (int32_t i = 0; i < info->num_fields; ++i) {
            auto* f = &info->fields[i];
            printf("  found field %s with offset %d\n", f->name, f->offset);
        }

        for (int32_t i = 0; i < info->num_static_fields; ++i) {
            auto* f = &info->static_fields[i];
            printf("  found static field %s with sig %s and ptr %p\n", f->name, f->sig, f->ptr);
        }

        for (int32_t i = 0; i < info->num_methods; ++i) {
            auto* m = &info->methods[i];
            printf(
                "  found method %s%s\n"
                "    offset %d\n"
                "    function pointer %p\n"
                "    trampoline pointer %p\n"
                , m->name, m->sig, m->offset, m->fnPtr, m->trampoline);
        }
    }

    assert(classes.count(cls) == 0 && "Java class was loaded twice!");
    classes.emplace(cls, info);
    std::string cname(info->name);
    cnames.emplace(cname, cls);
}

} // extern "C"

const JavaClassInfo*
GetJavaClassInfo(jclass cls) {
    return classes.at(cls);
}

const jclass
GetJavaClassFromName(const char* name) {
  return cnames.at(std::string(name));
}

const JavaStaticFieldInfo*
GetJavaStaticFieldInfo(jclass cls, const char* name, const char* sig) {
    auto* clazz = classes.at(cls);
    auto* fields = clazz->static_fields;
    for (int32_t i = 0, e = clazz->num_static_fields; i < e; ++i) {
        auto* f = &fields[i];
        if (strcmp(name, f->name) == 0 && strcmp(sig, f->sig) == 0) {
            return f;
        }
    }

    // TODO: Should technically throw NoSuchFieldError.
    fprintf(stderr,
        "Could not find static field %s in class %s. Aborting.\n",
        name, clazz->name);
    abort();
}

const JavaFieldInfo*
GetJavaFieldInfo(jclass cls, const char* name) {
    auto* clazz = classes.at(cls);
    auto* fields = clazz->fields;
    for (int32_t i = 0, e = clazz->num_fields; i < e; ++i) {
        auto* f = &fields[i];
        if (strcmp(name, f->name) == 0) {
            return f;
        }
    }

    // TODO: Should technically throw NoSuchFieldError.
    fprintf(stderr,
        "Could not find field %s in class %s. Aborting.\n",
        name, clazz->name);
    abort();
}

const std::pair<JavaMethodInfo*,int32_t>
TryGetJavaMethodInfo(jclass cls, const char* name, const char* sig) {
    auto* clazz = classes.at(cls);
    auto* methods = clazz->methods;
    for (int32_t i = 0, e = clazz->num_methods; i < e; ++i) {
        auto* m = &methods[i];
        if (strcmp(name, m->name) == 0 && strcmp(sig, m->sig) == 0) {
	  return std::pair<JavaMethodInfo*,int32_t>(m,i);
        }
    }

    // Recurse to super class.
    if (auto super_ptr = clazz->super_ptr) {
        // TODO: Technically might not want to recurse for 'private' methods.
        return TryGetJavaMethodInfo(*super_ptr, name, sig);
    }
    return std::pair<JavaMethodInfo*,int32_t>(nullptr,-1);
}

const std::pair<JavaMethodInfo*,int32_t>
GetJavaMethodInfo(jclass cls, const char* name, const char* sig) {
  auto res = TryGetJavaMethodInfo(cls, name, sig);
    if (res.first) {
        return res;
    } else {
        // TODO: Should technically throw NoSuchMethodError.
        fprintf(stderr,
            "Could not find method %s%s in class %s. Aborting\n",
            name, sig, GetJavaClassInfo(cls)->name);
        abort();
    }
}
