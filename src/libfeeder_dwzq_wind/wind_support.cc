#include "wind_support.h"

#include "flatbuffers/flatbuffers.h"
#include "feeder/feeder.h"
#include "define.h"

namespace co {

    string ToUTF8(const string& msg) {
        try {
            return x::GBKToUTF8(msg);
        } catch (std::exception & e) {
            return msg;
        }
    }

    string StdCode2Wind(string std_code) {
		static map<string, string> d = {
			{"000001.SH", "999999.SH"},
			{"000002.SH", "999998.SH"},
			{"000003.SH", "999997.SH"},
			{"000004.SH", "999996.SH"},
			{"000005.SH", "999995.SH"},
			{"000006.SH", "999994.SH"},
			{"000007.SH", "999993.SH"},
			{"000008.SH", "999992.SH"},
			{"000010.SH", "999991.SH"},
			{"000011.SH", "999990.SH"},
			{"000012.SH", "999989.SH"},
			{"000013.SH", "999988.SH"},
			{"000016.SH", "999987.SH"},
			{"000015.SH", "999986.SH"} };
        string wind_code = std_code;
        // 转换后缀
        // 后缀映射关系：万德->标准: SH->SH, SZ->SZ, CF->CFFEX
        if (wind_code.length() >= 3) {
            int pos = (int)wind_code.find(".CFFEX", wind_code.length() - 6);
            if (pos > 0) {
                wind_code = wind_code.substr(0, wind_code.length() - 6);
            }
        }
        // 转换指数代码
        map<string, string>::iterator itr = d.find(std_code);
        if (itr != d.end()) {
            wind_code = itr->second;
        } else {
            if (std_code.length() == 9 && std_code.substr(0, 3) == "000" && std_code.substr(6, 3) == co::kSuffixSH) {
                // 000XXX.SH - > 990XXX.SH
                wind_code = "990" + std_code.substr(3, 6);
            }
        }
        return wind_code;
    }

	string GetWindError(TDF_ERR code) {
		map<TDF_ERR, string> d;
		d[TDF_ERR_UNKOWN] = "TDF_ERR_UNKOWN";
		d[TDF_ERR_INITIALIZE_FAILURE] = "TDF_ERR_INITIALIZE_FAILURE";
		d[TDF_ERR_NETWORK_ERROR] = "TDF_ERR_NETWORK_ERROR";
		d[TDF_ERR_INVALID_PARAMS] = "TDF_ERR_INVALID_PARAMS";
		d[TDF_ERR_VERIFY_FAILURE] = "TDF_ERR_VERIFY_FAILURE";
		d[TDF_ERR_NO_AUTHORIZED_MARKET] = "TDF_ERR_NO_AUTHORIZED_MARKET";
		d[TDF_ERR_NO_CODE_TABLE] = "TDF_ERR_NO_CODE_TABLE";
		d[TDF_ERR_SUCCESS] = "TDF_ERR_SUCCESS";
		map<TDF_ERR, string>::iterator itr = d.find(code);
		return itr != d.end() ? itr->second : "TDF_ERR_UNKOWN";
	}

	int8_t WindMarket2Std(const string& tdf_market) {
		// 内部市场代码， 0-未知 1-上海 2-深圳 3-上海B 4-深圳B 5-中金所 6-上期所 7-大商所 8-郑商所 9-特转A 10-特转B, 11-三板
		// 宏汇市场代码：[SH]上海，[SZ]深圳，[CF]中金所，[SHF]上期所，[CZC]郑商所，[DCE]大商所
		static map<string, int8_t> d = { {"SH", 1},{"SH-1-1", 1},{"SH-2-0", 1},{"SZ", 2},{"SZ-1-1", 2},{"SZ-2-0", 2},{"BJ-1-0", 3},{"CF", 5},{"SHF", 6},{"CZC", 8},{"DCE", 7}};
		map<string, int8_t>::iterator itr = d.find(tdf_market);
		int ret = itr != d.end() ? itr->second : 0;
		if (ret == 0) {
			LOG_ERROR << "unknown wind_market: " << tdf_market;
			xthrow() << "unknown wind_market: " << tdf_market;
		}
		return ret;
	}

