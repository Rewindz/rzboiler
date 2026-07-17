#include <exception>
#include <string>
#include <print>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <CLI/CLI.hpp>

#include <rz/json/json.hpp>
#include <rz/json/saveable.hpp>
#include <rz/fs/fs.hpp>
#include <rz/string/utils.hpp>

#include "options.hpp"
#include "rz/defs/defs.hpp"

void logger(const std::string& info)
{
    std::println("{}", info);
}

int main(int argc, char **argv)
{
    CLI::App app{"CMake Project Boilerplate Generator"};

    auto configPath = rz::fs::GetAppConfigPath("rzboiler", logger).value_or(std::filesystem::path(".")) / "boiler.json";

    rz::json::Saveable<CMake_Options> savedOptions(configPath, 4, logger);
    if(savedOptions.Load() != rz::STATUS::RZ_SUCCESS)
        savedOptions.Save();

    app.require_subcommand();

    {
        auto* createCmd = app.add_subcommand("create", "Create a new CMake project (in this directory)");
        std::string_view name;
        bool executable, slib;
        executable = slib = false;
        createCmd->add_option("name", name, "Name of the project to create.")->required();
        auto* execFlag = createCmd->add_flag("-e,--executable", executable, "Add 'add_executable({PROJ_NAME})' to the CMakeLists");
        auto* slibFlag = createCmd->add_flag("-s,--static", slib, "Add 'add_library({PROJ_NAME} STATIC)' to the CMakeLists");
        execFlag->excludes(slibFlag);
        createCmd->callback([&]() {
            using namespace std::filesystem;
            path projPath = name;
            if(exists(projPath)){
                std::println("Path {} already exists!", name);
                return;
            }

            if(!create_directory(projPath)){
                std::println("Failed to create project directory {}!", name);
                return;
            }

            std::ofstream cmakelists(projPath / "CMakeLists.txt");
            if(cmakelists.is_open()) {
                cmakelists << std::format("cmake_minimum_required(VERSION {})\n", savedOptions->version);

                if(!savedOptions->cxx_compiler.empty())
                    cmakelists << std::format("set(CMAKE_CXX_COMPILER {})\n", savedOptions->cxx_compiler);
                if(!savedOptions->c_compiler.empty())
                    cmakelists << std::format("set(CMAKE_C_COMPILER {})\n", savedOptions->c_compiler);

                if(savedOptions->cxx_std > 0)
                    cmakelists << std::format("set(CMAKE_CXX_STANDARD {})\n", savedOptions->cxx_std);
                if(savedOptions->c_std > 0)
                    cmakelists << std::format("set(CMAKE_C_STANDARD {})\n", savedOptions->c_std);

                if(savedOptions->export_compile)
                    cmakelists << "set(CMAKE_EXPORT_COMPILE_COMMANDS ON)\n";

                cmakelists << std::format("project({})\n\n\n", name);

                if(executable)
                    cmakelists << std::format("add_executable({})\n", name);
                if(slib)
                    cmakelists << std::format("add_library({} STATIC)\n", name);

            }

        });
    }

    {
        auto* listOptionsCmd = app.add_subcommand("list", "List the available config option names");
        listOptionsCmd->callback([](){
            std::println("version            string      The cmake_minimum_required version number");
            std::println("cxx_compiler       string      The C++ compiler to use");
            std::println("c_compiler         string      The C compiler to use");
            std::println("default_src_dir    string      Default source directory");
            std::println("cxx_std            int         The C++ standard to use");
            std::println("c_std              int         The C standard to use");
            std::println("export_compile     bool        Export compile_commands.json");
        });
    }

    {
        auto *configPathCmd = app.add_subcommand("cfg", "Prints the path to the boiler config file");
        configPathCmd->callback([&configPath]() {
            std::println("{}", configPath.string());
        });
    }

    {
        auto* viewConfigCmd = app.add_subcommand("view", "Prints the current saved config");
        viewConfigCmd->callback([&savedOptions]() {
            nlohmann::json j = *savedOptions;
            std::cout << j.dump(4);
        });
    }

    {
        auto* setCmd = app.add_subcommand("set", "Change a boiler config setting");
        std::string_view name, value;
        setCmd->add_option("name", name, "Setting name")->required();
        setCmd->add_option("value", value, std::format("Value to be set (Find these with ./{} list)", argv[0]))->required();
        setCmd->callback([&savedOptions, &name, &value]() {
            using rz::string::iequal_many;
            if(name == "version")
                savedOptions->version = value;
            else if(name == "cxx_compiler")
                savedOptions->cxx_compiler = value;
            else if(name == "c_compiler")
                savedOptions->c_compiler = value;
            else if(name == "default_src_dir")
                savedOptions->default_src_dir = value;
            else if(name == "export_compile") {
                if(iequal_many(value, {"t", "true", "1"}))
                    savedOptions->export_compile = true;
                else if (iequal_many(value, {"f", "false", "0"}))
                    savedOptions->export_compile = false;
                else {
                    std::println("The value {} is not valid for {}!", value, name);
                    return;
                }
            }
            else if(name == "cxx_std") {
                // TODO: maybe check for valid verson numbers?
                try {
                    savedOptions->cxx_std = std::stoi(std::string(value));;
                } catch (const std::exception& e) {
                    std::println("{} is not a valid number!", value);
                    return;
                }
            }
            else if(name == "c_std") {
                // TODO: maybe check for valid verson numbers?
                try{
                    savedOptions->c_std = std::stoi(std::string(value));;
                } catch (const std::exception& e) {
                    std::println("{} is not a valid number!", value);
                    return;
                }
            }
            else {
                std::println("{} is not a valid option name!", name);
                return;
            }
            savedOptions.Save();
        });
    }

    CLI11_PARSE(app, argc, argv);
    return 0;
}
