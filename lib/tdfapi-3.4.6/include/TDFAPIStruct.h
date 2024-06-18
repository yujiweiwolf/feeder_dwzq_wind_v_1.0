#ifndef __TDF_API_STRUCT_H__ 
#define __TDF_API_STRUCT_H__
#pragma  pack(push)
#pragma pack(1)

#ifndef THANDLE
typedef void* THANDLE;
#endif

#ifndef __int64
#define __int64 long long
#endif
#define MAXSERVER 4

/******************************************************************* 消息头 *********************************************************************/
struct TDF_APP_HEAD
{
	int	nHeadSize;         //本记录结构大小
	int	nItemCount;	       //记录条数
	int	nItemSize;         //记录大小
};
struct TDF_MSG
{
	unsigned short  	    sFlags;		        //16位 标识符.
	int  	                nDataType;	        //32位 数据类型 见enum TDF_MSG_ID
	int			            nDataLen;	        //32位 数据长度（不包括TDF_APP_HEAD的长度）
	int			            nServerTime;		//32位服务器时间戳（精确到毫秒HHMMSSmmm），对于原生压缩，就是本地时间
	int     		        nOrder;		        //32位 流水号
	int                     nConnectId;         //连接ID，在TDF_Open时，配置里面设定的值
	TDF_APP_HEAD*           pAppHead;	        //应用头
	void*                   pData;              //数据指针
};
struct TDF_REFDATA_MSG
{
	int                     nReqID;
	int                     errorCode;          //==0表示成功，-1读取ETF文件失败，-2打开ETF文件失败
	int  	                nDataType;	        //32位 数据类型 见enum TDF_MSG_ID
	int			            nDataLen;	        //32位 数据长度（不包括TDF_APP_HEAD的长度）
	void*                   pData;              //数据指针，TDF_ETF_LIST_HEAD
};
struct TDF_ETF_LIST_HEAD
{
	int   nFileLength;                         //文件长度
	char  szFileName[64];                      //文件名(20161227-SZ159949.ETF)
	void* pData;                               //文件内容
};
/*******************************************************************  回调函数 *********************************************************************/
//数据回调，用于通知用户收到了行情、逐笔成交，逐笔委托，委托队列等,pMsgHead->pAppHead->ItemCount字段可以获知得到了多少条记录，pMsgHead->pAppHead->pData指向第一条数据记录
typedef void (*TDF_DataMsgHandler)  (THANDLE hTdf, TDF_MSG* pMsgHead);    
//系统消息回调，用于通知用户收到了网络断开事件、连接（重连）结果、代码表结果等。当获取系统消息时，pMsgHead->pAppHead指针为空, pMsgHead->pData指向相应的结构体
typedef void (*TDF_SystemMsgHandler)(THANDLE hTdf, TDF_MSG* pMsgHead);  
//系统消息回调，用于通知用户收到了参考数据
typedef void (*TDF_RefDataMsgHandler)(THANDLE hTdf, TDF_REFDATA_MSG* pMsgHead);  
/******************************************************************* 消息类型定义 *********************************************************************/
//消息类型对应的结构体见后面
enum TDF_MSG_ID
{
    MSG_INVALID = -100,
    //系统消息
    MSG_SYS_DISCONNECT_NETWORK,     //网络断开事件, 相应的结构体为NULL
    MSG_SYS_CONNECT_RESULT,         //主动发起连接的结果
    MSG_SYS_LOGIN_RESULT,           //登陆应答
    MSG_SYS_CODETABLE_RESULT,       //代码表结果，收到此消息后可获取对应代码表,此消息表示所有授权市场代码表可取
    MSG_SYS_QUOTATIONDATE_CHANGE,   //行情日期变更通知（已取消）
    MSG_SYS_MARKET_CLOSE,           //闭市（已取消）
    MSG_SYS_HEART_BEAT,             //服务器心跳消息, 相应的结构体为NULL
    //数据消息
    MSG_DATA_INDEX,                 //指数数据
    MSG_DATA_MARKET,                //行情数据
    MSG_DATA_FUTURE,                //期货行情
    MSG_DATA_TRANSACTION,           //逐笔成交
    MSG_DATA_ORDERQUEUE,            //委托队列
    MSG_DATA_ORDER,                 //逐笔委托
	MSG_DATA_BBQTRANSACTION,        //BBQ现券成交数据
	MSG_DATA_BBQBID,                //BBQ现券报价数据
	MSG_DATA_NON_MD,				//非行情消息
	MSG_DATA_OTC_OPTION,			//场外期权
	MSG_DATA_BROKERQUEUE,           //经纪商队列(港股)

	//参考数据消息
	MSG_REF_ETFL_LIST  = 1,			//收到参考数据(如ETF文件)