	int8_t WindBsFlag2Std(const int& bs_flag) {
		// 内部买卖方向：1-买，2-卖，3-申购，4-赎回，-1-撤单
		// 深交所买卖方向：(买：'B', 卖：'A', 不明：' ')
		// chFunctionCode（上交所原始字段为OrderBSFlag）:B-买，S-卖
		int ret = 0;
		switch (bs_flag) {
		case (int)'B':
			ret = kBsFlagBuy;
			break;
		case (int)'S':
		case (int)'A':
			ret = kBsFlagSell;
			break;
		case (int)'C':
			ret = kBsFlagWithdraw;
			break;
		default:
			break;
		}
		return ret;
	}

	int8_t WindOrderType2Std(const char& wind_order_type) {
		// 内部委托类型：0-限价单，1-市价单，2-本方最优市价单
		// 深交所委托类型：0-限价单，1-市价单，U-本方最优
        // 上交所委托类型：'A'-新增限价单；'D'-撤单
		int8_t std_order_type = 0;
		switch (wind_order_type) {
		case '1': // 深交所
			std_order_type = kQOrderTypeMarket;
			break;
		case 'U': // 深交所
			std_order_type = kQOrderTypeSelfMarket;
			break;
			case 'D': // 上交所
			std_order_type = kQOrderTypeWithdraw;
		    break;
		default:
			break;
		}
		return std_order_type;
	}

	int8_t WindStatus2Std(
        const int& wind_status,
		const int64_t& upper_limit,
		const int64_t& lower_limit,
		const int64_t& bp1,
		const int64_t& ap1) {
		//0   未设置
		//'Y' 新产品
		//'R' 交易间，禁止任何交易活动
		//'P' 休市，例如，午餐休市。无撮合市场内部信息披露。
		//'B' 停牌
		//'C' 收市
		//'D' 停牌
		//'Z' 产品待删除
		//'V' 结束交易
		//'T' 固定价格集合竞价
		//'W' 暂停，除了自有订单和交易的查询之外，任何交易活动都被禁止。
		//'X' 停牌,( 'X'和'W'的区别在于 'X'时可以撤单)
		//'I' 盘中集合竞价。
		//'N' 盘中集合竞价订单溥 平衡
		//'L' 盘中集合竞价PreOBB
		//'I' 开市集合竞价
		//'M' 开市集合况竞价OBB
		//'K' 开市集合竞价订单溥平衡(OBB)前期时段
		//'S' 非交易服务支持
		//'U' 盘后处理
		//'F' 盘前处理
		//'E' 启动
		//'O' 连续撮合
		//'Q' 连续交易和集合竞价的波动性中断
		// 内部状态，0:正常，1：停牌，2：涨停，3：跌停，4：熔断
		int8_t std_status = co::kStateOK;
		const char c = (const char)wind_status;
		switch (c) {
		case 'B':
		case 'D':
		case 'W':
		case 'X':
			std_status = kStateSuspension; // 停牌
			break;
		case 'Q':
			std_status = kStateBreak; // 熔断
			break;
		default:
            if (upper_limit > 0 && bp1 >= upper_limit) {
                std_status = co::kStateLimitUp;
            } else if (lower_limit > 0 && ap1 > 0 && ap1 <= lower_limit) {
                std_status = co::kStateLimitDown;
            }
			break;
		}
		return std_status;
	}

