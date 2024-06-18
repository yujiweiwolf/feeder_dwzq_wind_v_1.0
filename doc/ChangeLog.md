@TODO
重连时进行登出操作

+--------------------------------------------------+
v4.1.12.0
* 升级libfeeder中的合约状态修复
  东吴静态数据落地，供FPGA行情使用

v4.1.11.0
* 配置文件中的server_host_1为0时, 是旧版本的单活模式;
  等于1时, 是新版本的双活模式.

v4.1.9.0
* 增加集合竞价虚拟未匹配量的解析；

v4.1.8.0
* 优化序列化性能

v4.1.7.0
* 修改holo行情

v4.1.6.0  [2021-12-13 14:14:00]
* 修改功能：修改上交所逐笔委托的解析；

v4.1.5.0  [2021-07-27 10:35:48]
        * Bugfix：修复推送pb2和pb3协议时崩溃的问题；
		* Bugfix：修复收盘后无法自动退出的问题；

+--------------------------------------------------+
v4.1.4.0  [2021-05-07 17:24:07]
        * 升级api, 增加上海逐笔委托中撤单的处理；
		* 内嵌co_holod全息行情服务器；

+--------------------------------------------------+
v4.1.3.0  [2021-04-07 11:24:07] Guangxu Pan <guangxup@gmail.com>
        * 新增功能：增加性能调试字段，可跟踪每条行情在系统各节点的处理时间，可监控客户端是否漏收行情以及接收和处理行情速度是否及时；

+--------------------------------------------------+
v4.1.2.0  [2020-04-21 16:08:18] Guangxu Pan <guangxup@gmail.com>
        * Bugfix: 修复本地文件名，从co_spot改为co_stock

+--------------------------------------------------+
v4.1.1.0  [2020-04-20 16:38:17] Guangxu Pan <guangxup@gmail.com>
        * Bugfix: 修复缺失部分股票名称的问题

+--------------------------------------------------+
v4.1.0.0  [2020-02-27 19:02:13] Guangxu Pan <guangxup@gmail.com>
        * 升级到宏汇TDF_API 3.3.0版本，并支持Linux系统部署

+--------------------------------------------------+
v4.0.3.0  [2020-02-17 13:38:30] Guangxu Pan <guangxup@gmail.com>
        * 增加未识别逐笔委托的警告日志

+--------------------------------------------------+
v4.0.2.0  [2020-02-08 14:03:46] Guangxu Pan <guangxup@gmail.com>
        * trade_push_addr中增加level2数据

+--------------------------------------------------+
v4.0.1.0  [2019-09-19 12:23:45] Guangxu Pan <guangxup@gmail.com>
        * 增加加密密码的功能

+--------------------------------------------------+
v4.0.0.0  [2019-05-08 10:21:00] Guangxu Pan <guangxup@gmail.com>
        * 升级为FB协议

+--------------------------------------------------+
v3.0.0.12  [2019-03-16 08:31:43] Guangxu Pan <guangxup@gmail.com>
        * 新增功能：增加主买主卖字段的计算，使用smart holo逻辑，用于与全息行情进行合并。

+--------------------------------------------------+
v3.0.0.11  [2019-01-29 10:33:32] Guangxu Pan <guangxup@gmail.com>
        * 增加错误盘口数据的自动过滤功能

+--------------------------------------------------+
v3.0.0.10  [2018-05-11 13:57:27] Guangxu Pan <guangxup@gmail.com>
        * 期权underlying_code增加市场后缀

+--------------------------------------------------+
v3.0.0.9  [2018-05-02 17:31:13] Guangxu Pan <guangxup@gmail.com>
        * 删除swordfish老行情协议的逐笔数据推送功能

+--------------------------------------------------+
v3.0.0.8  [2017-12-30 10:26:47] Guangxu Pan <guangxup@gmail.com>
        * 修复压缩文件里面原始文件名不正确的Bug
        * 修复委托队列前缀不正确的Bug
        * 修改导历史数据的退出机制，空转一分钟就退出。

