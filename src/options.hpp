#pragma once
#include <nlohmann/json.hpp>
#include <string>

struct CMake_Options
{
    std::string version;
    std::string cxx_compiler;
    std::string c_compiler;
    std::string default_src_dir;
    bool export_compile = false;
    int cxx_std = -1;
    int c_std = -1;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CMake_Options,
    version, cxx_compiler, c_compiler, default_src_dir,
    export_compile, cxx_std, c_std);