	MSG_SYS_PACK_OVER   = -10,      //当前网络包解析完毕
	//市场事件消息
	MSG_SYS_MARKET_EVENT = -9,		//市场事件通知消息(如市场清盘，转数等),收到此事件的清盘或者快照后可获取快照和期权信息

	//单个市场代码表通知.由于某些较小市场数据回调时，同时请求的其他市场才处理完代码表，行情数据较MSG_SYS_CODETABLE_RESULT先到，
	//增加该消息，通知每一市场代码表处理结果，客户可据此选择何时请求代码表
	MSG_SYS_SINGLE_CODETABLE_RESULT,//单个市场代码表结果，收到此消息后可获取对应的单个市场代码表

	MSG_SYS_QUOTEUNIT_CHANGE,		//价差变化
	MSG_SYS_FAIL_REPLAY,			//市场回放失败通知
	MSG_SYS_ADDCODE,				//新增代码，在此前代码表的基础上，有新代码
};

/******************************************************************* 系统消息 *********************************************************************/
//MSG_SYS_PACK_OVER
struct TDF_PACK_OVER
{
	int nDataNumber;
	int nConID;
};
//数据类型MSG_SYS_CONNECT_RESULT
struct TDF_CONNECT_RESULT
{
    char szIp[32];
    char szPort[8];
    char szUser[64];
    char szPwd[64];

    unsigned int nConnResult; //为0则表示连接失败，非0则表示连接成功
    int nConnectionID;        //连接ID
};
//数据类型MSG_SYS_LOGIN_RESULT
struct TDF_LOGIN_RESULT
{
    unsigned int nLoginResult;//为0则表示登陆验证失败，非0则表示验证成功

    char szInfo[256];       //登陆结果文本
    int nMarkets;           //市场个数
    char szMarket[256][8];  //市场代码 SZ-2-0, SH-2-0, SHF-1-0
    int nDynDate[256];      //动态数据日期
};
//数据类型MSG_SYS_CODETABLE_RESULT
struct TDF_CODE_RESULT
{
    char szInfo[128];       //代码表结果文本
    int nMarkets;           //市场个数
    char szMarket[256][8];  //市场代码
    int nCodeCount[256];    //代码表项数
    int nCodeDate[256];     //代码表日期
};
//数据类型MSG_SYS_SINGLE_CODETABLE_RESULT
struct TDF_SINGLE_CODE_RESULT
{
	char szMarket[8];  //市场代码
	int nCodeCount;    //代码表项数
	int nCodeDate;     //代码表日期
};
//数据类型MSG_SYS_QUOTATIONDATE_CHANGE
struct TDF_QUOTATIONDATE_CHANGE
{
    char szMarket[8];	    //市场代码
    int nOldDate;	        //原行情日期
    int nNewDate;	        //新行情日期
};
//数据类型MSG_SYS_MARKET_CLOSE
struct TDF_MARKET_CLOSE
{
    char    szMarket[8];        //交易所名称
    int		nTime;				//时间(HHMMSSmmm)
    char	chInfo[64];			//闭市信息
};


enum MARKET_EVENT_NUM
{
	ID_BREAKPOINT_RECV = -1,				//断点方式接受数据
	ID_SNAP_SHOT,							//快照
	ID_MARKET_OPEN,							//开盘
	ID_MARKET_CLEAR,						//清盘
	ID_MARKET_CLOSE,						//闭市
	ID_MARKET_TRANS,						//转数
	ID_MARKET_SAVE,							//落盘
	ID_CLEAR_CODES,							//部分代码清盘  
	ID_SINGLE_CODE_CLEAR,					//单个代码清盘
	ID_MARKET_HEART_BEAT,					//市场心跳(一分钟一次)
	ID_CODE_HEART_BEAT,						//代码心跳(无数据一分钟一次)
	ID_TRANS_CODES,							//部分代码转数
};

struct TDF_CODE;
//市场事件 MSG_SYS_MARKET_EVENT
struct TDF_MARKET_EVENT
{
	char    szMarket[12];       //市场Key
	__int64 market_time;		//市场时间，LTC时间(自19000101起到当前日期的毫秒数)
	__int64 vss_time;			//VSS本地时间，LTC时间(自19000101起到当前日期的毫秒数)
	int		nEvent;				//参看MARKET_EVENT_NUM
	int		nDate;				//市场日期
	int     nCodeSize;			//代码数(部分代码和单个代码清盘时有值)
	TDF_CODE* pCode;			//代码信息(部分代码和单个代码清盘时有值)
};

//价差变更 MSG_SYS_QUOTEUNIT_CHANGE
struct TDF_QUOTEUNIT_CHANGE
{
	char szMarket[12];
	char szWindCode[32];
};

//回放失败通知
//消息类型MSG_SYS_FAIL_REPLAY
struct TDF_FAIL_REPLAY
{
	char szMarket[12];	//市场名.SH-2-0格式
	int nDate;			//回放日期。格式YYMMDD
	char szInfo[64];	//错误信息
};

