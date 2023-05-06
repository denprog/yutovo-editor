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
    uint solve_delay = 2000; //milliseconds

    //caret
    uint caret_blink_delay = 500; //milliseconds

    //undo
    uint undo_size = 100; //count of undo groups
};

}

#endif