+--------------------------------------------------+
v3.0.0.7  [2017-12-18 16:43:32] Guangxu Pan <guangxup@gmail.com>
        * 解决盘中股票停牌状态自动修正功能失效的Bug

+--------------------------------------------------+
v3.0.0.6  [2017-08-31 22:07:32] Guangxu Pan <guangxup@gmail.com>
        + 调整逐笔数据的推送格式，将sn放到前面，与股票的stamp在同样的位置
        + 增加禁用推送逐笔数据的功能
        + 增加股票推送逻辑，对于停牌股票将不会被清洗服务器过滤掉。

+--------------------------------------------------+
v3.0.0.5  [2017-08-16 17:18:09] Guangxu Pan <guangxup@gmail.com>
        + 增加宏汇TDFAPI V3.0的支持
        + 日期检查，只检查沪深两个交易所的行情日期。

+--------------------------------------------------+
v3.0.0.4  [2017-06-17 10:43:17] Guangxu Pan <guangxup@gmail.com>
        + 增加委托队列数据
        * 修改逐笔委托和逐笔成交中char字段的解析方式
        * 修改买卖方向转换函数的实现，将map改成switch。

+--------------------------------------------------+
v3.0.0.3  [2017-02-14 10:48:30] Guangxu Pan <guangxup@gmail.com>
        + 增加程序退出时的事件监听，关闭gzip输出流，避免压缩文件被损坏。

+--------------------------------------------------+
v3.0.0.2  [2017-02-08 17:07:21] Guangxu Pan <guangxup@gmail.com>
        * 修改本地行情文件的存储格式为gzip

+--------------------------------------------------+
v3.0.0.1  [2016-12-28 09:36:16] Guangxu Pan <guangxup@gmail.com>
        * 修改自动退出时机：在数据时间14:57:00之后，而且在收到第一个心跳信息的系统时间10分钟之后，认为已经闭市

+--------------------------------------------------+
v3.0.0.0  [2016-12-09 10:08:07] Guangxu Pan <guangxup@gmail.com>
        * 修改为Coral行情协议

+--------------------------------------------------+
v2.0.6.18 [2016-11-30 15:53:30] Guangxu Pan <guangxup@gmail.com>
        * 修改逐笔委托和逐笔成交的解析程序，删除order_type, match_kind, match_code的前后空白字符

+--------------------------------------------------+
v2.0.6.17 [2016-11-28 16:37:37] Guangxu Pan <guangxup@gmail.com>
        + 增加检测“闭市”的功能，有的行情源不推送“闭市”的系统消息，需要自动判断是否已闭市并退出。

+--------------------------------------------------+
v2.0.6.16 [2016-11-25 17:00:27] Guangxu Pan <guangxup@gmail.com>
        * 修改逐笔委托和逐笔成交推送消息格式，增加委托合同号和成交合同号，用于全息行情服务器进行丢包检查。

+--------------------------------------------------+
v2.0.6.15 [2016-11-25 10:03:05] Guangxu Pan <guangxup@gmail.com>
        * 修改行情文件的保存逻辑，先保存到临时文件，当接收到“闭市”消息时再关闭文件，并移动到最终位置。
        + 增加行情延迟监控，统计股票、期权、逐笔委托、逐笔成交的最小延迟、最大延迟和平均延迟；

+--------------------------------------------------+
v2.0.6.14 [2016-11-08 14:38:17] Guangxu Pan <guangxup@gmail.com>
        + 增加订阅回放多个日期的功能，设置sub_date参数填写日期列表，多个日期之间用逗号分隔。 

+--------------------------------------------------+
v2.0.6.13 [2016-10-28 11:23:16] Guangxu Pan <guangxup@gmail.com>
        * 增加订阅类型：3-行情和逐笔数据

+--------------------------------------------------+
v2.0.6.12 [2016-09-05 13:13:27] Guangxu Pan <guangxup@gmail.com>
        * 修改行情日期检查逻辑，原来只在登录成功后进行检查，现增加在代码表接收处进行检查。
        * 修改配置，删除enable_push选项，根据push_addr判断为PUSH或PUB