	string ParseQStock(TDF_MARKET_DATA* p) {
		string wind_code = p->szWindCode;
		QContextPtr ctx = QServer::Instance()->GetContext(wind_code);
		if (!ctx) { // 有的股票在代码表中不存在，需要创建，比如：H06886.SH
			string std_code = WindCode2Std(wind_code);
			ctx = QServer::Instance()->NewContext(wind_code, std_code);
		}
		co::fbs::QTickT& m = ctx->PrepareQTick();
		if (m.dtype <= 0) { // dtype没有设置说明静态数据需要初始化
			m.dtype = kDTypeStock;
			// m.name // 正常流程是在代码表接收时创建Context并填写，这里无法获取股票名称
			m.src = kSrcQTickLevel2; // 数据源
			m.pre_close = i2f(p->nPreClose); // 昨收价
			m.upper_limit = i2f(p->nHighLimited); // 涨停价
			m.lower_limit = i2f(p->nLowLimited); // 跌停价
			// m.pre_settle = 0; // 昨结算
			// m.pre_open_interest = 0; // 昨持仓量
			// m.multiple = 0; // 合约乘数
			// m.price_step = 0; // 最小报价单位
			// m.create_date = 0; // 创建日
			// m.list_date = 0; // 上市日，第一个交易日
			// m.expire_date = 0; // 到期日，最后一个交易日
			// m.start_settle_date = 0; // 开始交割日
			// m.end_settle_date = 0; // 结束交割日
			// m.exercise_date = 0; // 行权日期
			// m.exercise_price = 0; // 期权行权价
			// m.cp_flag = 0; // 期权认购认沽标记：1-认购，2-认沽
			// m.underlying_code = ""; // 标的证券代码
		}
		if (m.pre_close <= 0 && p->nPreClose > 0) {
			m.pre_close = i2f(p->nPreClose);
		}
		if (m.upper_limit <= 0 && p->nHighLimited > 0) {
			m.upper_limit = i2f(p->nHighLimited);
		}
		if (m.lower_limit <= 0 && p->nLowLimited > 0) {
			m.lower_limit = i2f(p->nLowLimited);
		}
		// --------------------------------------------------
		int64_t stamp = p->nTime % 1000000000LL;
		m.timestamp = p->nTradingDay * 1000000000LL + stamp;
		int64_t pre_sum_volume = m.sum_volume;
		double pre_sum_amount = m.sum_amount;
		m.sum_volume = p->iVolume;
		m.sum_amount = (double)p->iTurnover;
		m.new_price = i2f(p->nMatch);
		m.new_volume = m.sum_volume - pre_sum_volume;
		m.new_amount = m.sum_amount - pre_sum_amount;
		m.open = i2f(p->nOpen);
		m.high = i2f(p->nHigh);
		m.low = i2f(p->nLow);
		m.avg_bid_price = i2f(p->nWeightedAvgBidPrice);
		m.avg_ask_price = i2f(p->nWeightedAvgAskPrice);
		// m.open_interest = 0;
		// m.close = 0;
		// m.settle = 0;
		//m.new_knock_count = p->nNumTrades;
		//m.sum_knock_count += m.new_knock_count;
		for (int i = 0; i < 10; ++i) {
		    if ((p->nBidPrice[i] > 0 && p->nBidVol[i] > 0) || (i == 1 && p->nBidVol[i] > 0)) {
				m.bp.emplace_back(i2f(p->nBidPrice[i]));
				m.bv.emplace_back(p->nBidVol[i]);
			} else {
				break;
			}
		}
		for (int i = 0; i < 10; ++i) {
		    if ((p->nAskPrice[i] > 0 && p->nAskVol[i] > 0) || (i == 1 && p->nAskVol[i] > 0)) {
				m.ap.emplace_back(i2f(p->nAskPrice[i]));
				m.av.emplace_back(p->nAskVol[i]);
			} else {
				break;
			}
		}
		// -------------------------------------------------------------------------
		int8_t status = WindStatus2Std(p->nStatus, p->nHighLimited, p->nLowLimited, p->nBidPrice[0], p->nAskPrice[0]);
		// 修复行情停牌状态，满足如下条件则认为停牌：行情时间>=9:25:00，成交量=0，买一量=0，卖一量=0，将状态置为停牌。
		if (status == kStateOK && stamp >= kFixSecurityStatusBeginTime && m.sum_volume <= 0 && m.bv.empty() && m.av.empty()) {
			const string& code = m.code;
			if (code.length() == 9) { // 000002.SZ, 600000.SH
				const char c1 = code.at(0);
				const char c2 = code.at(1);
				switch (c1) {
				case '6':
					status = kStateSuspension;
					break;
				case '0':
				case '3':
					if (c2 == '0') { // 00, 30 开头
						status = kStateSuspension;
					}
					break;
				default:
					break;
				}
			}
		}
		m.status = status;
		string raw = ctx->FinishQTick();
        {
            tick_num_++;
            if (!close_csv_flag_) {
                if (!fout_.is_open()) {
                    string csv_file = "./log/dongwu_static_data_" + to_string(x::RawDate()) + ".csv";
                    fout_.open(csv_file, std::ios::out | std::ios::trunc);
                    __info << "create csv file: " << csv_file;
                    string csv_head = "code,name,upper_limit,lower_limit\n";
                    fout_.write(csv_head.data(), csv_head.size());
                    fout_.flush();
                }
                string wind_code = p->szWindCode;
                QContextPtr ctx = QServer::Instance()->GetContext(wind_code);
                if (ctx) {
                    std::stringstream ss;
                    ss << wind_code
                       << "," << m.name
                       << "," << m.upper_limit
                       << "," << m.lower_limit
                       << "\n";
                    std::string line = ss.str();
                    fout_.write(line.data(), line.size());
                    fout_.flush();
                    int64_t stamp = m.timestamp % 1000000000LL;
                    if (stamp > 91500000 && tick_num_ > 100000) {
                        close_csv_flag_ = true;
                        fout_.close();
                        __info << "close csv file, code: " << m.code << ", " << m.timestamp;
                    }
                }
            }
        }
		return raw;
	}

