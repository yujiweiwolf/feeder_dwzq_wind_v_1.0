# feeder_dwzq_wind
* 东吴证券万德宏汇行情接入


# 注意
* 不同券商的万德宏汇TDF行情系统的版本可能不一样，而且不兼容，但是客户端的解析代码逻辑大致相同。为了复用代码，
本项目中将公共代码放在了src/libwind下，该目录并不是一个独立的库，不能直接编译。在VS2019解决方案中建立了一个
libwind的工程，目的是为了方便编辑公共代码，请不要对其进行编译。

* 不同的券商分别建立独立的工程，配置不同的lib库地址，并在工程配置文件中定义TDF API版本的宏，定义示例：
C/C++ -> Preprocessor -> Preprocessor Definitions: TDFAPI_V2

* 目前已实现的Feeder：
1. dwzq_stock_feeder:  TDFAPI_V2 (tdfapi-2.5_dwzq)
2. dwzq_option_feeder: TDFAPI_V3 (tdfapi-3.0_dwzq)




# 备注
```
行情服务器

-------------------------------------------------------------------
测试环境：
服务器IP：114.80.159.20
端口：10000
帐号：axzqcs_tdf
密码：axzqcs_tdf

==============================================================================
安信证券：tdfapi-2.7_axzq
..\..\..\vclibs\boost-1.62.0\include;..\..\..\vclibs\libiconv-1.14\include;..\..\..\vclibs\libopenssl-1.1.0c\include;..\..\..\vclibs\libprotobuf-3.1.0\include;..\..\..\vclibs\libsodium-1.0.11\include;..\..\..\vclibs\libzmq-4.2.0\include;..\..\..\vclibs\libx-1.0\include;..\..\..\vclibs\libcoral-1.0\include;..\..\lib\tdfapi-2.7_axzq\include;%(AdditionalIncludeDirectories)
..\..\..\vclibs\boost-1.62.0\lib\Win32;..\..\..\vclibs\libiconv-1.14\lib\Win32\Release\v140\static;..\..\..\vclibs\libopenssl-1.1.0c\lib\Win32\Release\v140\dynamic;..\..\..\vclibs\libprotobuf-3.1.0\lib\Win32\Release\v140\static;..\..\..\vclibs\libsodium-1.0.11\lib\Win32\Release\v140\static;..\..\..\vclibs\libzmq-4.2.0\lib\Win32\Release\v140\dynamic;..\..\..\vclibs\libx-1.0\lib\Win32\Release\v140\static;..\..\..\vclibs\libcoral-1.0\lib\Win32\Release\v140\static;..\..\lib\tdfapi-2.7_axzq\lib\Win32Release;%(AdditionalLibraryDirectories)
#pragma comment(lib, "TDFAPI2.lib")

==============================================================================
东吴证券：tdfapi-2.5_dwzq
..\..\..\vclibs\boost-1.62.0\include;..\..\..\vclibs\libiconv-1.14\include;..\..\..\vclibs\libopenssl-1.1.0c\include;..\..\..\vclibs\libprotobuf-3.1.0\include;..\..\..\vclibs\libsodium-1.0.11\include;..\..\..\vclibs\libzmq-4.2.0\include;..\..\..\vclibs\libx-1.0\include;..\..\..\vclibs\libcoral-1.0\include;..\..\lib\tdfapi-2.5_dwzq\include;%(AdditionalIncludeDirectories)
..\..\..\vclibs\boost-1.62.0\lib\Win32;..\..\..\vclibs\libiconv-1.14\lib\Win32\Release\v140\static;..\..\..\vclibs\libopenssl-1.1.0c\lib\Win32\Release\v140\dynamic;..\..\..\vclibs\libprotobuf-3.1.0\lib\Win32\Release\v140\static;..\..\..\vclibs\libsodium-1.0.11\lib\Win32\Release\v140\static;..\..\..\vclibs\libzmq-4.2.0\lib\Win32\Release\v140\dynamic;..\..\..\vclibs\libx-1.0\lib\Win32\Release\v140\static;..\..\..\vclibs\libcoral-1.0\lib\Win32\Release\v140\static;..\..\lib\tdfapi-2.5_dwzq\lib\Win32Release;%(AdditionalLibraryDirectories)
#pragma comment(lib, "TDFAPI25.lib")

东吴证券：期权行情（独立于股票行情）
tdfapi-3.0_dwzq

==============================================================================
海通证券：tdfapi-3.2_htzq（3.2.0不支持订阅日期，使用tdfapi-2.7axzq版本。）
..\..\..\vclibs\boost-1.62.0\include;..\..\..\vclibs\libiconv-1.14\include;..\..\..\vclibs\libopenssl-1.1.0c\include;..\..\..\vclibs\libprotobuf-3.1.0\include;..\..\..\vclibs\libsodium-1.0.11\include;..\..\..\vclibs\libzmq-4.2.0\include;..\..\..\vclibs\libx-1.0\include;..\..\..\vclibs\libcoral-1.0\include;..\..\lib\tdfapi-3.2.0_htzq\include;%(AdditionalIncludeDirectories)
..\..\..\vclibs\boost-1.62.0\lib\Win32;..\..\..\vclibs\libiconv-1.14\lib\Win32\Release\v140\static;..\..\..\vclibs\libopenssl-1.1.0c\lib\Win32\Release\v140\dynamic;..\..\..\vclibs\libprotobuf-3.1.0\lib\Win32\Release\v140\static;..\..\..\vclibs\libsodium-1.0.11\lib\Win32\Release\v140\static;..\..\..\vclibs\libzmq-4.2.0\lib\Win32\Release\v140\dynamic;..\..\..\vclibs\libx-1.0\lib\Win32\Release\v140\static;..\..\..\vclibs\libcoral-1.0\lib\Win32\Release\v140\static;..\..\lib\tdfapi-3.2.0_htzq\lib\Win32Release;%(AdditionalLibraryDirectories)
#pragma comment(lib, "TDFAPI30.lib")


==============================================================================
停牌状态修正逻辑：
宏汇推过来的股票停牌状态有可能不准，所以需要增加自动修正的逻辑。

对于停牌股票，有可能在开盘前或开盘后推过来一条或多条行情，时间戳可能是八点多，也可能是开盘后，状态有的是停牌有的不是停牌。
情况举例：
1.共推过来两条行情：第一条行情cStatus为“停牌”；第二条cStatus为0，盘口为0。
2.共推过来两条行情，第一条行情cStatus为“正常”；第二条cStatus为“停牌”，但是两条行情的时间戳都是一样的。
3.共推过来一条行情：第一条行情cStatus为0，盘口为0；

==============================================================================
对于指数行情，在9:25分之前可能会推过来几条行情，时间戳都是0，在9:25之后时间戳才正常。

期权交易所时间戳：
在没有任何成交数据之前，期权的交易所时间戳都是0。比如9:20的时候，期权行情里面的时间戳是0.


===========================
股票状态清洗流程：
1.每收到一条股票行情数据，执行如下检查：行情时间>=9:25:00，成交量=0，买一量=0，卖一量=0，将状态置为停牌。
2.当收到第一条时间>=9:26:00.000的股票行情数据时，执行如下操作：
   检查当前接收到的所有股票行情，如果行情原始股票状态为非停牌，行情时间>=9:20，买一量=0，卖一量=0，成交量=0，则将状态置为停牌




```