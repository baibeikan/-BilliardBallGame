// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 FAST_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// FAST_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef FAST_EXPORTS
#define FAST_API __declspec(dllexport)
#else
#define FAST_API __declspec(dllimport)
#endif


////////////////////////////////////////////////////////////////////////////////
// TCP服务器
////////////////////////////////////////////////////////////////////////////////

// TCP服务器接收客户端连接回调
// pAddr：当前客户端地址字符串[xxx.xxx.xxx.xxx:port]
// pExample：FastTcpServerOpen函数pExample参数传递的类或数据实例指针
// 返回值：空
typedef void (CALLBACK *FastCallBackConect)(const char* pAddr, void* pExample);

// TCP服务器断开客户端连接回调
// pAddr：当前客户端地址字符串[xxx.xxx.xxx.xxx:port]
// exit：退出代码[0：安全关闭]
// pExample：FastTcpServerOpen函数pExample参数传递的类或数据实例指针
// 返回值：空
typedef void (CALLBACK *FastCallBackDisconect)(const char* pAddr, int exit, void* pExample);

// TCP服务器接收客户端数据回调
// pAddr：当前客户端地址字符串[xxx.xxx.xxx.xxx:port]
// pRecv：从当前客户端接收的数据
// uRecv：从当前客户端接收的数据长度
// pExample：FastTcpServerOpen函数pExample参数传递的类或数据实例指针
// pOutSend：回传给当前客户端的数据[用户编写]
//			 返回值=0		不处理
//			 返回值>0		填充发送给客户端的数据流
// 返回值：[用户编写]
//		   =0		不向客户端发送数据
//		   >0		发送给客户端的数据字节数
typedef int (CALLBACK *FastCallBackTcpServerQA)(const char* pAddr, void* pRecv, int uRecv, void* pExample, void* pOutSend);

// TCP服务器发送给客户端数据回调
// pAddr：当前客户端地址字符串[xxx.xxx.xxx.xxx:port]
// pExample：FastTcpServerOpen函数pExample参数传递的类或数据实例指针
// pOutSend：回传给当前客户端的数据[用户编写]
//			 返回值=0	不处理
//			 返回值>0	填充发送给客户端的数据流
// 返回值：[用户编写]
//		   =0		不向客户端发送数据
//		   >0		发送给客户端的数据字节数
typedef int (CALLBACK *FastCallBackTcpServerSend)(const char* pAddr, void* pExample, void* pOutSend);

// TCP服务器接收客户端数据回调
// pAddr：当前客户端地址字符串[xxx.xxx.xxx.xxx:port]
// pRecv：从当前客户端接收的数据
// uRecv：从当前客户端接收的数据长度
// pExample：FastTcpServerOpen函数pExample参数传递的类或数据实例指针
// 返回值：空
typedef void (CALLBACK *FastCallBackTcpServerRecv)(const char* pAddr, void* pRecv, int uRecv, void* pExample);

// 打开TCP服务器[等待客户端请求模式]
// name：服务器名称[唯一标记]
// port：服务器端口
// pExample：传递给回调函数的类或数据实例指针
// cbRun：接收到客户端数据后处理填充发送给客户端数据的回调函数
// cbConect：客户端断接入回调函数
// cbDisconect：客户端断断开回调函数
// 返回值：<0	失败[-1：参数错误|-2：服务器创建错误]
//		   >=0	成功
FAST_API int FastTcpServerOpen(const char* name, unsigned short port, void* pExample,
	FastCallBackTcpServerQA cbRun, FastCallBackConect cbConect = NULL, FastCallBackDisconect cbDisconect = NULL);

// 打开TCP服务器[服务器主动发送模式]
// name：服务器名称[唯一标记]
// port：服务器端口
// pExample：传递给回调函数的类或数据实例指针
// cbSend：填充发送给客户端数据的回调函数
// cbRecv：接收到客户端数据后处理的回调函数
// cbConect：客户端接入回调函数
// cbDisconect：客户端断开回调函数
// uTestConnectionTime：无数据交互时检测连接中断的时间[单位：毫秒]
// 返回值：<0	失败[-1：参数错误|-2：服务器创建错误]
//		   >=0	成功
FAST_API int FastTcpServerOpen(const char* name, unsigned short port, void* pExample,
	FastCallBackTcpServerSend cbSend, FastCallBackTcpServerRecv cbRecv = NULL, 
	FastCallBackConect cbConect = NULL, FastCallBackDisconect cbDisconect = NULL,
	unsigned int uTestConnectionTime = 1000);

// 关闭TCP服务器
// name：服务器名称[唯一标记]
// 返回值：空
FAST_API void FastTcpServerClose(const char* name);

// TCP服务器是否打开
// name：服务器名称[唯一标记]
// 返回值：TCP服务器是否打开
FAST_API bool FastHasTcpServer(const char* name);