	string ParseQIndex(TDF_INDEX_DATA* p) {
		string wind_code = p->szWindCode;
		QContextPtr ctx = QServer::Instance()->GetContext(wind_code);
		if (!ctx) {
			string std_code = WindCode2Std(wind_code);
			ctx = QServer::Instance()->NewContext(wind_code, std_code);
		}
		co::fbs::QTickT& m = ctx->PrepareQTick();
		if (m.dtype <= 0) {  // dtype没有设置说明静态数据需要初始化
			string name = p->pCodeInfo->chName;
            name = x::Trim(ToUTF8(name));
			m.dtype = kDTypeIndex;
			m.name = name;
			m.src = kSrcQTickLevel2; // 数据源
			m.pre_close = i2f(p->nPreCloseIndex); // 昨收价
		}
		if (m.pre_close <= 0 && p->nPreCloseIndex > 0) {
			m.pre_close = i2f(p->nPreCloseIndex);
		}
		// --------------------------------------------------
		int64_t stamp = p->nTime % 1000000000LL;
		m.timestamp = p->nTradingDay * 1000000000LL + stamp;
		int64_t pre_sum_volume = m.sum_volume;
		double pre_sum_amount = m.sum_amount;
		m.sum_volume = p->iTotalVolume;
		m.sum_amount = (double)p->iTurnover * 100.0; // 宏汇的指数成交金额比真实值缩小了100倍
		m.new_price = i2f(p->nLastIndex);
		m.new_volume = m.sum_volume - pre_sum_volume;
		m.new_amount = m.sum_amount - pre_sum_amount;
		m.open = i2f(p->nOpenIndex);
		m.high = i2f(p->nHighIndex);
		m.low = i2f(p->nLowIndex);
		string raw = ctx->FinishQTick();
		return raw;
	}

	string ParseQFuture(TDF_FUTURE_DATA * p) {
		return "";
	}

