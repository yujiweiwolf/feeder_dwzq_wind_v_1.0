#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include "x/x.h"
#include "coral/coral.h"
#include "feeder/feeder.h"

#include "TDFAPI.h"
#include "TDFAPIStruct.h"

namespace co {

    string ToUTF8(const string& msg);
    string StdCode2Wind(string std_code);

	string GetWindError(TDF_ERR code);
	int8_t WindMarket2Std(const string& wind_market);
	int8_t WindBsFlag2Std(const int& bs_flag);
	int8_t WindOrderType2Std(const char& wind_order_type);
	int8_t WindStatus2Std(const int& wind_status, const int64_t& upper_limit, const int64_t& lower_limit, const int64_t& bp1, const int64_t& ap1);

	string ParseQStock(TDF_MARKET_DATA* p);
	string ParseQIndex(TDF_INDEX_DATA* p);
	string ParseQFuture(TDF_FUTURE_DATA* p);
	string ParseQOption(QContextPtr& ctx, TDF_FUTURE_DATA* p, TDF_OPTION_CODE* info);

	bool IsShCode(string wind_code);
	bool ParseQOrder(co::fbs::QOrderT* q, TDF_ORDER* p);
	bool ParseQKnock(co::fbs::QKnockT* q,TDF_TRANSACTION* p);
	bool ParseQQueue(co::fbs::QQueueT* q,TDF_ORDER_QUEUE* p);

    static std::ofstream fout_;
    static bool close_csv_flag_ = false;
    static int tick_num_ = 0;
}