#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <mutex>
#include <thread>
#include <boost/thread.hpp>

#include "x/x.h"
#include "coral/coral.h"

#include "feeder/feeder.h"

#include "define.h"
#include "config.h"
#include "wind_support.h"

using namespace std;

namespace co {

	/**
	* 万德宏汇TDF行情服务器
	* @author Guangxu Pan, bajizhh@gmail.com
	* @since 2014-05-10 13:01:15
	* @version 2019-05-03 11:11:35
	*/
	class WindServer {
	public:

		static WindServer* Instance();

		bool Reconnect();
		void Start(const int64_t& date);
		void Stop();
		void Join();

	protected:
		WindServer();

		static void OnData(THANDLE hd, TDF_MSG* pMsg);
		static void OnMsg(THANDLE hd, TDF_MSG* pMsg);
		static void GetCodeTable(THANDLE hd, string wind_market);

		void Init();
		void Run();

	private:
        static WindServer* instance_;

		static bool running_;
        std::shared_ptr<boost::thread> thread_;

        string sub_market_;

        THANDLE tdf_handle_;
        TDF_OPEN_SETTING_EXT tdf_settings_;
	};
}