//消息类型MSG_SYS_ADDCODE
struct TDF_ADD_CODE
{
	char szMarket[8];       //market code: SHF-1-0
	int  nCodeDate;			//代码表日期
	TDF_CODE* pCode;		//代码表信息
	unsigned int nItems;	//pCode对应的代码数量
};
/******************************************************************* 代码表 *********************************************************************/
//使用TDF_GetCodeTable获取基本代码表，使用TDF_GetOptionCodeInfo获取期权代码表（期权含额外信息）
struct TDF_CODE
{
    char szWindCode[32];    //Wind Code: AG1302.SHF
    char szMarket[8];       //market code: SHF-1-0
    char szCode[32];        //原始 code:ag1302
    char szENName[32];
    char szCNName[32];      //chinese name: 沪银1302
    int nType;              //代码类型               
	char szWindType[8];		//windType : FUR(期货)
	int nRecord;			//当日编号(marketID*64*1024+record)
	int nLotSize;			//交易量纲,港股有效
};
struct TDF_OPTION_CODE
{
    TDF_CODE basicCode;
    
    char szContractID[32];			// 期权合约代码
    char szUnderlyingSecurityID[32];// 标的证券代码
    char chCallOrPut;               // 认购认沽C1        认购，则本字段为“C”；若为认沽，则本字段为“P”
    int  nExerciseDate;             // 期权行权日，YYYYMMDD    
    //扩充字段
    char chUnderlyingType;			// 标的证券类型C3    0-A股 1-ETF (EBS – ETF， ASH – A 股)
	char chOptionType;              // 欧式美式C1        若为欧式期权，则本字段为“E”；若为美式期权，则本字段为“A”
	
	char chPriceLimitType;          // 涨跌幅限制类型C1 ‘N’表示有涨跌幅限制类型, ‘R’表示无涨跌幅限制类型
	int  nContractMultiplierUnit;	// 合约单位,         经过除权除息调整后的合约单位, 一定是整数
	__int64  nExercisePrice;        // 期权行权价,       经过除权除息调整后的期权行权价，右对齐，精确到厘
	int  nStartDate;                // 期权首个交易日,YYYYMMDD
	int  nEndDate;                  // 期权最后交易日/行权日，YYYYMMDD
	int  nExpireDate;               // 期权到期日，YYYYMMDD
};
union TD_EXCODE_INFO
{
	struct TD_OptionCodeInfo             //futures options 专用 (nType >= 0x90 && nType <= 0x95),非期权下列字段无效
	{
		char chContractID[32];           // 期权合约代码C19
		char szUnderlyingSecurityID[32]; // 标的证券代码
		char chUnderlyingType;			 // 标的证券类型C3    0-A股 1-ETF (EBS – ETF， ASH – A 股)
		char chOptionType;               // 欧式美式C1        若为欧式期权，则本字段为“E”；若为美式期权，则本字段为“A”
		char chCallOrPut;                // 认购认沽C1        认购，则本字段为“C”；若为认沽，则本字段为“P”
		char chPriceLimitType;           // 涨跌幅限制类型C1 ‘N’表示有涨跌幅限制类型, ‘R’表示无涨跌幅限制类型
		int  nContractMultiplierUnit;	 // 合约单位,         经过除权除息调整后的合约单位, 一定是整数
		__int64  nExercisePrice;             // 期权行权价,       经过除权除息调整后的期权行权价，右对齐，精确到厘
		int  nStartDate;                 // 期权首个交易日,YYYYMMDD
		int  nEndDate;                   // 期权最后交易日/行权日，YYYYMMDD
		int  nExerciseDate;              // 期权行权日，YYYYMMDD
		int  nExpireDate;                // 期权到期日，YYYYMMDD
	}Option;
};
struct TDF_CODE_INFO
{
	int  nMarketID;           //交易所ID，定义见文档
	int  nDataLevel;          //数据Level
	int  nFlag;               //数据来源
	int  nType;				  //证券类型
	char chCode[32];          //证券代码(原始代码)
	char chName[64];		  //汉语证券名称
	TD_EXCODE_INFO exCodeInfo;
	char chWindType[8];		  //windType : FUR(期货),测试
	int  nLotSize;			  //交易量纲,港股有效
	char chMarket[4];		  //市场简称(如:SHF)
	int  nIsRegistration;	  //深交所有效，是否注册制, 0: 无数据; 1: 是; 2: 否;
	int  nExtType;			  //扩展类型 1.深交所上市初期
	int  nRecord;			  //当日编号
	char chWindCode[32];	  //Wind Code: AG1302.SHF
	char chMarketKey[8];      //market code: SHF-1-0
};

