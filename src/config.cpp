#include "config.h"
#include "logger.h"
#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/stringbuffer.h"
#include <rapidjson/istreamwrapper.h>
#include <fstream>

namespace yutovo
{

//Config

Config::Config() :
    logger(Logger::GetInstance(".", "yutovo", true, true))
{
}

bool Config::Open(const std::string& file_name)
{
    std::ifstream ifs{file_name};
    if (!ifs.is_open())
    {
        logger->Error("Config file not open: {}", file_name);
        return false;
    }

    rapidjson::IStreamWrapper isw{ifs};
    rapidjson::Document doc;
    doc.ParseStream(isw);

    if (doc.HasParseError())
    {
        logger->Error("Config file parse error: {}", file_name);
        return false;
    }

    if (doc.HasMember("service_ip") && doc["service_ip"].IsString())
        service_ip = doc["service_ip"].GetString();
    if (doc.HasMember("service_port") && doc["service_port"].IsInt())
        service_port = doc["service_port"].GetInt();
    if (doc.HasMember("service_timeout") && doc["service_timeout"].IsInt())
        service_timeout = doc["service_timeout"].GetInt();
}

}
