#include "wind_server.h"

#include "define.h"


namespace co {

    WindServer* WindServer::instance_ = nullptr;

    bool WindServer::running_ = true;

    WindServer* WindServer::Instance() {
        static std::once_flag flag;
        std::call_once(flag, [&]() {
            if (instance_ == 0) {
                instance_ = new WindServer();
            }
        });
        return instance_;
    }

    WindServer::WindServer():
        tdf_handle_(nullptr) {

    }

    void WindServer::Init() {
        string server_host = Config::Instance()->server_host();
        string server_port = std::to_string(Config::Instance()->server_port());
        string username = Config::Instance()->username();
        string password = Config::Instance()->password();

        int double_flag = Config::Instance()->double_flag();
        string server_host_1 = Config::Instance()->server_host_1();
        string server_port_1 = std::to_string(Config::Instance()->server_port_1());
        string username_1 = Config::Instance()->username_1();
        string password_1 = Config::Instance()->password_1();

        auto opt = Config::Instance()->opt();
        int64_t sub_date = opt->sub_date();
        int64_t sub_time = opt->sub_time();
        sub_market_ = Config::Instance()->sub_market();

        TDF_SetEnv(TDF_ENVIRON_HEART_BEAT_INTERVAL, 10); // 心跳间隔，秒数（如果为0，则设为1）
        TDF_SetEnv(TDF_ENVIRON_MISSED_BEART_COUNT, 100); // 丢失心跳次数视作掉线
        TDF_SetEnv(TDF_ENVIRON_OPEN_TIME_OUT, 30); // 开始连接时候超时时间，秒数（如果为0，则设为1）

        memset(&tdf_settings_, 0, sizeof(tdf_settings_));

        if (double_flag == 0) {
            tdf_settings_.nServerNum = 1;
            strncpy(tdf_settings_.siServer[0].szIp, server_host.data(), server_host.size());
            strncpy(tdf_settings_.siServer[0].szPort, server_port.data(), server_port.size());
            strncpy(tdf_settings_.siServer[0].szUser, username.data(), username.size());
            strncpy(tdf_settings_.siServer[0].szPwd, password.data(), password.size());
            __info << "单活接入模式.";
        } else {
            tdf_settings_.nServerNum = 2;
            strncpy(tdf_settings_.siServer[0].szIp, server_host.data(), server_host.size());
            strncpy(tdf_settings_.siServer[0].szPort, server_port.data(), server_port.size());
            strncpy(tdf_settings_.siServer[0].szUser, username.data(), username.size());
            strncpy(tdf_settings_.siServer[0].szPwd, password.data(), password.size());

            strncpy(tdf_settings_.siServer[1].szIp, server_host_1.data(), server_host_1.size());
            strncpy(tdf_settings_.siServer[1].szPort, server_port_1.data(), server_port_1.size());
            strncpy(tdf_settings_.siServer[1].szUser, username_1.data(), username_1.size());
            strncpy(tdf_settings_.siServer[1].szPwd, password_1.data(), password_1.size());
            __info << "双活接入模式.";
        }
        tdf_settings_.pfnMsgHandler = (TDF_DataMsgHandler)&WindServer::OnData;
        tdf_settings_.pfnSysMsgNotify = (TDF_SystemMsgHandler)&WindServer::OnMsg;
        // #市场列表，不用区分大小写，用英文字符 ; 分割；市场订阅和代码订阅不能同时为空；订阅格式为：市场简称-数据level：SH-2;SZ-2;CF-2;SHF-2;CZC-2;DCE-2;
        if (!sub_market_.empty()) {
            tdf_settings_.szMarkets = sub_market_.c_str();
        }
//        if (sub_date != 0) {
//            LOG_ERROR << "unsupported date: " << sub_date << ", TDFAPI 3.0 do not support sub history data";
//            xthrow() << "unsupported date: " << sub_date << ", TDFAPI 3.0 do not support sub history data";
//        }
        tdf_settings_.nTime = sub_time;
        // --------------------------------------------------------------------------------
        // #数据类型订阅，支持订阅3种类型TRANSACTION;ORDER;ORDERQUEUE。 ！注意：行情数据任何时候都发送，不需要订阅!
        // 为0请求所有品种，或者取值为DATA_TYPE_FLAG中多种类别，比如DATA_TYPE_MARKET | DATA_TYPE_TRANSACTION
        tdf_settings_.nTypeFlags = 0;
        if (opt->enable_order()) {
            tdf_settings_.nTypeFlags |= DATA_TYPE_ORDER;
        }
        if (opt->enable_knock()) {
            tdf_settings_.nTypeFlags |= DATA_TYPE_TRANSACTION;
        }
        if (opt->enable_queue()) {
            tdf_settings_.nTypeFlags |= DATA_TYPE_ORDERQUEUE;
        }
    }