//交易所ID定义///////////////////////////////
#ifndef ID_MARKET_DEFINE
#define ID_MARKET_DEFINE
#define ID_MARKET_SZ	0	//深圳
#define ID_MARKET_SH	1	//上海
#define ID_MARKET_PT	2	//PT
#define ID_MARKET_HZ	3	//板块指数
#define ID_MARKET_SL	4	//自定义市场
#define ID_MARKET_MI	5	//自定义指数
#define ID_MARKET_HK	6	//港股
#define ID_MARKET_QH	7	//期货
#define ID_MARKET_FE	8	//外汇
#define ID_MARKET_KF	9	//外汇(港股)
#define ID_MARKET_CF	10	//中国金融指数

#define ID_MARKET_SHF	11	//上海金属商品交易所
#define ID_MARKET_CZC	12	//郑州商品
#define ID_MARKET_DCE	13	//大连商品
/////////////////////////////////////////////
#define ID_MARKET_CME	14  //芝加哥商业交易所
#define ID_MARKET_CBT	15  //芝加哥期货交易所
#define ID_MARKET_CMX	16  //纽约商品期货交易所
#define ID_MARKET_NYM	17  //纽约商品交易所
#define ID_MARKET_NYB	18  //纽约期货交易所
#define ID_MARKET_IPE	19	//伦敦国际石油交易所
#define ID_MARKET_LME	20	//伦敦金属交易所
#define ID_MARKET_N		21  //纽约交易所
#define ID_MARKET_O		22  //纳斯达克交易所
#define ID_MARKET_HKF	23  //香港交易所期货/期权
/////////////////////////////////////////////
#endif
/******************************************************************* 数据消息 *********************************************************************/
//数据类型MSG_DATA_BBQTRANSACTION,     
struct TDF_BBQTRANSACTION_DATA
{
	char        szWindCode[32];         //Wind Code: AG1302.SHF
	int         nActionDay;             //业务发生日(自然日)
	int			nTime;					//时间(HHMMSSmmm)
	int         nDoneID;                //成交报价编号
	__int64			nPrice;				//成交收益率(%)或价格 *10000
	char        chPriceStatus;          //收益率或价格标识	1：收益率 	2：价格
	char        chStatus;               //删除/成交
	char        chDirection;            //成交方向	1：done	2：gvn		3：tkn		4：其他
	char        chSource;               //报价机构	1：国际 2：平安利顺 3：中诚 4：天津信唐	5：国利
	char        chSpecialFlag;          //标识报价是否是行权或到期	0无标记	1备注中包含到期		2备注中包含行权
};
//数据类MSG_DATA_BBQBID,    
struct TDF_BBQBID_DATA
{
	char        szWindCode[32];         //Wind Code: AG1302.SHF 
	int         nActionDay;             //业务发生日(自然日)
	int			nTime;					//时间(HHMMSSmmm)
	char        chSource;               //报价机构

	__int64     nBidPrice;				//买入收益率或价格 x10000
	__int64     nBidVolume;				//买入量 x10000
	char        chBidPriceStatus;		//收益率或价格标识	1：收益率 	2：价格
	char        chIsBid;				//是否bid	0：不是bid（普通报价）	1：是bid（意向报价）
	char        chBidSpecialFlag;		//标识报价是否是行权或到期	0无标记	1备注中包含到期		2备注中包含行权
	char        chBidStatus;			//买入最优报价状态	0：正常报价	1：撤销报价

	__int64     nOfrPrice;				//卖出收益率或价格 x10000
	__int64     nOfrVolume;				//卖出量	所有量的加总，如2000+1000，就合并为3000；如字符串中含- - 则视为0. x10000
	char        chOfrPriceStatus;		//收益率或价格标识	1：收益率 	2：价格
	char        chIsOfr;				//是否ofr	0：不是ofr（普通报价）	  1：是ofr（意向报价）
	char        chOfrSpecialFlag;		//标识报价是否是行权或到期	0无标记	1备注中包含到期		2备注中包含行权
	char        chOfrStatus;			//卖出最优报价状态	0：正常报价	1：撤销报价
	int         nDoneID;                //成交报价编号
	char		aucInfo[420];			//买卖盘原始信息
};
//外汇交易中心债券相关信息
struct TDF_NoSSE_Bond{
	char    chBidQuoteOrg[32];   //买报价方
	char    chAskQuoteOrg[32];   //卖报价方
	int     nBidCleanPrice;      //最优买入净价
	int     nAskCleanPrice;      //最优卖出净价
	int     nBidYield;           //最优买入到期收益率
	int     nAskYield;           //最优卖出到期收益率
	__int64 iBidTradeVolume;     //最优买入报价量
	__int64 iAskTradeVolume;     //最优卖出报价量
	int     nBidTotalPrice;      //最优买入全价
	int     nAskTotalPrice;      //最优卖出全价
};