	string ParseQOption(QContextPtr& ctx, TDF_FUTURE_DATA * p, TDF_OPTION_CODE* info) {
		string wind_code = p->szWindCode;
		co::fbs::QTickT& m = ctx->PrepareQTick();
		if (m.dtype <= 0 && info) { // dtype没有设置说明静态数据需要初始化
			// -----------------------------------------------------------
			m.dtype = kDTypeOption;
			// m.name // 正常流程是在代码表接收时创建Context并填写，这里无法获取股票名称
			m.src = kSrcQTickLevel2; // 数据源
			m.pre_close = i2f(p->nPreClose); // 昨收价
			m.upper_limit = i2f(p->nHighLimited); // 涨停价
			m.lower_limit = i2f(p->nLowLimited); // 跌停价
			m.pre_settle = i2f(p->nPreSettlePrice); // 昨结算
			m.pre_open_interest = p->iPreOpenInterest; // 昨持仓量
            m.price_step = 0.0001;
			// m.multiple = 0; // 合约乘数
			// m.price_step = 0; // 最小报价单位
			// m.create_date = 0; // 创建日
			// m.list_date = 0; // 上市日，第一个交易日
			// m.expire_date = 0; // 到期日，最后一个交易日
			// m.start_settle_date = 0; // 开始交割日
			// m.end_settle_date = 0; // 结束交割日
			// m.exercise_date = 0; // 行权日期
			// m.exercise_price = 0; // 期权行权价
			// m.cp_flag = 0; // 期权认购认沽标记：1-认购，2-认沽
			// m.underlying_code = ""; // 标的证券代码
			// 注意：如果进行历史行情回放，对于已经退市的期权，基本代码表中的某些字段可能是空的，比如到期日、行权价、标的证券代码等。
			m.multiple = info->nContractMultiplierUnit; // 合约乘数
			m.list_date = info->nStartDate;
			m.expire_date = info->nExpireDate;
			m.exercise_date = info->nExerciseDate;
			// 注意：标的物除权导致期权的行权价发生变化，回放历史行情的时候行权价已经是变化后的了，不是当天实盘获取的行权价
			m.exercise_price = i2f(info->nExercisePrice);
			m.cp_flag = info->chCallOrPut == 'C' ? 1 : 2; // 认购为“C”；认沽为“P”
			// 标的证券代码，这里将当前代码的市场后缀作为标的证券的市场后缀。
			string underlying_code = info->szUnderlyingSecurityID;
			int pos = (int)underlying_code.find(".");
			if (pos < 0) {
				pos = (int)wind_code.rfind(".");
				if (pos > 0) {
					string suffix = wind_code.substr(pos, (int)wind_code.length() - pos);
					underlying_code.append(suffix);
				}
			}
			m.underlying_code = underlying_code;
		}
		if (m.pre_close <= 0 && p->nPreClose > 0) {
			m.pre_close = i2f(p->nPreClose);
		}
		if (m.upper_limit <= 0 && p->nHighLimited > 0) {
			m.upper_limit = i2f(p->nHighLimited);
		}
		if (m.lower_limit <= 0 && p->nLowLimited > 0) {
			m.lower_limit = i2f(p->nLowLimited);
		}
		if (m.pre_settle <= 0 && p->nPreSettlePrice > 0) {
			m.pre_settle = i2f(p->nPreSettlePrice);
		}
		if (m.pre_open_interest <= 0 && p->iPreOpenInterest > 0) {
			m.pre_open_interest = p->iPreOpenInterest;
		}
		// --------------------------------------------------
		int64_t stamp = p->nTime % 1000000000LL;
		m.timestamp = p->nTradingDay * 1000000000LL + stamp;
		int64_t pre_sum_volume = m.sum_volume;
		double pre_sum_amount = m.sum_amount;
		m.sum_volume = p->iVolume;
		m.sum_amount = (double)p->iTurnover;
		m.new_price = i2f(p->nMatch);
		m.new_volume = m.sum_volume - pre_sum_volume;
		m.new_amount = m.sum_amount - pre_sum_amount;
		m.open = i2f(p->nOpen);
		m.high = i2f(p->nHigh);
		m.low = i2f(p->nLow);
		m.open_interest = p->iOpenInterest;
		m.close = i2f(p->nClose);
		m.settle = i2f(p->nSettlePrice);
		//m.new_knock_count = p->nNumTrades;
		//m.sum_knock_count += m.new_knock_count;
		for (int i = 0; i < 5; ++i) {
		    if ((p->nBidPrice[i] > 0 && p->nBidVol[i] > 0) || (i == 1 && p->nBidVol[i] > 0)) {
				m.bp.emplace_back(i2f(p->nBidPrice[i]));
				m.bv.emplace_back(p->nBidVol[i]);
			} else {
				break;
			}
		}
		for (int i = 0; i < 5; ++i) {
		    if ((p->nAskPrice[i] > 0 && p->nAskVol[i] > 0) || (i == 1 && p->nAskVol[i] > 0)) {
				m.ap.emplace_back(i2f(p->nAskPrice[i]));
				m.av.emplace_back(p->nAskVol[i]);
			} else {
				break;
			}
		}
		int status = WindStatus2Std(p->nStatus, p->nHighLimited, p->nLowLimited, p->nBidPrice[0], p->nAskPrice[0]);
		m.status = (int8_t)status;
		string raw = ctx->FinishQTick();
		return raw;
	}

