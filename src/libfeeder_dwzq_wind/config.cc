#include <mutex>
#include <thread>

#include <x/x.h>
#include "yaml-cpp/yaml.h"
#include "config.h"

namespace co {

    Config* Config::instance_ = 0;

    Config* Config::Instance() {
        static std::once_flag flag;
        std::call_once(flag, [&]() {
            if (instance_ == 0) {
                instance_ = new Config();
                instance_->Init();
            }
            });
        return instance_;
    }

    void Config::Init() {
        // 读取配置
        auto getStr = [&](const YAML::Node& node, const std::string& name) {
            try {
                return node[name] && !node[name].IsNull() ? node[name].as<std::string>() : "";
            } catch (std::exception& e) {
                LOG_ERROR << "load configuration failed: name = " << name << ", error = " << e.what();
                throw std::runtime_error(e.what());
            }
        };
        auto getStrings = [&](std::vector<std::string>* ret, const YAML::Node& node, const std::string& name, bool drop_empty = false) {
            try {
                if (node[name] && !node[name].IsNull()) {
                    for (auto item : node[name]) {
                        std::string s = x::Trim(item.as<std::string>());
                        if (!drop_empty || !s.empty()) {
                            ret->emplace_back(s);
                        }
                    }
                }
            } catch (std::exception& e) {
                LOG_ERROR << "load configuration failed: name = " << name << ", error = " << e.what();
                throw std::runtime_error(e.what());
            }
        };
        auto getInt = [&](const YAML::Node& node, const std::string& name, const int64_t& default_value = 0) {
            try {
                return node[name] && !node[name].IsNull() ? node[name].as<int64_t>() : default_value;
            } catch (std::exception& e) {
                LOG_ERROR << "load configuration failed: name = " << name << ", error = " << e.what();
                throw std::runtime_error(e.what());
            }
        };
        auto getBool = [&](const YAML::Node& node, const std::string& name) {
            try {
                return node[name] && !node[name].IsNull() ? node[name].as<bool>() : false;
            } catch (std::exception& e) {
                LOG_ERROR << "load configuration failed: name = " << name << ", error = " << e.what();
                throw std::runtime_error(e.what());
            }
        };

        auto filename = x::FindFile("feeder.yaml");
        YAML::Node root = YAML::LoadFile(filename);
        opt_ = QOptions::Load(filename);

        auto wind_feeder = root["wind"];
        server_host_ = getStr(wind_feeder, "server_host");
        server_port_ = getInt(wind_feeder,"server_port");
        username_ = getStr(wind_feeder,"username");
        password_ = getStr(wind_feeder, "password");
        password_ = DecodePassword(password_);

        double_flag_ = getInt(wind_feeder,"double_flag");
        server_host_1_ = getStr(wind_feeder,"server_host_1");
        server_port_1_ = getInt(wind_feeder,"server_port_1");
        username_1_ = getStr(wind_feeder,"username_1");
        password_1_ = getStr(wind_feeder,"password_1");
        password_1_ = DecodePassword(password_1_);
        sub_market_ = getStr(wind_feeder,"sub_market");

        string dates_str = getStr(wind_feeder,"sub_date_list");
        sub_date_list_.clear();
        vector<string> items;
        x::Split(&items, dates_str);
        for (auto item : items) {
            int64_t date = x::ToInt64(item);
            if (date == 0) {
                date = x::RawDate();
            }
            sub_date_list_.push_back(date);
        }
        if (opt_->sub_date() > 0 && sub_date_list_.size() > 0) {
            string error = "sub_date conflict: feeder.sub_date and wind.sub_date_list should not be set at the same time";
            LOG_ERROR << error;
            throw std::invalid_argument(error);
        }
        stringstream ss_dates;
        for (size_t i = 0; i < sub_date_list_.size(); ++i) {
            if (i > 0) {
                ss_dates << ",";
            }
            ss_dates << sub_date_list_[i];
        }
        string sub_date_text = ss_dates.str();
        stringstream ss;
        ss << "+-------------------- configuration begin --------------------+" << endl;
        ss << opt_->ToString() << endl;
        ss << endl;
        ss << "wind:" << endl
            << "  server_host: " << server_host_ << endl
            << "  server_port: " << server_port_ << endl
            << "  username: " << username_ << endl
            << "  password: " << string(password_.size(), '*') << endl
            << "  double_flag: " << double_flag_ << endl
            << "  server_host_1: " << server_host_1_ << endl
            << "  server_port_1: " << server_port_1_ << endl
            << "  username_1: " << username_1_ << endl
            << "  password_1: " << string(password_1_.size(), '*') << endl
            << "  sub_market: " << sub_market_ << endl
            << "  sub_date_list: [" << sub_date_text << "]" << endl;
        ss << "+-------------------- configuration end   --------------------+";
        LOG_INFO << endl << ss.str();
    }
}