//ETF申购、赎回
struct TDF_ETF_DATA
{
	int		nETFBuyNumber;       // ETF申购笔数
	int		nETFSellNumber;      // ETF赎回笔数
	__int64 iETFBuyAmount;		 // ETF申购数量
	__int64 iETFBuyMoney;		 // ETF申购金额
	__int64 iETFSellAmount;		 // ETF赎回数量
	__int64 iETFSellMoney;		 // ETF赎回金额
};

//数据类型MSG_DATA_MARKET
struct TDF_MARKET_DATA
{
    char        szWindCode[32];         //Wind Code: AG1302.SHF
    char        szCode[32];             //原始 Code
    int         nActionDay;             //业务发生日(自然日)
    int         nTradingDay;            //交易日
    int			nTime;					//时间(HHMMSSmmm)
    int			nStatus;				//状态
    __int64 nPreClose;					//前收盘价
    __int64 nOpen;						//开盘价
    __int64 nHigh;						//最高价
    __int64 nLow;						//最低价
    __int64 nMatch;						//最新价
    __int64 nAskPrice[10];				//申卖价
    __int64 nAskVol[10];				//申卖量
    __int64 nBidPrice[10];				//申买价
    __int64 nBidVol[10];				//申买量
    int nNumTrades;						//成交笔数
    __int64		 iVolume;				//成交总量
    __int64		 iTurnover;				//成交总金额
    __int64		 nTotalBidVol;			//委托买入总量
    __int64		 nTotalAskVol;			//委托卖出总量
    __int64 nWeightedAvgBidPrice;		//加权平均委买价格
    __int64 nWeightedAvgAskPrice;		//加权平均委卖价格
    int			 nIOPV;					//IOPV净值估值
    int			 nYieldToMaturity;		//到期收益率
    __int64 nHighLimited;				//涨停价
    __int64 nLowLimited;				//跌停价
    char		 chPrefix[4];			//证券信息前缀
    int			 nSyl1;					//市盈率1
    int			 nSyl2;					//市盈率2
    int			 nSD2;					//升跌2（对比上一笔）
	TDF_NoSSE_Bond TDF_Bond;			//外汇交易中心债券相关信息
	int			 nTradeFlag;			//交易标志,具体含义参看文档
	
	__int64 iAfterPrice;    //盘后价格(科创板有使用到)
	int nAfterVolume;		//盘后量(科创板有使用到)
	__int64 iAfterTurnover;	//盘后成交金额(科创板有使用到)
	int nAfterMatchItems;   //盘后成交笔数(科创板有使用到)
	
	const TDF_CODE_INFO *  pCodeInfo;   //代码信息， TDF_Close，清盘重连后，此指针无效