////////////////////////////////////////////////////////////////////////////////
// TCP客户端
////////////////////////////////////////////////////////////////////////////////

// TCP客户端连接服务器
// name：客户端名称[唯一标记]
// ip：目标服务器IP
// port：目标服务器端口
// 返回值：<0	失败[-1：参数错误|-2：客户端连接错误]
//		   >=0	成功
FAST_API int FastTcpClientOpen(const char* name, const char* ip, unsigned short port);

// TCP客户端断开服务器
// name：客户端名称[唯一标记]
// 返回值：空
FAST_API void FastTcpClientClose(const char* name);

// TCP客户端发送数据
// name：客户端名称[唯一标记]
// pSend：向服务器发送的数据
// uSend：向服务器发送的数据字节数
// 返回值：<0	失败[-1：参数错误|-2：套接字错误|-3：套接字安全关闭]
//		   >=0	成功[实际发送数据字节数]
FAST_API int FastTcpClientSend(const char* name, void* pSend, int uSend);

// TCP客户端接收数据
// name：客户端名称[唯一标记]
// pRecv：从服务器接收的数据
// uRecv：从服务器接收的数据字节数
// 返回值：<0	失败[-1：参数错误|-2：套接字错误|-3：套接字安全关闭]
//		   >=0	成功[实际发送数据字节数]
FAST_API int FastTcpClientRecv(const char* name, void* pRecv, int uRecv);

// TCP客户端连接服务器直到成功
// name：客户端名称[唯一标记]
// ip：目标服务器IP
// port：目标服务器端口
// pExit：用于用户强制结束的事件
// milliseconds：每次连接失败等待的时间
// 返回值：<0	失败[-1：参数错误|-2：客户端连接错误|-3：用户强制结束]
//		   >=0	成功
FAST_API int FastTcpClientOpenUntilSuccess(const char* name, const char* ip, unsigned short port, 
	void* pExit = NULL, long milliseconds = 500);


////////////////////////////////////////////////////////////////////////////////
// UDP服务器
////////////////////////////////////////////////////////////////////////////////

// UDP服务器接收客户端数据回调[接收到数据触发]
// pAddr：当前客户端地址字符串[xxx.xxx.xxx.xxx:port]
// pRecv：从当前客户端接收的数据
// uRecv：从当前客户端接收的数据字节数
// pExample：FastUdpServerOpen函数pExample参数传递的类或数据实例指针
// pOutSend：回传给当前客户端的数据[用户编写]
//			 返回值=0	不处理
//			 返回值>0	填充发送给客户端的数据流
//			 返回值<0	填充发送给客户端的十六进制文本路径
// 返回值：[用户编写]
//		   =0	不向客户端发送数据
//		   >0	发送给客户端的数据字节数
//	       <0	发送给客户端十六进制文本文件第几行[取绝对值]
typedef int (CALLBACK *FastCallBackUdpServerQA)(const char* pAddr, void* pRecv, unsigned int uRecv, void* pExample, void* pOutSend);

// 打开UDP服务器
// name：服务器名称[唯一标记]
// port：服务器端口
// pExample：传递给回调函数的类或数据实例指针
// cbRun：接收到客户端数据后处理填充发送给客户端数据的回调函数
// 返回值：<0	失败[-1：参数错误|-2：服务器创建错误]
//		   >=0	成功
FAST_API int FastUdpServerOpen(const char* name, unsigned short port, void* pExample, FastCallBackUdpServerQA cbRun);

// 关闭UDP服务器
// name：服务器名称[唯一标记]
// 返回值：空
FAST_API void FastUdpServerClose(const char* name);

// UDP服务器是否打开
// name：服务器名称[唯一标记]
// 返回值：UDP服务器是否打开
FAST_API bool FastHasUdpServer(const char* name);


////////////////////////////////////////////////////////////////////////////////
// UDP通讯
////////////////////////////////////////////////////////////////////////////////

// 打开UDP通讯
// name：UDP名称[唯一标记]
// port：本机端口
// 返回值：<0	失败[-1：参数错误|-2：UDP创建错误]
//		   >=0	成功
FAST_API int FastUdpOpen(const char* name, unsigned short port);

// 关闭UDP通讯
// name：UDP名称[唯一标记]
// 返回值：空
FAST_API void FastUdpClose(const char* name);

// UDP发送数据
// name：UDP名称[唯一标记]
// ipTarget：目标IP
// portTarget：目标端口
// pSend：向目标发送的数据[>0：数据|<0：十六进制文本文件路径]
// uSend：向目标发送的数据字节数[>0：数据字节数|<0：文本第几行]
// 返回值：<0	失败[-1：参数错误|-2：套接字错误|-3：套接字安全关闭]
//		   >=0	成功[实际发送数据字节数]
FAST_API int FastUdpSend(const char* name, const char* ipTarget, unsigned short portTarget, void* pSend, int uSend);

