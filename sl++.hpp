#pragma once

#include <net.hpp>
#include <processes.hpp>
#include <efiles.hpp>
#include <std.hpp>
#include <logger.hpp>
#include <osmaker.hpp>
#include <autoconsole.hpp>

namespace slpp
{
    namespace net = ::net;
    namespace processes = ::processes;
    namespace efiles = ::efiles;
    namespace std = ::std;
    namespace osmaker = ::osmaker;
    namespace cppforge = ::cppforge;
    
    inline void log(
        std::string t, std::string l
    ) {
        std::string path = "logs/";
        std::filesystem::create_directories(path);

        efiles::append(path + "app.log", "[" + l + "]" + t);
    }
}