	TDF_ETF_DATA TDF_ETF; 	//ETF申购赎回信息(上交所)
};
//数据类型MSG_DATA_INDEX
struct TDF_INDEX_DATA
{
    char        szWindCode[32];         //Wind Code: AG1302.SHF 
    char        szCode[32];             //原始Code
    int         nActionDay;             //业务发生日(自然日)
    int         nTradingDay;            //交易日
    int         nTime;			        //时间(HHMMSSmmm)
	int			nStatus;				//状态，20151223新增
    __int64		    nOpenIndex;		    //今开盘指数
    __int64 	    nHighIndex;		    //最高指数
    __int64 	    nLowIndex;		    //最低指数
    __int64 	    nLastIndex;		    //最新指数
    __int64	    iTotalVolume;	        //参与计算相应指数的交易数量
    __int64	    iTurnover;		        //参与计算相应指数的成交金额
    __int64		    nPreCloseIndex;	    //前盘指数
	const TDF_CODE_INFO *  pCodeInfo;   //代码信息， TDF_Close，清盘重连后，此指针无效
};
//数据类型MSG_DATA_FUTURE
struct TDF_FUTURE_DATA
{
    char        szWindCode[32];         //Wind Code: AG1302.SHF
    char        szCode[32];             //原始 Code
    int         nActionDay;             //业务发生日(自然日)
    int         nTradingDay;            //交易日
    int			 nTime;					//时间(HHMMSSmmm)	
    int			 nStatus;				//状态
    __int64		 iPreOpenInterest;		//昨持仓
    __int64 nPreClose;					//昨收盘价
    int nPreSettlePrice;		//昨结算
    __int64 nOpen;						//开盘价	
    __int64 nHigh;						//最高价
    __int64 nLow;						//最低价
    __int64 nMatch;						//最新价
    __int64		 iVolume;				//成交总量
    __int64		 iTurnover;				//成交总金额
    __int64		 iOpenInterest;			//持仓总量
    __int64 nClose;						//今收盘
    int nSettlePrice;			//今结算
    __int64 nHighLimited;				//涨停价
    __int64 nLowLimited;				//跌停价
    int			 nPreDelta;			    //昨虚实度
    int			 nCurrDelta;            //今虚实度
    __int64 nAskPrice[5];				//申卖价
    __int64 nAskVol[5];					//申卖量
    __int64 nBidPrice[5];				//申买价
    __int64 nBidVol[5];					//申买量
	//Add 20140605
	__int64	nAuctionPrice;		        //波动性中断参考价
	int	nAuctionQty;		            //波动性中断集合竞价虚拟匹配量	
	int nAvgPrice;                      //郑商所期货均价
	const TDF_CODE_INFO *  pCodeInfo;   //代码信息， TDF_Close，清盘重连后，此指针无效
	char chOriginalCode[12];			//期货原始代码 code:ag1302
};
//数据类型MSG_DATA_TRANSACTION
struct TDF_TRANSACTION
{
    char    szWindCode[32];             //Wind Code: AG1302.SHF
    char    szCode[32];                 //原始Code
    int     nActionDay;                 //自然日
    int 	nTime;		                //成交时间(HHMMSSmmm)
    int 	nIndex;		                //成交编号
    __int64		nPrice;		            //成交价格
    int 	nVolume;	                //成交数量
    __int64		nTurnover;	            //成交金额
    int     nBSFlag;                    //买卖方向(买：'B', 卖：'S', 不明：' ')
    char    chOrderKind;                //成交类别
    char    chFunctionCode;             //成交代码
    int	    nAskOrder;	                //叫卖方委托序号
    int	    nBidOrder;	                //叫买方委托序号
	int     nChannel;					//channel id
	const TDF_CODE_INFO *  pCodeInfo;   //代码信息， TDF_Close，清盘重连后，此指针无效
	__int64		nBizIndex;				//业务编号
};
//数据类型MSG_DATA_ORDER
struct TDF_ORDER
{
	char    szWindCode[32]; //Wind Code: AG1302.SHF
	char    szCode[32];     //原始Code
	int 	nActionDay;	    //委托日期(YYMMDD)
	int 	nTime;			//委托时间(HHMMSSmmm)
	int 	nOrder;	        //委托号
	__int64		nPrice;		//委托价格
	int 	nVolume;		//委托数量
	char    chOrderKind;	//委托类别
	char    chFunctionCode;	//委托代码('B','S','C')
	int     nChannel;		//channel id
	const TDF_CODE_INFO *  pCodeInfo;     //代码信息， TDF_Close，清盘重连后，此指针无效
	__int64		nOrderOriNo; //原始订单号
	__int64		nBizIndex;	 //业务编号
};
//数据类型MSG_DATA_ORDERQUEUE
struct TDF_ORDER_QUEUE
{
	char    szWindCode[32]; //Wind Code: AG1302.SHF 
	char    szCode[32];     //原始 Code
	int     nActionDay;     //自然日
	int 	nTime;			//时间(HHMMSSmmm)
	int     nSide;			//买卖方向('B':Bid 'A':Ask)
	__int64		nPrice;		//委托价格
	int 	nOrders;		//订单数量
	int 	nABItems;		//明细个数
	int 	nABVolume[200];	//订单明细
	const TDF_CODE_INFO *  pCodeInfo;     //代码信息， TDF_Close，清盘重连后，此指针无效
};

//场外期权，数据类型MSG_DATA_OTC_OPTION
struct TDF_OTC_Option{

	char  szWindCode[32];		//Wind Code: AG1302.SHF
	char  szCode[32];			//原始 Code
	char  szWindType[8];		//windType : FUR.SHF(上期所期货)

	char chOptionType;			// 欧式美式C1        若为欧式期权，则本字段为“E”；若为美式期权，则本字段为“A”
	char chCallOrPut;			// 认购认沽C1        认购，则本字段为“C”；若为认沽，则本字段为“P”
	__int64  iExercisePrice;	// 期权行权价,  

	int  nTime;					// 成交时间
	int  nVolume;				// 成交量
	__int64 iMatch;				// 成交价
	__int64 iPreSettlement;		// 前日结算价
	__int64 iSettlement;		// 结算价
	int nPrePosition;			// 前日持仓量
	int nPosition;				// 持仓量
	__int64 iAskPrice;			// 委卖价格
	__int64 iBidPrice;			// 委买价格
	int nAskVol;				// 委卖数量
	int nBidVol;				// 委买数量
	const TDF_CODE_INFO *  pCodeInfo;     //代码信息， TDF_Close，清盘重连后，此指针无效
};

