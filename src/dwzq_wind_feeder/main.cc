#include <iostream>
#include <sstream>
#include <stdio.h>
#include <boost/program_options.hpp>
#include <x/x.h>

#include "../libfeeder_dwzq_wind/libfeeder_dwzq_wind.h"

using namespace std;
using namespace co;
namespace po = boost::program_options;

int main(int argc, char* argv[]) {
	try {
        po::options_description desc("[co_stock_quotation_svr] Usage");
        desc.add_options()
			("passwd", po::value<std::string>(), "encode plain password")
            ("help,h", "show help message")
            ("version,v", "show version information");
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        if (vm.count("passwd")) {
			cout << co::EncodePassword(vm["passwd"].as<std::string>()) << endl;
			return 0;
		} else if (vm.count("help")) {
            cout << desc << endl;
            return 0;
        } else if (vm.count("version")) {
            cout << kVersion << endl;
            return 0;
        }
		LOG_INFO << "start Wind feeder: version = " << kVersion << " ......";
		QOptionsPtr opt = Config::Instance()->opt();
		QServer::Instance()->Init(opt);
		vector<int64_t>& dates = Config::Instance()->sub_date_list();
		if (dates.empty()) {
			WindServer::Instance()->Start(opt->sub_date());
			WindServer::Instance()->Join();
			WindServer::Instance()->Stop();
		} else {
			for (size_t i = 0; i < dates.size(); ++i) {
				int64_t date = dates[i];
				WindServer::Instance()->Start(date);
				WindServer::Instance()->Join();
				WindServer::Instance()->Stop();
			}
			LOG_INFO << "all data of " << dates.size() << " dates have received over, now quit.";
		}
        LOG_INFO << "server is Stopped.";
	} catch (x::Exception& e) {
		LOG_FATAL << "server is crashed, " << e.what();
		return 1;
	} catch (std::exception& e) {
        LOG_FATAL << "server is crashed, " << e.what();
		return 2;
    } catch (...) {
        LOG_FATAL << "server is crashed, unknown reason";
		return 3;
    }
	return 0;
}

