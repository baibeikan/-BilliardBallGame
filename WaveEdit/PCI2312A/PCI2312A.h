#ifndef _PCI2312A_DEVICE_
#define _PCI2312A_DEVICE_

//***********************************************************
// 用于INT硬件参数
typedef struct _PCI2312A_PARA_INT
{
	LONG lINTDrection;      // 中断方向:0:下降沿触发中断 1:上升沿触发中断
	LONG lIntEnable[16];	// 控制16个通道中断使能, TRUE：使能;FALSE:禁止
	ULONG lINTTarCount[16];	// 中断目标数，当lINTEndCount>0时 通道中断达到目标数后禁止该通道中断
} PCI2312A_PARA_INT, *PPCI2312A_PARA_INT;

typedef struct _PCI2312A_INT_STATE
{
	SHORT INTTime[8];		// 中断时间
	WORD INTSrc;			// 中断时各通道中断状态
	WORD INTDIVal;			// 中断时各通道状态
	LONG INTCount[16];		// 中断时各通道中断数
} PCI2312A_INT_STATE, *PPCI2312A_INT_STATE;


//######################## 常量定义 #################################
// CreateFileObject所用的文件操作方式控制字(可通过或指令实现多种方式并操作)
#define PCI2312A_modeRead				0x0000		// 只读文件方式
#define PCI2312A_modeWrite				0x0001		// 只写文件方式
#define	PCI2312A_modeReadWrite			0x0002		// 既读又写文件方式
#define PCI2312A_modeCreate				0x1000		// 如果文件不存可以创建该文件，如果存在，则重建此文件，并清0
#define PCI2312A_typeText				0x4000		// 以文本方式操作文件

//***********************************************************

