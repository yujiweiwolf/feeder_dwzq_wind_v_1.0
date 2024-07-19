#pragma once

namespace co {

	const string kVersion = "v4.1.14";

	// 每收到一条股票行情数据，执行如下检查：行情时间>=9:25:00，成交量=0，买一量=0，卖一量=0，将状态置为停牌。
	constexpr int kFixSecurityStatusBeginTime = 92500000;
	// 当收到第一条时间>=9:26:00.000的股票行情数据时，执行如下操作：
	// 检查当前接收到的所有股票行情，如果行情原始股票状态为非停牌，行情时间 >= 9:26，买一量 = 0，卖一量 = 0，成交量 = 0，则将状态置为停牌
	constexpr int kFixAllSecurityStatusTime = 92600000;

}