+--------------------------------------------------+
v2.0.6.11 [2016-08-24 11:17:48] Guangxu Pan <guangxup@gmail.com>
        + 增加行情本地存储的功能，将序列化后的数据采用Base64编码进行保存，保存位置为：data/co_stock_yyyymmdd.b64
        * 修改配置文件，删除pub_port参数，PUB和PUSH统一使用pub_addr进行配置

+--------------------------------------------------+
v2.0.6.10 [2016-08-03 10:19:25] Guangxu Pan <guangxup@gmail.com>
        * 修复Bug：解决在9:30开盘之前，大部分股票被修正为停牌状态的问题

+--------------------------------------------------+
v2.0.6.9 [2016-06-22 10:12:33] Guangxu Pan <guangxup@gmail.com>
        + 增加股票状态修复日志
		+ 增加修改停牌股票时间戳的逻辑，因为有可能存在时间戳相同但是状态不同的行情数据，需要修改停牌行情的时间戳，避免被清洗服务器过滤掉。

+--------------------------------------------------+
v2.0.6.8 [2016-06-14 16:59:04] Guangxu Pan <guangxup@gmail.com>
        + 增加第二条股票状态修复逻辑，当接收到股票行情时，将满足如下条件的股票状态置为停牌：行情时间>=9:19:00，成交量=0，买一量=0，卖一量=0

+--------------------------------------------------+
v2.0.6.7 [2016-06-14 15:17:37] Guangxu Pan <guangxup@gmail.com>
        * 修改股票停牌状态修复逻辑，对于9:20统一修复的股票，将时间戳增加1毫秒，防止修复后的行情被清洗服务器过滤掉！

+--------------------------------------------------+
v2.0.6.6 [2016-05-26 13:15:20] Guangxu Pan <guangxup@gmail.com>
        * 修改重连机制，允许在推送模式下进行重连，从头开始传输行情。

+--------------------------------------------------+
v2.0.6.5 [2016-05-24 17:16:13] Guangxu Pan <guangxup@gmail.com>
        + 增加行情推送到清洗服务器的功能

+--------------------------------------------------+
v2.0.6.4 [2016-05-20 08:38:13] Guangxu Pan <guangxup@gmail.com>
        * 针对安信版本，升级宏汇TDF-API到2.8，解决2.7版本接收不到代码表导致无股票名称和期权基础信息的问题。

+--------------------------------------------------+
v2.0.6.3 [2016-05-11 15:07:26] Guangxu Pan <guangxup@gmail.com>
        * 修改行情打印时间，直接打印最新数据的时间戳，不再与上一个时间戳进行大小比较显示最大值
		* 修改重连机制，当网络连接断开后，宏汇底层API会尝试自动重连，重连成功后会重新从开盘前回放行情；
		   增加对重连机制的干涉，当收到重连通知消息时检查已接收到的数据条数，如果发现之前已经收到过数据，
		   说明是盘中的自动重连，这种情况非常危险，直接强制退出程序。

+--------------------------------------------------+
v2.0.6.2 [2015-12-31 15:54:09] Guangxu Pan <guangxup@gmail.com>
        * 只接收逐笔数据时，为了提高性能，忽略行情数据的解析。

+--------------------------------------------------+
v2.0.6.1 [2015-09-29 15:13:01] Guangxu Pan <guangxup@gmail.com>
        + 修改股票停牌逻辑判断

+--------------------------------------------------+
v2.0.6.0 [2015-09-14 21:39:22] Guangxu Pan <guangxup@gmail.com>
        + v1.0版本支持SIRTP协议，v2.0版本支持Swordfish协议
		+ 修改停牌逻辑判断，增加自定义判断逻辑

+--------------------------------------------------+
v1.0.5.5 [2015-01-12 17:09:30] Guangxu Pan <guangxup@gmail.com>
		+ 增加打印代码表的功能，在DEBUG日志级别下有效

+--------------------------------------------------+
v1.0.5.4 [2014-12-17 14:19:06] Guangxu Pan <guangxup@gmail.com>
		+ 增加市场和代码订阅功能

