#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include "feeder/feeder.h"

namespace co {
    using namespace std;

    class Config {
    public:
        static Config* Instance();

        string server_host() {
            return server_host_;
        }
        int server_port() {
            return server_port_;
        }
        string username() {
            return username_;
        }
        string password() {
            return password_;
        }

        int double_flag() {
            return double_flag_;
        }

        string server_host_1() {
            return server_host_1_;
        }
        int server_port_1() {
            return server_port_1_;
        }
        string username_1() {
            return username_1_;
        }
        string password_1() {
            return password_1_;
        }
        string sub_market() {
            return sub_market_;
        }
        vector<int64_t>& sub_date_list() {
            return sub_date_list_;
        }
        QOptionsPtr opt() {
            return opt_;
        }

    protected:
        Config() = default;
        ~Config() = default;
        Config(const Config&) = delete;
        const Config& operator=(const Config&) = delete;

        void Init();

    private:
        static Config* instance_;

        QOptionsPtr opt_;

        string server_host_;
        int server_port_;
        string username_;
        string password_;

        int double_flag_ = 0;
        string server_host_1_;
        int server_port_1_;
        string username_1_;
        string password_1_;
        string sub_market_;
        vector<int64_t> sub_date_list_;
    };
}