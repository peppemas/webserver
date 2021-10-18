#include <iostream>
#include <csignal>

#include "httplib.h"
#include "popl.hpp"

using namespace httplib;
using namespace popl;

Server *svr;

std::string dump_headers(const Headers &headers) {
    std::string s;
    char buf[BUFSIZ];

    for (const auto & x : headers) {
        snprintf(buf, sizeof(buf), "%s: %s\n", x.first.c_str(), x.second.c_str());
        s += buf;
    }

    return s;
}

void stop_handler(sig_atomic_t s) {
    std::cout <<"Stopping server..." << std::endl;
    if (svr && svr->is_valid()) {
        svr->stop();
    }
    std::cout << "done." << std::endl;
    exit(0);
}

int main(int argc, char **argv) {
    std::string host;
    std::string mount_directory;
    int port;

    //============================================================================================
    // setup signal handler (Ctrl+C)
    //============================================================================================
    signal(SIGINT, stop_handler);

    //============================================================================================
    // setup options
    //============================================================================================
    OptionParser op("Allowed options");
    auto help_option     = op.add<Switch>("", "help", "produce help message");
    auto host_option   = op.add<Implicit<std::string>>("h", "host", "the host to listen to", "localhost");
    auto port_option = op.add<Implicit<int>>("p", "port", "the port number", 8080);
    auto directory_option = op.add<Implicit<std::string>>("d", "directory", "base directory to host", "./");
    port_option->assign_to(&port);
    host_option->assign_to(&host);
    directory_option->assign_to(&mount_directory);

    //============================================================================================
    // parse options
    //============================================================================================
    try {
        op.parse(argc, argv);
        if (help_option->count() == 1) std::cout << op << std::endl;
    } catch (const popl::invalid_option& e) {
        std::cerr << "Invalid Option Exception: " << e.what() << "\n";
        std::cerr << "error:  ";
        return -1;
    } catch (const std::exception& e) {
        //std::cerr << "option: " << e.option()->name(e.what_name()) << "\n";
        //std::cerr << "value:  " << e.value() << "\n";
        return -1;
    }

    //============================================================================================
    // initialize the server
    //============================================================================================
    svr = new Server();
    if (!svr->is_valid()) {
        std::cout << "Cannot start WebServer" << std::endl;
        return -1;
    }

    //============================================================================================
    // set directory mountpoint
    //============================================================================================
    auto ret = svr->set_mount_point("/", mount_directory);
    if (!ret) {
        std::cerr << "Cannot mount the directory " << mount_directory;
        return -1;
    }

    //============================================================================================
    // set post routing handler
    //============================================================================================
    svr->set_post_routing_handler([](const auto& req, auto& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
    });

    //============================================================================================
    // start listening
    //============================================================================================
    std::cout << "WebServer listening at " << host << ":" << port << std::endl;
    svr->listen(host.c_str(), port);

    return 0;
}