+--------------------------------------------------+
v1.0.5.3 [2014-12-11 09:24:25] Guangxu Pan <guangxup@gmail.com>
		* 修改行情日期检查逻辑，可配置行情日期检查失败后是否进行重连。

+--------------------------------------------------+
v1.0.5.2 [2014-11-04 10:24:49] Guangxu Pan <guangxup@gmail.com>
		+ 增加行情日期检查逻辑，订阅最新行情时有可能返回昨天的行情。

+--------------------------------------------------+
v1.0.5.1 [2014-09-30 09:34:46] Guangxu Pan <guangxup@gmail.com>
		+ 初始化pub的zmq之后增加2s的sleep

+--------------------------------------------------+
v1.0.5.0 [2014-09-25 10:30:12] Guangxu Pan <guangxup@gmail.com>
		* 修正涨停时买盘口字段未赋值和跌停时卖盘口字段未赋值的bug

+--------------------------------------------------+
v1.0.4.2 [2014-09-09 10:13:48] Guangxu Pan <guangxup@gmail.com>
		- 删除159901行情延迟监控功能

+--------------------------------------------------+
v1.0.4.1 [2014-08-06 21:50:43] Guangxu Pan <guangxup@gmail.com>
		* 升级libiconv.lib，从1.9.2升级到1.14

+--------------------------------------------------+
v1.0.4.0 [2014-08-04 17:03:01] Guangxu Pan <guangxup@gmail.com>
		* 升级宏汇行情API库，版本为：TDFAPI-2.5_WINDOWS_20140730

+--------------------------------------------------+
v1.0.3.5 [2014-08-01 09:29:54] Guangxu Pan <guangxup@gmail.com>
		+ 宏汇已升级股票涨跌停的bug，本程序中去掉根据盘口判断停牌的逻辑。

+--------------------------------------------------+
v1.0.3.4 [2014-08-01 09:12:59] Guangxu Pan <guangxup@gmail.com>
        + 增加159901行情延迟监控功能

+--------------------------------------------------+
v1.0.3.3 [2014-07-29 09:24:36] Guangxu Pan <guangxup@gmail.com>
		* 修改涨跌停字段弄反了的bug

+--------------------------------------------------+
v1.0.3.2 [2014-07-28 14:50:44] Guangxu Pan <guangxup@gmail.com>
		* 完善市场代码的转换函数

+--------------------------------------------------+
v1.0.3.1 [2014-07-22 15:30:09] Guangxu Pan <guangxup@gmail.com>
		* 修改停牌判断逻辑，只有在集合竞价之前才根据盘口来判断是否停牌。

+--------------------------------------------------+
v1.0.3.0 [2014-07-14 14:35:42] Guangxu Pan <guangxup@gmail.com>
		* 修改期权pb协议，增加stamp和type字段，原type字段改为sub_type。

+--------------------------------------------------+
v1.0.2.3 [2014-07-14 13:21:21] Guangxu Pan <guangxup@gmail.com>
		* 修改停牌判断逻辑，加上根据盘口判断停牌的逻辑。

+--------------------------------------------------+
v1.0.2.2 [2014-07-08 14:40:31] Guangxu Pan <guangxup@gmail.com>
        * 修改停牌判断逻辑，去掉根据盘口判断停牌的逻辑。

+--------------------------------------------------+
v1.0.2.1 [2014-07-04 09:56:03] Guangxu Pan <guangxup@gmail.com>
		* 修改停牌判断逻辑

+--------------------------------------------------+
v1.0.2.0 [2014-07-01 09:54:14] Guangxu Pan <guangxup@gmail.com>
        + 增加逐笔数据
		* 修改停牌判断逻辑

+--------------------------------------------------+
v1.0.1.0 [2014-06-09 13:50:48] Guangxu Pan <guangxup@gmail.com>
        * 升级TDF2.5库

+--------------------------------------------------+
v1.0.0.0 [2014-05-10 13:11:28] Guangxu Pan <guangxup@gmail.com>
        * 初始版本