	bool ParseQOrder(co::fbs::QOrderT* q, TDF_ORDER* p) {
		int64_t timestamp = p->nActionDay * 1000000000LL + p->nTime;
        std::string code = p->szWindCode;
        int8_t bs_flag = WindBsFlag2Std(p->chFunctionCode);
        int8_t order_type = WindOrderType2Std(p->chOrderKind);
        int64_t order_no = 0;

        bool is_sh = code.size() >= 3 &&
            code[code.size() - 3] == '.' &&
            code[code.size() - 2] == 'S' &&
            code[code.size() - 1] == 'H' ? true : false;
        if (is_sh) { // 沪市
            // if (p->pCodeInfo && p->pCodeInfo->nMarketID == ID_MARKET_SH) {
            // chOrderKind（交易所原始字段为OrderType）: A-新增委托订单；D-删除委托订单
            order_no = p->nOrderOriNo;
        } else { // 深市
            order_no = p->nOrder;
        }

        q->timestamp = timestamp;
        q->code = code;
        q->bs_flag = bs_flag;
        q->order_type = order_type;
        q->order_no = order_no;
        q->order_price = p->nPrice;
        q->order_volume = p->nVolume;
        q->recv_time = x::RawDateTime();

		return true;
	}

	bool ParseQQueue(co::fbs::QQueueT* q, TDF_ORDER_QUEUE * p) {
		int64_t timestamp = p->nActionDay * 1000000000LL + p->nTime;
		int8_t bs_flag = WindBsFlag2Std(p->nSide);

		int64_t sum_volume = 0;
		q->volumes.resize(0);
		q->volumes.reserve(p->nABItems);
		for (int i = 0; i < p->nABItems; ++i) {
			q->volumes.emplace_back(p->nABVolume[i]);
			sum_volume += p->nABVolume[i];
		}

		q->timestamp = timestamp;
		q->code = p->szWindCode;
		q->bs_flag = bs_flag;
		q->price = p->nPrice;
		q->volume = sum_volume;
		q->recv_time = x::RawDateTime();
		return true;
	}

	bool ParseQKnock(co::fbs::QKnockT* q, TDF_TRANSACTION * p) {
		int64_t timestamp = p->nActionDay * 1000000000LL + p->nTime;
		// 买卖方向：0-未知（沪市集合竞价普通成交），1-主买成交，2-主卖成交，-1-撤单成交
		int8_t bs_flag = WindBsFlag2Std(p->nBSFlag);
		if (bs_flag == 0) {
			if ((p->nBidOrder > 0 && p->nAskOrder <= 0) || (p->nBidOrder <= 0 && p->nAskOrder > 0)) {
				bs_flag = kBsFlagWithdraw;
			}
		}
		q->timestamp = timestamp;
		q->code = p->szWindCode;
		q->bs_flag = bs_flag;
		q->match_no = p->nIndex;
		q->bid_order_no = p->nBidOrder;
		q->ask_order_no = p->nAskOrder;
		q->match_price = p->nPrice;
		q->match_volume = p->nVolume;
		q->match_amount = (double)p->nTurnover;// 宏汇结构体里面直接用int存储真实值
		q->recv_time = x::RawDateTime();

		return true;
	}

}
