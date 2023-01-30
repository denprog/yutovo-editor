#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>

namespace yutovo
{

class Logger;

class Config
{
public:
    Config();

    bool Open(const std::string& file_name);

public:
    std::string service_ip = "localhost";
    uint service_port = 8010;
    uint service_timeout = 10; //seconds

private:
    Logger* logger;
};

}

#endif