// UDP接收数据
// name：UDP名称[唯一标记]
// ipTarget：目标IP
// portTarget：目标端口
// pRecv：从目标接收的数据
// uRecv：从目标接收的数据字节数
// 返回值：<0	失败[-1：参数错误|-2：套接字错误|-3：套接字安全关闭]
//		   >=0	成功[实际发送数据字节数]
FAST_API int FastUdpRecv(const char* name, const char* ipTarget, unsigned short portTarget, void* pRecv, int uRecv);


////////////////////////////////////////////////////////////////////////////////
// 串口通讯
////////////////////////////////////////////////////////////////////////////////

// 打开串口通讯
// port：串口逻辑编号[唯一标记]
// uBaudRate：波特率[9600]
// uByteSize：字节位数[4-8]
// uParity：奇偶校验[0：NOPARITY|1：ODDPARITY|2：EVENPARITY|3：MARKPARITY|4：SPACEPARITY]
// uStopBits：停止位[0：ONESTOPBIT|1：ONE5STOPBITS|2：TWOSTOPBITS]
// uRead：读缓冲区
// uWrite：写缓冲区
// 返回值：<0	失败[-1：参数错误|-2：打开失败]
//		   >=0	成功
FAST_API int FastCommOpen(unsigned int port, 
	unsigned long uBaudRate = 9600, unsigned char uByteSize = 8,
	unsigned char uParity = NOPARITY, unsigned char uStopBits = ONESTOPBIT,
	unsigned long uRead = 1024, unsigned long uWrite = 1024);

// 关闭串口通讯
// port：串口逻辑编号[唯一标记]
// 返回值：空
FAST_API void FastCommClose(unsigned int port);

// 串口发送数据
// port：串口逻辑编号[唯一标记]
// pSend：发送的数据[>0：数据|<0：十六进制文本文件路径]
// uSend：发送的数据字节数[>0：数据字节数|<0：文本第几行]
// isClear：发送前清理缓冲区
// 返回值：<0	失败[-1：参数错误|-2：通讯错误|-3：清理缓存错误]
//		   >=0	成功[实际发送数据字节数]
FAST_API int FastCommSend(unsigned int port, void* pSend, int uSend, bool isClear = false);

// 串口接收数据
// port：串口逻辑编号[唯一标记]
// pRecv：接收的数据
// uRecv：接收的实际数据字节数
// isClear：接收后清理缓冲区
// 返回值：<0	失败[-1：参数错误|-2：通讯错误|-3：清理缓存错误]
//		   >=0	成功[实际发送接收字节数]
FAST_API int FastCommRecv(unsigned int port, void* pRecv, int uRecv, bool isClear = false);


////////////////////////////////////////////////////////////////////////////////
// EMail
////////////////////////////////////////////////////////////////////////////////

// 遍历EMail回调
// email：EMail地址
// subject：标题
// date：日期
// filenames：模块名称数组[NULL：正文文本|非NULL：附件文件名称]
// streams：内容数组[正文：字符串|附件：数据流]
// lengths：内容字节数
// uPartCount：邮件模块数量
// 返回值：<0	删除该邮件
//		   =0	不处理
typedef int (CALLBACK *FastCallBackMailRecv)(const char* email, const char* subject, const char* date, 
	const char** filenames, const char** streams, unsigned int* lengths, unsigned int uPartCount);

// 发送EMail
// host：邮箱站点地址[例：smtp.126.com]
// username：EMail账户
// password：EMail密码
// email：EMail地址
// targetEmail：目标EMail地址[以split分割多个]
// subject：标题
// content：内容
// attachment：附件[以split分割多个]
// split：字符串分割符
// 返回值：<0	失败[-1：参数错误|-2：发送失败]
//		   >=0	成功
FAST_API int FastSmtpEMailSend(const char* host, const char* username, const char* password,
	const char* email, const char* emailTarget, const char* subject, const char* content, 
	const char* attachment = NULL, const char split = ';');

// 接收EMail[需要在邮箱开启POP3协议]
// host：邮箱站点地址[例：pop3.126.com]
// username：EMail账户
// password：EMail密码[授权密码]
// cbMailRecv：遍历EMail的回调函数
// targetEmail：目标EMail地址[只接收该地址的邮件]
// filterSubject：标题过滤[只接收标题包含该参数的邮件]
// theLastFewTimes：次数过滤[只接收最近多少封邮件]
// 返回值：<0	失败[-1：参数错误|-2：接收失败]
//		   >=0	成功
FAST_API int FastPop3EMailRecv(const char* host, const char* username, const char* password, FastCallBackMailRecv cbMailRecv, 
	const char* targetEmail = NULL, const char* filterSubject = NULL, unsigned int theLastFewTimes = 0);