//***********************************************************
// 用户函数接口
#ifndef _PCI2312A_DRIVER_
#define DEVAPI __declspec(dllimport)
#else
#define DEVAPI __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
	//######################## 常规通用函数 #################################
	// 适用于本设备的最基本操作
	HANDLE DEVAPI FAR PASCAL PCI2312A_CreateDevice(int DeviceID = 0);		// 创建设备对象
	int DEVAPI FAR PASCAL PCI2312A_GetDeviceCount(HANDLE hDevice);			// 取得设备总台数
	int DEVAPI FAR PASCAL PCI2312A_GetDeviceCurrentID(HANDLE hDevice);
	BOOL DEVAPI FAR PASCAL PCI2312A_ListDeviceDlg(HANDLE hDevice);			// 列表系统当中的所有的该PCI设备
    BOOL DEVAPI FAR PASCAL PCI2312A_ReleaseDevice(HANDLE hDevice);			// 关闭设备,禁止传输,且释放资源

	//####################### 数字I/O输入输出函数 #################################
	// 用户可以使用WriteRegisterULong和ReadRegisterULong等函数直接控制寄存器进行I/O
	// 输入输出，但使用下面两个函数更省事，它不需要您关心寄存器分配和位操作等，而只
	// 需象VB等语言的属性操作那么简单地实现各开关量通道的控制。
	BOOL DEVAPI FAR PASCAL PCI2312A_SetDeviceDO(            // 设置数字量输出状态     
										HANDLE hDevice,     // 设备句柄								        
										BYTE bDOSts[16]);	// 开关状态

	BOOL DEVAPI FAR PASCAL PCI2312A_GetDeviceDO(            // 取得数字量输出状态     
										HANDLE hDevice,     // 设备句柄								        
										BYTE bDISts[16]);	// 开关状态	


	BOOL DEVAPI FAR PASCAL PCI2312A_GetDeviceDI(            // 取得数字量输入状态     
										HANDLE hDevice,     // 设备句柄								        
										BYTE bDISts[16]);	// 开关状态	

	//####################### 中断函数 #################################
	// 它由硬件信号的状态变化引起CPU产生中断事件hEventInt。
	BOOL DEVAPI FAR PASCAL PCI2312A_InitDeviceInt(			// 初始化中断
										HANDLE hDevice,     // 设备句柄	
										HANDLE hEventInt,	// 中断事件
										PPCI2312A_PARA_INT pINTPara);	//  硬件参数, 它仅在此函数中决定硬件状态

	ULONG DEVAPI FAR PASCAL PCI2312A_GetDeviceIntCount(HANDLE hDevice,	// 在中断初始化后，取得指定通道产生的次数
										LONG lChannel = 0);				// 通道号[0~15]

	BOOL DEVAPI FAR PASCAL PCI2312A_ClearIntCount(HANDLE hDevice,		// 对指定开关量输入通道的中断次数清零，并使能该通道中断
										LONG lChannel = 0);				// 通道号[0~15]

	BOOL DEVAPI FAR PASCAL PCI2312A_GetIntSrc(HANDLE hDevice,			// 取得各通道中断状态
										LONG lChannelInt[16]);			// 各通道中断状态

	ULONG DEVAPI FAR PASCAL PCI2312A_GetIntStateCount(HANDLE hDevice);	// 取得当前驱动中剩余中断状态数，最大2048，超出覆盖


	LONG DEVAPI FAR PASCAL PCI2312A_ReadINTState(						// 读取中断状态
										HANDLE hDevice,					// 设备句柄	
										PCI2312A_INT_STATE pINTState[],	// 返回中断状态
										ULONG lReadCount);				// 读取中断状态数

	BOOL DEVAPI FAR PASCAL PCI2312A_ReleaseDeviceInt(HANDLE hDevice); // 释放中断资源

	//################# 内存映射寄存器直接操作及读写函数 ########################
	// 适用于用户对本设备更直接、更特殊、更低层、更复杂的控制。比如根据特殊的
	// 控制对象需要特殊的控制流程和控制效率时，则用户可以使用这些接口予以实现。
	BOOL DEVAPI FAR PASCAL PCI2312A_GetDeviceBar(					// 取得指定的指定设备寄存器组BAR地址
											HANDLE hDevice,			// 设备对象句柄,它由CreateDevice函数创建
											__int64 pbPCIBar[6]);	// 返回PCI BAR所有地址,具体PCI BAR中有多少可用地址请看硬件说明书

	BOOL DEVAPI FAR PASCAL PCI2312A_GetDevVersion(					// 获取设备固件及程序版本
											HANDLE hDevice,			// 设备对象句柄,它由CreateDevice函数创建
											PULONG pulFmwVersion,	// 固件版本
											PULONG pulDriverVersion);// 驱动版本

	BOOL DEVAPI FAR PASCAL PCI2312A_WriteRegisterByte(			// 往指定寄存器空间位置写入单节字数据
											HANDLE hDevice,			// 设备对象句柄,它由CreateDevice函数创建
											__int64 pbLinearAddr,	// 指定寄存器的线性基地址,它等于GetDeviceAddr中的pbLinearAddr参数返回值
											ULONG OffsetBytes,		// 相对于线性基地址基地址的偏移位置(字节)
											BYTE Value);			// 往指定地址写入单字节数据（其地址由线性基地址和偏移位置决定）

	BOOL DEVAPI FAR PASCAL PCI2312A_WriteRegisterWord(			// 写双字节数据（其余同上）
											HANDLE hDevice, 
											__int64 pbLinearAddr, 
											ULONG OffsetBytes,  
											WORD Value);

	BOOL DEVAPI FAR PASCAL PCI2312A_WriteRegisterULong(			// 写四节字数据（其余同上）
											HANDLE hDevice, 
											__int64 pbLinearAddr, 
											ULONG OffsetBytes,  
											ULONG Value);

	BYTE DEVAPI FAR PASCAL PCI2312A_ReadRegisterByte(			// 读入单字节数据（其余同上）
											HANDLE hDevice, 
											__int64 pbLinearAddr, 
											ULONG OffsetBytes);

	WORD DEVAPI FAR PASCAL PCI2312A_ReadRegisterWord(			// 读入双字节数据（其余同上）
											HANDLE hDevice, 
											__int64 pbLinearAddr, 
											ULONG OffsetBytes);

	ULONG DEVAPI FAR PASCAL PCI2312A_ReadRegisterULong(			// 读入四字节数据（其余同上）
											HANDLE hDevice, 
											__int64 pbLinearAddr, 
											ULONG OffsetBytes);

	//################# I/O端口直接操作及读写函数 ########################
	// 适用于用户对本设备更直接、更特殊、更低层、更复杂的控制。比如根据特殊的
	// 控制对象需要特殊的控制流程和控制效率时，则用户可以使用这些接口予以实现。
	// 但这些函数主要适用于传统设备，如ISA总线、并口、串口等设备，不能用于本PCI设备
	BOOL DEVAPI FAR PASCAL PCI2312A_WritePortByte(HANDLE hDevice, __int64 pbPort, BYTE Value);
    BOOL DEVAPI FAR PASCAL PCI2312A_WritePortWord(HANDLE hDevice, __int64 pbPort, WORD Value);
    BOOL DEVAPI FAR PASCAL PCI2312A_WritePortULong(HANDLE hDevice, __int64 pbPort, ULONG Value);

    BYTE DEVAPI FAR PASCAL PCI2312A_ReadPortByte(HANDLE hDevice, __int64 pbPort);
    WORD DEVAPI FAR PASCAL PCI2312A_ReadPortWord(HANDLE hDevice, __int64 pbPort);
    ULONG DEVAPI FAR PASCAL PCI2312A_ReadPortULong(HANDLE hDevice, __int64 pbPort);

	//########################### 线程操作函数 ######################################
	HANDLE DEVAPI FAR PASCAL PCI2312A_CreateSystemEvent(void);			// 创建内核事件对象，供InitDeviceInt和VB子线程等函数使用
	BOOL DEVAPI FAR PASCAL PCI2312A_ReleaseSystemEvent(HANDLE hEvent);	// 释放内核事件对象


#ifdef __cplusplus
}
#endif

// 自动包含驱动函数导入库
#ifndef _PCI2312A_DRIVER_
	#ifndef _WIN64
		#pragma comment(lib, "PCI2312A/PCI2312A_32.lib")
		#pragma message("======== Welcome to use our art company's products!")
		#pragma message("======== Automatically linking with PCI2312A_32.dll...")
		#pragma message("======== Successfully linked with PCI2312A_32.dll")
	#else
		#pragma comment(lib, "PCI2312A/PCI2312A_64.lib")
		#pragma message("======== Welcome to use our art company's products!")
		#pragma message("======== Automatically linking with PCI2312A_64.dll...")
		#pragma message("======== Successfully linked with PCI2312A_64.dll")
	#endif

#endif

#endif // _PCI2312A_DEVICE_