    bool WindServer::Reconnect() {
        TDF_ERR rc = TDF_ERR_NETWORK_ERROR;
        //while (rc == TDF_ERR_NETWORK_ERROR || rc == TDF_ERR_VERIFY_FAILURE) { // 只有在发生网络错误的情况下才应该重连
        string server_host = Config::Instance()->server_host();
        string server_port = std::to_string(Config::Instance()->server_port());
        string username = Config::Instance()->username();
        string password = Config::Instance()->password();
        while (true) { // 只有在发生网络错误的情况下才应该重连
            LOG_INFO << "connecting to TDF server: " << server_host << ":" << server_port << " ......";
            if (tdf_handle_) {
                TDF_Close(tdf_handle_);
                tdf_handle_ = 0;
            }
            tdf_handle_ = TDF_OpenExt(&tdf_settings_, &rc);
            if (tdf_handle_) {
                LOG_INFO << "connect to TDF server ok";
                break;
            }
            LOG_ERROR << "connect to TDF server failed: " << GetWindError(rc) << ", retry after 10 seconds ......";
            x::Sleep(10000);
        }
        return rc == TDF_ERR_SUCCESS;
    }

    void WindServer::Run() {
        Init();
        bool ok = Reconnect();
        if (!ok) {
            return;
        }
        QServer::Instance()->Wait();
        QServer::Instance()->Stop();
        running_ = false;
    }

    void WindServer::Start(const int64_t& date) {
        LOG_INFO << "start receiving: sub_date = " << date << " ...";
        running_ = true;
        QServer::Instance()->Start();
        thread_ = std::make_shared<boost::thread>(boost::bind(&WindServer::Run, this));
    }

    void WindServer::Stop() {
        LOG_INFO << "stop receiving ...";
        running_ = false;
        if (tdf_handle_) {
            TDF_Close(tdf_handle_);
            tdf_handle_ = 0;
        }
        if (thread_) {
            thread_->join();
        }
        QServer::Instance()->Stop();
        LOG_INFO << "stop receiving ok";
    }

    void WindServer::Join() {
        if (thread_) {
            thread_->join();
        }
     }

    void WindServer::GetCodeTable(THANDLE hd, string wind_market) {
        LOG_INFO << "get code table: wind_market = " << wind_market << " ......";
        //int64_t market = (int64_t)WindMarket2Std(wind_market);
        TDF_CODE* pCodeTable;  //获取代码表 
        unsigned int nItems;
        TDF_GetCodeTable(hd, wind_market.c_str(), &pCodeTable, &nItems);
        for (int j = 0; j < (int)nItems; ++j) {
            TDF_CODE& tdf_code = pCodeTable[j];
            int32_t type = tdf_code.nType;
            string wind_code = tdf_code.szWindCode;
            string std_code = WindCode2Std(wind_code);
            string name = x::GBKToUTF8(x::Trim(tdf_code.szCNName));
            QContextPtr ctx = QServer::Instance()->GetContext(wind_code);
            if (!ctx) {
                ctx = QServer::Instance()->NewContext(wind_code, std_code);
            }
            co::fbs::QTickT& static_tick = ctx->static_tick();  // 将万德的信息保存到static_tick中
            static_tick.underlying_code = wind_market;
            static_tick.code = wind_code;
            co::fbs::QTickT& tick = ctx->tick();
            tick.name = name;
            bool is_index = ((type & 0xF0) == 0x00) ? true : false;
            if (is_index) {
                // 对于指数代码，代码表中的代码为真实代码（000300.SH， 000001.SH），但是行情中的代码有两种情况：
                // 行情中的有的指数代码为真实代码，例如：代码表中：000300.SH，行情中：000300.SH
                // 行情中的有的指数代码为万德代码，例如：代码表中：000001.SH，行情中：999999.SH
                string real_wind_code = StdCode2Wind(std_code);
                if (real_wind_code != wind_code) {
                    QContextPtr ctx2 = QServer::Instance()->GetContext(real_wind_code);
                    if (!ctx2) {
                        QServer::Instance()->SetContext(real_wind_code, ctx);
                    }
                }
            }
        }
        //  xdebug() << ss.str();
        TDF_FreeArr(pCodeTable);
        LOG_INFO << "get code table ok: wind_market = " << wind_market << ", size = " << nItems;
    }