/******************************************************************* 订阅相关 *********************************************************************/
enum DATA_TYPE_FLAG
{
	DATA_TYPE_TRANSACTION = 0x2,           //逐笔成交
	DATA_TYPE_ORDER       = 0x4,           //逐笔委托
	DATA_TYPE_ORDERQUEUE  = 0x8,           //委托队列 
	DATA_TYPE_NONE = 0,                    //只要行情，其他都不订阅；订阅多个类型DATA_TYPE_TRANSACTION | DATA_TYPE_ORDER | DATA_TYPE_ORDERQUEUE
};
enum SUBSCRIPTION_STYLE
{
	SUBSCRIPTION_FULL = 3,                //全市场订阅
	SUBSCRIPTION_SET=0,                   //设置为订阅列表中股票，
	SUBSCRIPTION_ADD=1,                   //增加订阅列表中股票
	SUBSCRIPTION_DEL=2,                   //删除列表中的订阅
};
enum CODE_TYPE_FLAG
{
	CODE_TYPE_ALL = 0x00,
	CODE_TYPE_INDEX = 0x01,
	CODE_TYPE_SHARES = 0x02,
	CODE_TYPE_FUND = 0x04,
	CODE_TYPE_BOND = 0x08,
	CODE_TYPE_FUTURES = 0x10,
	CODE_TYPE_OPTIONS = 0x20,
};

/******************************************************************* 配置 *********************************************************************/
//环境设置，在调用TDF_Open之前设置k
enum TDF_ENVIRON_SETTING
{
	TDF_ENVIRON_HEART_BEAT_INTERVAL,		//Heart Beat间隔（秒数）, 若值为0则表示默认值10秒钟
	TDF_ENVIRON_MISSED_BEART_COUNT,			//如果没有收到心跳次数超过这个值，且没收到其他任何数据，则判断为掉线，若值0为默认次数2次
	TDF_ENVIRON_OPEN_TIME_OUT,				//在调TDF_Open期间，接收每一个数据包的超时时间（秒数，不是TDF_Open调用总的最大等待时间），若值为0则默认30秒
	TDF_ENVIRON_USE_PACK_OVER,				//网络包结束消息
	TDF_ENVIRON_HEART_BEAT_FLAG,			//心跳数据发送方式(0: 取消发送心跳包 1：没有数据发送的时候发送心跳数据，2：有规律发送心跳数据)
	TDF_ENVIRON_SOURCE_MODE,				//双活数据源模式,值参考TDF_SOURCE_SETTING枚举类
	TDF_ENVIRON_SOURCE_MODE_VALUE,			//双活数据源模式下参数的值
	TDF_ENVIRON_OUT_LOG,					//1、当前目录下创建log，否则在当前路径有log文件夹时创建到log文件夹中
	TDF_ENVIRON_SNAPSHOT_ENENT,				//1、发送快照事件和清盘通知但快照不发送到回调函数，否则直接发送快照到回调函数，无快照通知, 测试中
	TDF_ENVIRON_ORIGINAL_VOL,				//原始结构有效，1、指数成交量和成交额的单位为股和元，默认为100股和100元
	TDF_ENVIRON_BREAKPOINT,					//1.网络断开后断点续传，else.请求最新快照
	TDF_ENVIRON_SWITCH_SAME,				//switch双活模式下有效，保证一个交易日的前收和开盘只有一个有效值
	TDF_ENVIRON_DISTRIBUTION,				//1.连接的上游是TDF分发服务，默认为0表示上游为TDFServer服务
};
//双活模式设置
enum TDF_SOURCE_SETTING
{
	TDF_SOURCE_MERGE,						//合并模式，选择数据更快的一路数据源数据
	TDF_SOURCE_SWITCH,						//延迟切换模式，收到数据间隔超过设定阈值，则切换，默认值为5000毫秒，设置通过TDF_ENVIRON_SOURCE_MODE_VALUE变量设置
};
//单服务器配置（可用多服务器实现，保留单服务器配置以兼容旧API使用方法）
//相比旧API，少了：nReconnectCount、nReconnectGap、nProtocol，nDate
struct TDF_OPEN_SETTING
{
    char szIp[32];
    char szPort[8];
    char szUser[64];
    char szPwd[64];
	//回调函数设置
	TDF_DataMsgHandler pfnMsgHandler;			//数据消息处理回调
	TDF_SystemMsgHandler pfnSysMsgNotify;		//系统消息通知回调 

	//订阅设置									--注意：订阅是市场独立的！对于每个市场必须订阅市场或代码，只订阅市场，则发送此市场全部代码。
	const char* szMarkets;						// 市场订阅(主)！例如"SZ-2-0;SH-2-0;SHF-1-0"，需要订阅的市场列表，以“;”分割
	const char* szResvMarkets;					// 市场订阅(缺)！例如"SZ-2-0;SH-2-0;SHF-1-0"，需要订阅的市场列表，以“;”分割，建议为""	
	const char* szSubScriptions;				// 代码订阅,例如"600000.sh;ag.shf;000001.sz"，需要订阅的股票(单个股票格式为原始Code+.+市场，如999999.SH)，以“;”分割，为空则订阅全市场
	unsigned int nTypeFlags;					// 数据类型订阅！支持订阅3种类型TRANSACTION;ORDER;ORDERQUEUE。 ！注意：行情数据任何时候都发送，不需要订阅! 参见enum DATA_TYPE_FLAG

