#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>

namespace yutovo
{

typedef unsigned int uint;

struct Config
{
    //solver
    std::string service_ip = "localhost";
    uint service_port = 8010;
    uint service_timeout = 10; //seconds

    //caret
    uint caret_blink_delay = 500;

    //undo
    uint undo_size = 100; //count of undo groups
};

}

#endif
