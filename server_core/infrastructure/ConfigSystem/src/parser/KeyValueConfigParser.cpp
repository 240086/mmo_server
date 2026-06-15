// server_core/infrastructure/ConfigSystem/src/parser/JsonConfigParser.cpp
#include <mmo/infrastructure/ConfigSystem/parser/KeyValueConfigParser.hpp>
#include <fstream>

namespace mmo::infrastructure::config
{
    ParseResult KeyValueConfigParser::ParseFile(
        const std::filesystem::path &path,
        std::unordered_map<ConfigKey, ConfigValue> &out)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            // 🌟 完美对齐落盘的富错误向量契约
            return {false, {ParseError{"Cannot open target file", path.string(), 0}}};
        }

        std::string line;
        int lineNum = 0;
        while (std::getline(file, line))
        {
            lineNum++;
            if (line.empty() || line[0] == '#')
                continue; // 跳过空行和注释

            auto pos = line.find('=');
            if (pos == std::string::npos)
                continue;

            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            // V1 Baseline 默认以 string 灌入，后续由 rapidjson 负责强类型转换
            out[key] = value;
        }

        return {true, {}}; // 成功返回空错误集
    }
}