	unsigned int nTime;							//为0则请求实时行情，(0xffffffff从头请求,待开发)
	unsigned int nConnectionID;					//连接ID，连接回调消息的附加结构 TDF_CONNECT_RESULT中 会包含这个ID
	unsigned int nCodeTypeFlags;				//代码类型订阅！支持订阅CODE_TYPE_FLAG表示的类型，多种类型使用'|'进行计算后填入,与nTypeFlags计算方式相同
};

//多服务器配置
struct TDF_SERVER_INFO
{
	char szIp[32];		//IP
	char szPort[8];		//端口
	char szUser[64];	//用户名
	char szPwd[64];		//密码
};
struct TDF_OPEN_SETTING_EXT
{
	//服务器设置
	TDF_SERVER_INFO	siServer[MAXSERVER];	//服务器信息
	unsigned int nServerNum;				//服务器数量
	//回调函数设置
	TDF_DataMsgHandler pfnMsgHandler;       //数据消息处理回调
	TDF_SystemMsgHandler pfnSysMsgNotify;   //系统消息通知回调
	
	//unsigned int nProtocol;               //协议号，为0则为默认，或者0x6001
	//订阅设置                              --注意：订阅是市场独立的！对于每个市场必须订阅市场或代码，只订阅市场，则发送此市场全部代码。
	const char* szMarkets;					// 市场订阅(主)！例如"SZ-2-0;SH-1-1;"，需要订阅的市场列表，以“;”分割
	const char* szResvMarkets;              // 市场订阅(缺)！例如"SZ-2-0;SH-1-1;"，需要订阅的市场列表，以“;”分割,建议为""
	const char* szSubScriptions;            // 代码订阅,例如"600000.sh;ag.shf;000001.sz"，需要订阅的股票(单个股票格式为原始Code+.+市场，如999999.SH)，以“;”分割，为空则订阅全市场
	unsigned int nTypeFlags;                // 数据类型订阅！支持订阅3种类型TRANSACTION;ORDER;ORDERQUEUE。 ！注意：行情数据任何时候都发送，不需要订阅! 参见enum DATA_TYPE_FLAG

	unsigned int nTime;                     //为0则请求实时行情，为0xffffffff从头请求
	unsigned int nConnectionID;             //连接ID，连接回调消息的附加结构 TDF_CONNECT_RESULT中 会包含这个ID，消息头也会包含该ID
	unsigned int nCodeTypeFlags;			//代码类型订阅！支持订阅CODE_TYPE_FLAG表示的类型，多种类型使用'|'进行计算后填入,与nTypeFlags计算方式相同
};
//代理服务器设置
enum TDF_PROXY_TYPE
{
	TDF_PROXY_SOCK4,
	TDF_PROXY_SOCK4A,
	TDF_PROXY_SOCK5,
	TDF_PROXY_HTTP11,
};
struct TDF_PROXY_SETTING
{
	TDF_PROXY_TYPE nProxyType;	//代理类型
	char szProxyHostIp[32];		//代理IP
	char szProxyPort[8];		//代理端口
	char szProxyUser[32];		//代理用户名
	char szProxyPwd[32];		//代理密码
};
//总包，流量，连接次数，最后一次连接时间+日期
struct ConStatInfo
{
	__int64  iTotalPacks;		//总包
	__int64  iTotalTraffic;		//总流量
	int      nToalConTimes;		//连接次数
	__int64  ilastConTime;		//最后一次连接时间+日期
};

//打开参考数据服务
struct TDF_OPEN_REFDATA_SETTING
{
	char szIp[32];
	char szPort[8];
	char szUser[64];
	char szPwd[64];
	//回调函数设置
	TDF_RefDataMsgHandler pfnRefDataHandler;       //参考数据消息处理回调
};

//沪港通、深港通等
struct TDF_NON_MD_DATA
{
	int nItemSize;					//非行情消息大小
	int nMsgType;					//非行情消息类型
	char pData[1024*1024 - 8];		//非行情消息内容
};

//经纪商队列(HK)/////////////////////////////////////
struct TD_BrokerQueue
{
	char  szWindCode[32];				//Wind Code: AG1302.SHF
	char  szCode[32];					//原始 Code
	int   nActionDay;					//自然日
	int   nAskTime;						// 叫卖时间（HHMMSSmmm）
	int   nBidTime;						// 叫买时间（HHMMSSmmm）
	int   nAskBrokers;					// 叫卖经纪个数
	int   nBidBrokers;					// 叫买经纪个数
	short sAskBroker[40];				// 叫卖前40经纪
	short sBidBroker[40];				// 叫买前40经纪
	const TDF_CODE_INFO *  pCodeInfo;    //代码信息， TDF_Close，清盘重连后，此指针无效
};

#pragma pack(pop)
#endif
