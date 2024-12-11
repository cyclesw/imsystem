#include <iostream>
#include <string>

#include "common/logger.h"

int main(int argc, char** argv)
{
    using namespace ns_log;

    Logger::InitLogger(false, LogLevel::trace);
    int i = 0;
    std::string str = "asdas";
    LOG_INFO("who am i");

    return 0;
}