    void WindServer::OnData(THANDLE hd, TDF_MSG* pMsg) {
        if (!pMsg->pData) {
            return;
        }
        if (pMsg->pAppHead->nItemCount <= 0) {
            return;
        }
        //int64_t begin_ms = x::Timestamp();
        switch(pMsg->nDataType) {
        case MSG_DATA_MARKET: { // 行情数据
            if (!QServer::Instance()->options()->enable_spot()) {
                return;
            }
            if (pMsg->pAppHead->nItemSize != sizeof(TDF_MARKET_DATA)) {
                LOG_WARN << "parse TDF_MARKET_DATA failed: pMsg->pAppHead->nItemSize != sizeof(TDF_MARKET_DATA)"
                    << ", pMsg->pAppHead->nItemSize = " << pMsg->pAppHead->nItemSize
                    << ", sizeof(TDF_MARKET_DATA) = " << sizeof(TDF_MARKET_DATA);
                return;
            }
            TDF_MARKET_DATA* pData = (TDF_MARKET_DATA*)pMsg->pData;
            int64_t last_stamp = 0;
            for (int i = 0; i < pMsg->pAppHead->nItemCount; ++i) {
                TDF_MARKET_DATA* p = pData+i;
                string raw = ParseQStock(p);
                if (!raw.empty()) {
                    QServer::Instance()->PushQTick(raw);
                    last_stamp = p->nTime;
                }
            }
//            // 对所有股票进行停牌状态修正
//            if (!QServer::Instance()->is_status_fixed() && last_stamp > 0 && last_stamp >= kFixAllSecurityStatusTime) {
//                QServer::Instance()->FixAllSecurityStatus();
//                QServer::Instance()->set_is_status_fixed(true);
//            }
            break;
        }
        case MSG_DATA_INDEX: { // 指数数据
            if (!QServer::Instance()->options()->enable_spot()) {
                return;
            }
            if (pMsg->pAppHead->nItemSize != sizeof(TDF_INDEX_DATA)) {
                LOG_WARN << "parse TDF_INDEX_DATA failed: pMsg->pAppHead->nItemSize != sizeof(TDF_INDEX_DATA)"
                << ", pMsg->pAppHead->nItemSize = " << pMsg->pAppHead->nItemSize
                    << ", sizeof(TDF_INDEX_DATA) = " << sizeof(TDF_INDEX_DATA);
                return;
            }
            TDF_INDEX_DATA* pData = (TDF_INDEX_DATA*)pMsg->pData;
            for (int i = 0; i < pMsg->pAppHead->nItemCount; ++i) {
                TDF_INDEX_DATA* p = pData+i;
                string raw = ParseQIndex(p);
                if (!raw.empty()) {
                    QServer::Instance()->PushQTick(raw);
                }
            }
            break;
        }
        case MSG_DATA_FUTURE: { // 期货、期权行情
            if (!QServer::Instance()->options()->enable_option()) {
                return;
            }
            if (pMsg->pAppHead->nItemSize != sizeof(TDF_FUTURE_DATA)) {
                LOG_WARN << "parse TDF_FUTURE_DATA failed: pMsg->pAppHead->nItemSize != sizeof(TDF_FUTURE_DATA)"
                    << ", pMsg->pAppHead->nItemSize = " << pMsg->pAppHead->nItemSize
                    << ", sizeof(TDF_FUTURE_DATA) = " << sizeof(TDF_FUTURE_DATA);
                return;
            }
            TDF_FUTURE_DATA* pData = (TDF_FUTURE_DATA*)pMsg->pData;
            for (int i = 0; i < pMsg->pAppHead->nItemCount; ++i) {
                TDF_FUTURE_DATA* p = pData+i;
                if (strlen(p->szWindCode) != 11) { // 期权代码有11位：10001501.SH
                    continue;
                }
                string wind_code = p->szWindCode;
                QContextPtr ctx = QServer::Instance()->GetContext(wind_code);
                if (!ctx) {
                    if (!QServer::Instance()->is_contracts_received()) {
                        GetCodeTable(hd, "SH-1-1");
                        GetCodeTable(hd, "SZ-1-1");
                        QServer::Instance()->set_is_contracts_received(true);
                        ctx = QServer::Instance()->GetContext(wind_code);
                        if (!ctx) {
                            LOG_WARN << "no such option in code table: " << wind_code;
                        }
                    }
                    if (!ctx) {
                        string std_code = WindCode2Std(wind_code);
                        ctx = QServer::Instance()->NewContext(wind_code, std_code);
                        string outer_market;
                        switch (ctx->tick().market) {
                        case co::kMarketSH:
                            outer_market = "SH-1-1";
                            break;
                        case co::kMarketSZ:
                            outer_market = "SZ-1-1";
                            break;
                        default:
                            break;
                        }
                    }
                }
                co::fbs::QTickT& tick = ctx->tick();
                if (tick.dtype <= 0) { // 静态信息未初始化
                    TDF_OPTION_CODE info;
                    memset(&info, 0, sizeof(info));
                    co::fbs::QTickT& static_tick = ctx->static_tick();
                    string wind_market = static_tick.underlying_code;
                    int rc = TDF_GetOptionCodeInfo(hd, p->szWindCode, &info, wind_market.c_str());
                    if (rc != TDF_ERR_SUCCESS) {
                        LOG_WARN << "TDF_GetOptionCodeInfo failed: wind_code = " << wind_code << ", wind_market = " << wind_market;
                        continue;
                    }
                    string raw = ParseQOption(ctx, p, &info);
                    if (!raw.empty()) {
                        QServer::Instance()->PushQTick(raw);
                    }
                } else { // 静态信息已初始化，只需要解析动态字段即可
                    string raw = ParseQOption(ctx, p, nullptr);
                    if (!raw.empty()) {
                        QServer::Instance()->PushQTick(raw);
                    }
                }
            }
            break;
        }
        case MSG_DATA_ORDER: { // 逐笔委托
            if (!QServer::Instance()->options()->enable_order()) {
                return;
            }
            if (pMsg->pAppHead->nItemSize != sizeof(TDF_ORDER)) {
                LOG_WARN << "parse TDF_ORDER failed: pMsg->pAppHead->nItemSize != sizeof(TDF_ORDER)"
                    << ", pMsg->pAppHead->nItemSize = " << pMsg->pAppHead->nItemSize
                    << ", sizeof(TDF_ORDER) = " << sizeof(TDF_ORDER);
                return;
            }
            TDF_ORDER* pData = (TDF_ORDER*)pMsg->pData;
            for (int i = 0; i < pMsg->pAppHead->nItemCount; ++i) {
                TDF_ORDER* p = pData+i;
                auto q = QServer::Instance()->GetQOrder();
                bool ok = ParseQOrder(q, p);
                if (ok) {
                    QServer::Instance()->Push(q);
                }
            }
            break;
        }
        case MSG_DATA_ORDERQUEUE: { // 委托队列
            if (!QServer::Instance()->options()->enable_queue()) {
                return;
            }
            if (pMsg->pAppHead->nItemSize != sizeof(TDF_ORDER_QUEUE)) {
                LOG_WARN << "parse TDF_ORDER_QUEUE failed: pMsg->pAppHead->nItemSize != sizeof(TDF_ORDER_QUEUE)"
                    << ", pMsg->pAppHead->nItemSize = " << pMsg->pAppHead->nItemSize
                    << ", sizeof(TDF_ORDER_QUEUE) = " << sizeof(TDF_ORDER_QUEUE);
                return;
            }
            TDF_ORDER_QUEUE* pData = (TDF_ORDER_QUEUE*)pMsg->pData;
            for (int i = 0; i < pMsg->pAppHead->nItemCount; ++i) {
                TDF_ORDER_QUEUE* p = pData+i;
                auto q = QServer::Instance()->GetQQueue();
                bool ok = ParseQQueue(q, p);
                if (ok) {
                    QServer::Instance()->Push(q);
                }
            }
            break;
        }
        case MSG_DATA_TRANSACTION: { // 逐笔成交
            if (!QServer::Instance()->options()->enable_knock()) {
                return;
            }
            if (pMsg->pAppHead->nItemSize != sizeof(TDF_TRANSACTION)) {
                LOG_WARN << "parse TDF_TRANSACTION failed: pMsg->pAppHead->nItemSize != sizeof(TDF_TRANSACTION)"
                    << ", pMsg->pAppHead->nItemSize = " << pMsg->pAppHead->nItemSize
                    << ", sizeof(TDF_TRANSACTION) = " << sizeof(TDF_TRANSACTION);
                return;
            }
            TDF_TRANSACTION* pData = (TDF_TRANSACTION*)pMsg->pData;
            for (int i = 0; i < pMsg->pAppHead->nItemCount; ++i) {
                TDF_TRANSACTION* p = pData+i;
                auto q = QServer::Instance()->GetQKnock();
                bool ok = ParseQKnock(q, p);
                if (ok) {
                    QServer::Instance()->Push(q);
                }
            }
            break;
        }
        default:
            //LOG_WARN << "unknown data type received: " << pMsg->nDataType;
            break;
            return;
        }
    }

