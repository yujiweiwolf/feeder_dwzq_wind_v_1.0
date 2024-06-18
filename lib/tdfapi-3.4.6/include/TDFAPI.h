#ifndef __TDF_API_H__
#define __TDF_API_H__

#include "TDFAPIStruct.h"

#if defined(WIN32) || defined(WIN64) || defined(_WINDOWS)
#ifdef TDF_API_EXPORT
#define TDFAPI __declspec(dllexport) 
#else	
#define TDFAPI __declspec(dllimport)
#endif
#else
#define TDFAPI __attribute((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum TDF_ERR
{
    TDF_ERR_UNKOWN=-200,                // 未知错误

    TDF_ERR_INITIALIZE_FAILURE = -100,  // 初始化socket环境失败
    TDF_ERR_NETWORK_ERROR,              // 网络连接出现问题
    TDF_ERR_INVALID_PARAMS,             // 输入参数无效
    TDF_ERR_VERIFY_FAILURE,             // 登陆验证失败：原因为用户名或者密码错误；超出登陆数量
    TDF_ERR_NO_AUTHORIZED_MARKET,       // 所有请求的市场都没有授权
    TDF_ERR_NO_CODE_TABLE,              // 所有请求的市场该天都没有代码表
    
    TDF_ERR_SUCCESS = 0,                // 成功
};


//设置TDF环境变量值,在调用TDF_Open之前设置
//返回值：TDF_ERR_INVALID_PARAMS表示无效的nEnv，TDF_ERR_SUCCESS表示成功
TDFAPI int TDF_SetEnv(TDF_ENVIRON_SETTING nEnv, unsigned int nValue);

//设置日志文件路径
TDFAPI int TDF_SetLogPath(const char* path);


//同步函数，打开到TDFServer的连接，如果成功，则返回句柄，否则返回NULL，在TDF_Open期间发生了网络断开，将不会自动重连
//在调用期间，系统通知函数将收到MSG_SYS_CONNECT_RESULT，MSG_SYS_LOGIN_RESULT，MSG_SYS_CODETABLE_RESULT消息
//如果网络断开，则会收到MSG_SYS_DISCONNECT_NETWORK，pErr中存放错误代码
TDFAPI THANDLE TDF_Open(TDF_OPEN_SETTING* pSettings, TDF_ERR* pErr);
//可以配置多组服务器，取多组服务器中最快的行情推送
TDFAPI THANDLE TDF_OpenExt(TDF_OPEN_SETTING_EXT* pSettings,TDF_ERR* pErr);
//通过代理打开连接，回调消息和错误代码和TDF_Open一样
TDFAPI THANDLE TDF_OpenProxy(TDF_OPEN_SETTING* pOpenSettings, TDF_PROXY_SETTING* pProxySettings, TDF_ERR* pErr);
TDFAPI THANDLE TDF_OpenProxyExt(TDF_OPEN_SETTING_EXT* pOpenSettings, TDF_PROXY_SETTING* pProxySettings, TDF_ERR* pErr);

//获取指定市场的代码表，在已经收到MSG_SYS_CODETABLE_RESULT 消息之后，可以获得代码表
//szMarket格式为：market-level-source(SHF-1-0)
//获取到的代码表，需要调用TDF_FreeArr来释放内存
TDFAPI int TDF_GetCodeTable(THANDLE hTdf, const char* szMarket, TDF_CODE** pCode, unsigned int* pItems);

// 从万得代码来获取详细的期权代码信息
// pCodeInfo指针由用户提供，
// 如果成功获取，则返回TDF_ERR_SUCCESS，否则返回 TDF_ERR_NO_CODE_TABLE 或 TDF_ERR_INVALID_PARAMS
// szCode 格式为原始code + . + 市场(如ag.SHF)
TDFAPI int TDF_GetOptionCodeInfo(THANDLE hTdf, const char* szCode, TDF_OPTION_CODE* pCodeInfo, const char* szMarket);

//同步函数，关闭连接，不要在回调函数里面调用，否则会卡死
TDFAPI int TDF_Close(THANDLE hTdf);

TDFAPI void TDF_FreeArr(void *pArr);

//登陆后订阅; 此函数是个异步函数，在TDF_Open成功之后调用
//过滤发生在服务端，该函数调用后，订阅列表若有变化，服务端会发送所有订阅代码的最新快照
//szSubScriptions:需要订阅的股票(单个股票格式为原始Code+.+市场，如999999.SH)，以“;”分割，例如"600000.SH;ag.SHF;000001.SZ"
TDFAPI int TDF_SetSubscription(THANDLE hTdf, const char* szSubScriptions, SUBSCRIPTION_STYLE nSubStyle);

//打开获取参考数据的服务
TDFAPI THANDLE TDF_Open_RefData(TDF_OPEN_REFDATA_SETTING* pSettings, TDF_ERR* pErr);

//获取ETF清单,szMarketAbbr格式为为SH
//reqCode格式为原始code(999999)
TDFAPI int TDF_ReqETFList(THANDLE hTdf, int reqID, const char* szMarketAbbr, const char* reqCode, int reqDate, int dataLevel, int marektSource = 0);

//获取连接状态信息
TDFAPI int TDF_GetConStatInfo(THANDLE hTdf, int conIndex, ConStatInfo* pConsStatInfo);

//释放全局的内存：整个程序退出时，如果需要测试内存分配，可调用释放全局内存. 调用后，API库不可用
TDFAPI void TDF_EXIT();
//获取当前API版本号
TDFAPI const char* TDF_Version();

//////////////////////////////////以下为主动获取行情接口//////////////////////////////////////////
//获取当前最新行情,szMarket格式为market-level-source,需调用TDF_FreeArr(void*)释放内存
TDFAPI int TDF_GetLastMarketData(THANDLE hTdf, const char* szMarket, TDF_MARKET_DATA** pMarketData, int* nItems);
//获取当前最新指数,szMarket格式为market-level-source,需调用TDF_FreeArr(void*)释放内存
TDFAPI int TDF_GetLastIndexData(THANDLE hTdf, const char* szMarket, TDF_INDEX_DATA** pIndexData, int* nItems);
//获取当前最新期货(期权),szMarket格式为market-level-source,需调用TDF_FreeArr(void*)释放内存
TDFAPI int TDF_GetLastFutureData(THANDLE hTdf, const char* szMarket, TDF_FUTURE_DATA** pFutureData, int* nItems);
//获取当前最新快照,内核模式使用,szMarket格式为market-level-source,需调用TDF_FreeArr(void*)释放内存
TDFAPI int TDF_GetLastSnapShot(THANDLE hTdf, const char* szMarket, void** pSnapShot, unsigned int* nItems);
//根据windcode获取当前最新快照,内核模式使用,szMarket格式为market-level-source,需调用TDF_FreeArr(void*)释放内存
TDFAPI int TDF_GetSnapShotByWindcode(THANDLE hTdf, const char* szMarket, void** pOneSnapShot, const char* szWindCode);

#ifdef __cplusplus
}
#endif

#endif