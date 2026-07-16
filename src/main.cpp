#include <string>
#include <print>

#include <CLI/CLI.hpp>

#include <rz/json/json.hpp>
#include <rz/json/saveable.hpp>
#include <rz/fs/fs.hpp>

#include "options.hpp"

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
        auto* createCmd = app.add_subcommand("create", "Create A New CMake Project");
        std::string_view name;
        createCmd->add_option("name", name, "Name of the project to create.")->required();
        createCmd->callback([&name]() {

        });
    }

    {
        auto* setCmd = app.add_subcommand("set", "Change A Boiler Config Setting");
        std::string_view name, value;
        setCmd->add_option("name", name, "Setting Name")->required();
        setCmd->add_option("value", value, "Value To Be Set")->required();
    }

    CLI11_PARSE(app, argc, argv);
    return 0;
}