    void WindServer::OnMsg(THANDLE hd, TDF_MSG* pMsg) {
        if (!hd || !pMsg) {
            return;
        }
        switch (pMsg->nDataType) {
        case MSG_SYS_DISCONNECT_NETWORK: {
            LOG_ERROR << "[SYS] connection is broken";
            break;
        }
        case MSG_SYS_CONNECT_RESULT: {
            TDF_CONNECT_RESULT* pData = (TDF_CONNECT_RESULT*)pMsg->pData;
            if (pData && pData->nConnResult) {
                LOG_INFO << "[SYS] connection is established";
            } else {
                LOG_ERROR << "[SYS] connect to server failed.";
            }
            int64_t total = QServer::Instance()->stats()->GetTotalRawRows();
            // 如果连接建立后发现已经接收到数据，说明是盘中自动重连，这种情况非常危险，不允许将行情广播出去，必须退出。
            if (total > 0) { 
                LOG_FATAL << total << " records received already, forbidden Reconnecting on trading time, now quit.";
                exit(-1);
            }
            break;
        }           
        case MSG_SYS_LOGIN_RESULT: {
            TDF_LOGIN_RESULT* pData = (TDF_LOGIN_RESULT*)pMsg->pData;
            if (pData && pData->nLoginResult) {
                bool ok = true;
                int64_t sub_date = QServer::Instance()->options()->sub_date();
                string sub_market = Config::Instance()->sub_market();
                stringstream ss;
                ss << "[SYS] login ok:";
                for (int i = 0; i < pData->nMarkets; ++i) {
                    string wind_market_text = string(pData->szMarket[i]);
                    ss << endl << "[" << (i+1) << "/" << pData->nMarkets << "] market: " << wind_market_text << ", date: " << pData->nDynDate[i];
                    // 如果是订阅今天的数据，这里才进行检查。因为如果是订阅历史数据，登录后这里的日期仍然是最新的交易日期。
                    // 订阅历史数据的日期检查在代码表接收处进行检查。
                    bool is_sub = sub_market.empty() || (int)sub_market.find(wind_market_text) >= 0 ? true : false;
                    if (is_sub) {
                        if (sub_date == x::RawDate() && pData->nDynDate[i] != sub_date) {
                            ok = false;
                        }
                    } else {
                        ss << " (not sub market, ignored)";
                    }
                }
                LOG_INFO << ss.str();
                if (!ok) {
                    bool check = true;
                    if (check) {
                        LOG_INFO << "quotation date check failed, reconnect after 5s ......";
                        x::Sleep(5000);
                        WindServer::Instance()->Reconnect();
                    } else {
                        LOG_WARN << "quotation date check failed, continue receiving because of enable_date_check = false.";
                    }
                }
            } else {
                LOG_ERROR << "login failed: " << ToUTF8(pData->szInfo);
            }
            break;
        }
        case MSG_SYS_CODETABLE_RESULT: {
            if (QServer::Instance()->is_contracts_received()) { // 回放海通历史数据会接收到很多次代码表消息。已经接收到代码表，忽略
                LOG_WARN << "multi code table received";
                return;
            }
            TDF_CODE_RESULT* pData = (TDF_CODE_RESULT*)pMsg->pData;
                if (pData) {
                    bool ok = true;
                    int sub_date = QServer::Instance()->options()->sub_date();
                    std::stringstream ss;
                    ss << "[SYS] code table received:";
                    for (int i = 0; i < pData->nMarkets; ++i) {
                        ss << endl << (i+1) << " of " << pData->nMarkets << " markets: " 
                           << pData->szMarket[i] << ", codes size: " << pData->nCodeCount[i] << ", date: " << pData->nCodeDate[i];
                        std::string wind_market = pData->szMarket[i];
                        int64_t std_market = WindMarket2Std(wind_market);
                        if (std_market != co::kMarketSH && std_market != co::kMarketSZ) {
                            // 忽略非沪深交易所的代码表
                            continue;
                        }
                        if (pData->nCodeDate[i] != sub_date) {
                            ok = false;
                        }
                        GetCodeTable(hd, wind_market);
                    }
                    LOG_INFO << ss.str();
                    if (ok) {
                        QServer::Instance()->set_is_contracts_received(true);
                    }
                    if (!ok) {
                        bool check = true;
                        if (check) {
                            LOG_WARN << "quotation date check failed, Reconnect after 2s ......";
                            x::Sleep(2000);
                            WindServer::Instance()->Reconnect();
                        } else {
                            LOG_WARN << "quotation date check failed, continue receiving because of enable_date_check = false.";
                        }
                    }
                }
            break;
        }
        case MSG_SYS_QUOTATIONDATE_CHANGE: {
            TDF_QUOTATIONDATE_CHANGE* pData = (TDF_QUOTATIONDATE_CHANGE*)pMsg->pData;
            if (pData) {
                LOG_INFO << "[SYS] quotation date has changed, need Reconnect: market=" << pData->szMarket << ", old_date=" << pData->nOldDate << ", new_date=" << pData->nNewDate;
            }
            break;
        }
        case MSG_SYS_MARKET_CLOSE: {
            TDF_MARKET_CLOSE* pData = (TDF_MARKET_CLOSE*)pMsg->pData;
            if (pData) {
                LOG_INFO << "[SYS] market has closed, market=" << pData->szMarket << ", time=" << pData->nTime << ", info=" <<pData->chInfo;
                // 上海和深圳各有一条闭市消息，在接收到上海的闭市消息时表示上海的数据已回访结束，但是后面可能还有深圳的数据未接收完，需要继续接收。
                // 在回放历史数据的时候，有的股票已经回放到收盘了，但是有的股票还只回放到上午，这时候可能就会收到一条闭市消息。
                //is_market_closed_ = true;
                //running_ = false;
            }
            break;
        } 
        case MSG_SYS_HEART_BEAT: {
            LOG_INFO << "[SYS] heartbeat received";
            break;
        }
        default: // 忽略未知消息，转码机会发送一些其他数据。
            // LOG_WARN << "[SYS] unknown sys message received: nDataType=" << pMsg->nDataType;
            break;
        }
    }
}