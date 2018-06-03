// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� FAST_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// FAST_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef FAST_EXPORTS
#define FAST_API __declspec(dllexport)
#else
#define FAST_API __declspec(dllimport)
#endif


////////////////////////////////////////////////////////////////////////////////
// TCP������
////////////////////////////////////////////////////////////////////////////////

// TCP���������տͻ������ӻص�
// pAddr����ǰ�ͻ��˵�ַ�ַ���[xxx.xxx.xxx.xxx:port]
// pExample��FastTcpServerOpen����pExample�������ݵ��������ʵ��ָ��
// ����ֵ����
typedef void (CALLBACK *FastCallBackConect)(const char* pAddr, void* pExample);

// TCP�������Ͽ��ͻ������ӻص�
// pAddr����ǰ�ͻ��˵�ַ�ַ���[xxx.xxx.xxx.xxx:port]
// exit���˳�����[0����ȫ�ر�]
// pExample��FastTcpServerOpen����pExample�������ݵ��������ʵ��ָ��
// ����ֵ����
typedef void (CALLBACK *FastCallBackDisconect)(const char* pAddr, int exit, void* pExample);

// TCP���������տͻ������ݻص�
// pAddr����ǰ�ͻ��˵�ַ�ַ���[xxx.xxx.xxx.xxx:port]
// pRecv���ӵ�ǰ�ͻ��˽��յ�����
// uRecv���ӵ�ǰ�ͻ��˽��յ����ݳ���
// pExample��FastTcpServerOpen����pExample�������ݵ��������ʵ��ָ��
// pOutSend���ش�����ǰ�ͻ��˵�����[�û���д]
//			 ����ֵ=0		������
//			 ����ֵ>0		��䷢�͸��ͻ��˵�������
// ����ֵ��[�û���д]
//		   =0		����ͻ��˷�������
//		   >0		���͸��ͻ��˵������ֽ���
typedef int (CALLBACK *FastCallBackTcpServerQA)(const char* pAddr, void* pRecv, int uRecv, void* pExample, void* pOutSend);

// TCP���������͸��ͻ������ݻص�
// pAddr����ǰ�ͻ��˵�ַ�ַ���[xxx.xxx.xxx.xxx:port]
// pExample��FastTcpServerOpen����pExample�������ݵ��������ʵ��ָ��
// pOutSend���ش�����ǰ�ͻ��˵�����[�û���д]
//			 ����ֵ=0	������
//			 ����ֵ>0	��䷢�͸��ͻ��˵�������
// ����ֵ��[�û���д]
//		   =0		����ͻ��˷�������
//		   >0		���͸��ͻ��˵������ֽ���
typedef int (CALLBACK *FastCallBackTcpServerSend)(const char* pAddr, void* pExample, void* pOutSend);

// TCP���������տͻ������ݻص�
// pAddr����ǰ�ͻ��˵�ַ�ַ���[xxx.xxx.xxx.xxx:port]
// pRecv���ӵ�ǰ�ͻ��˽��յ�����
// uRecv���ӵ�ǰ�ͻ��˽��յ����ݳ���
// pExample��FastTcpServerOpen����pExample�������ݵ��������ʵ��ָ��
// ����ֵ����
typedef void (CALLBACK *FastCallBackTcpServerRecv)(const char* pAddr, void* pRecv, int uRecv, void* pExample);

// ��TCP������[�ȴ��ͻ�������ģʽ]
// name������������[Ψһ���]
// port���������˿�
// pExample�����ݸ��ص��������������ʵ��ָ��
// cbRun�����յ��ͻ������ݺ�����䷢�͸��ͻ������ݵĻص�����
// cbConect���ͻ��˶Ͻ���ص�����
// cbDisconect���ͻ��˶϶Ͽ��ص�����
// ����ֵ��<0	ʧ��[-1����������|-2����������������]
//		   >=0	�ɹ�
FAST_API int FastTcpServerOpen(const char* name, unsigned short port, void* pExample,
	FastCallBackTcpServerQA cbRun, FastCallBackConect cbConect = NULL, FastCallBackDisconect cbDisconect = NULL);

// ��TCP������[��������������ģʽ]
// name������������[Ψһ���]
// port���������˿�
// pExample�����ݸ��ص��������������ʵ��ָ��
// cbSend����䷢�͸��ͻ������ݵĻص�����
// cbRecv�����յ��ͻ������ݺ���Ļص�����
// cbConect���ͻ��˽���ص�����
// cbDisconect���ͻ��˶Ͽ��ص�����
// uTestConnectionTime�������ݽ���ʱ��������жϵ�ʱ��[��λ������]
// ����ֵ��<0	ʧ��[-1����������|-2����������������]
//		   >=0	�ɹ�
FAST_API int FastTcpServerOpen(const char* name, unsigned short port, void* pExample,
	FastCallBackTcpServerSend cbSend, FastCallBackTcpServerRecv cbRecv = NULL, 
	FastCallBackConect cbConect = NULL, FastCallBackDisconect cbDisconect = NULL,
	unsigned int uTestConnectionTime = 1000);

// �ر�TCP������
// name������������[Ψһ���]
// ����ֵ����
FAST_API void FastTcpServerClose(const char* name);

// TCP�������Ƿ��
// name������������[Ψһ���]
// ����ֵ��TCP�������Ƿ��
FAST_API bool FastHasTcpServer(const char* name);


////////////////////////////////////////////////////////////////////////////////
// TCP�ͻ���
////////////////////////////////////////////////////////////////////////////////

// TCP�ͻ������ӷ�����
// name���ͻ�������[Ψһ���]
// ip��Ŀ�������IP
// port��Ŀ��������˿�
// ����ֵ��<0	ʧ��[-1����������|-2���ͻ������Ӵ���]
//		   >=0	�ɹ�
FAST_API int FastTcpClientOpen(const char* name, const char* ip, unsigned short port);

// TCP�ͻ��˶Ͽ�������
// name���ͻ�������[Ψһ���]
// ����ֵ����
FAST_API void FastTcpClientClose(const char* name);

// TCP�ͻ��˷�������
// name���ͻ�������[Ψһ���]
// pSend������������͵�����
// uSend������������͵������ֽ���
// ����ֵ��<0	ʧ��[-1����������|-2���׽��ִ���|-3���׽��ְ�ȫ�ر�]
//		   >=0	�ɹ�[ʵ�ʷ��������ֽ���]
FAST_API int FastTcpClientSend(const char* name, void* pSend, int uSend);

// TCP�ͻ��˽�������
// name���ͻ�������[Ψһ���]
// pRecv���ӷ��������յ�����
// uRecv���ӷ��������յ������ֽ���
// ����ֵ��<0	ʧ��[-1����������|-2���׽��ִ���|-3���׽��ְ�ȫ�ر�]
//		   >=0	�ɹ�[ʵ�ʷ��������ֽ���]
FAST_API int FastTcpClientRecv(const char* name, void* pRecv, int uRecv);

// TCP�ͻ������ӷ�����ֱ���ɹ�
// name���ͻ�������[Ψһ���]
// ip��Ŀ�������IP
// port��Ŀ��������˿�
// pExit�������û�ǿ�ƽ������¼�
// milliseconds��ÿ������ʧ�ܵȴ���ʱ��
// ����ֵ��<0	ʧ��[-1����������|-2���ͻ������Ӵ���|-3���û�ǿ�ƽ���]
//		   >=0	�ɹ�
FAST_API int FastTcpClientOpenUntilSuccess(const char* name, const char* ip, unsigned short port, 
	void* pExit = NULL, long milliseconds = 500);


////////////////////////////////////////////////////////////////////////////////
// UDP������
////////////////////////////////////////////////////////////////////////////////

// UDP���������տͻ������ݻص�[���յ����ݴ���]
// pAddr����ǰ�ͻ��˵�ַ�ַ���[xxx.xxx.xxx.xxx:port]
// pRecv���ӵ�ǰ�ͻ��˽��յ�����
// uRecv���ӵ�ǰ�ͻ��˽��յ������ֽ���
// pExample��FastUdpServerOpen����pExample�������ݵ��������ʵ��ָ��
// pOutSend���ش�����ǰ�ͻ��˵�����[�û���д]
//			 ����ֵ=0	������
//			 ����ֵ>0	��䷢�͸��ͻ��˵�������
//			 ����ֵ<0	��䷢�͸��ͻ��˵�ʮ�������ı�·��
// ����ֵ��[�û���д]
//		   =0	����ͻ��˷�������
//		   >0	���͸��ͻ��˵������ֽ���
//	       <0	���͸��ͻ���ʮ�������ı��ļ��ڼ���[ȡ����ֵ]
typedef int (CALLBACK *FastCallBackUdpServerQA)(const char* pAddr, void* pRecv, unsigned int uRecv, void* pExample, void* pOutSend);

// ��UDP������
// name������������[Ψһ���]
// port���������˿�
// pExample�����ݸ��ص��������������ʵ��ָ��
// cbRun�����յ��ͻ������ݺ�����䷢�͸��ͻ������ݵĻص�����
// ����ֵ��<0	ʧ��[-1����������|-2����������������]
//		   >=0	�ɹ�
FAST_API int FastUdpServerOpen(const char* name, unsigned short port, void* pExample, FastCallBackUdpServerQA cbRun);

// �ر�UDP������
// name������������[Ψһ���]
// ����ֵ����
FAST_API void FastUdpServerClose(const char* name);

// UDP�������Ƿ��
// name������������[Ψһ���]
// ����ֵ��UDP�������Ƿ��
FAST_API bool FastHasUdpServer(const char* name);


////////////////////////////////////////////////////////////////////////////////
// UDPͨѶ
////////////////////////////////////////////////////////////////////////////////

// ��UDPͨѶ
// name��UDP����[Ψһ���]
// port�������˿�
// ����ֵ��<0	ʧ��[-1����������|-2��UDP��������]
//		   >=0	�ɹ�
FAST_API int FastUdpOpen(const char* name, unsigned short port);

// �ر�UDPͨѶ
// name��UDP����[Ψһ���]
// ����ֵ����
FAST_API void FastUdpClose(const char* name);

// UDP��������
// name��UDP����[Ψһ���]
// ipTarget��Ŀ��IP
// portTarget��Ŀ��˿�
// pSend����Ŀ�귢�͵�����[>0������|<0��ʮ�������ı��ļ�·��]
// uSend����Ŀ�귢�͵������ֽ���[>0�������ֽ���|<0���ı��ڼ���]
// ����ֵ��<0	ʧ��[-1����������|-2���׽��ִ���|-3���׽��ְ�ȫ�ر�]
//		   >=0	�ɹ�[ʵ�ʷ��������ֽ���]
FAST_API int FastUdpSend(const char* name, const char* ipTarget, unsigned short portTarget, void* pSend, int uSend);

// UDP��������
// name��UDP����[Ψһ���]
// ipTarget��Ŀ��IP
// portTarget��Ŀ��˿�
// pRecv����Ŀ����յ�����
// uRecv����Ŀ����յ������ֽ���
// ����ֵ��<0	ʧ��[-1����������|-2���׽��ִ���|-3���׽��ְ�ȫ�ر�]
//		   >=0	�ɹ�[ʵ�ʷ��������ֽ���]
FAST_API int FastUdpRecv(const char* name, const char* ipTarget, unsigned short portTarget, void* pRecv, int uRecv);


////////////////////////////////////////////////////////////////////////////////
// ����ͨѶ
////////////////////////////////////////////////////////////////////////////////

// �򿪴���ͨѶ
// port�������߼����[Ψһ���]
// uBaudRate��������[9600]
// uByteSize���ֽ�λ��[4-8]
// uParity����żУ��[0��NOPARITY|1��ODDPARITY|2��EVENPARITY|3��MARKPARITY|4��SPACEPARITY]
// uStopBits��ֹͣλ[0��ONESTOPBIT|1��ONE5STOPBITS|2��TWOSTOPBITS]
// uRead����������
// uWrite��д������
// ����ֵ��<0	ʧ��[-1����������|-2����ʧ��]
//		   >=0	�ɹ�
FAST_API int FastCommOpen(unsigned int port, 
	unsigned long uBaudRate = 9600, unsigned char uByteSize = 8,
	unsigned char uParity = NOPARITY, unsigned char uStopBits = ONESTOPBIT,
	unsigned long uRead = 1024, unsigned long uWrite = 1024);

// �رմ���ͨѶ
// port�������߼����[Ψһ���]
// ����ֵ����
FAST_API void FastCommClose(unsigned int port);

// ���ڷ�������
// port�������߼����[Ψһ���]
// pSend�����͵�����[>0������|<0��ʮ�������ı��ļ�·��]
// uSend�����͵������ֽ���[>0�������ֽ���|<0���ı��ڼ���]
// isClear������ǰ��������
// ����ֵ��<0	ʧ��[-1����������|-2��ͨѶ����|-3�����������]
//		   >=0	�ɹ�[ʵ�ʷ��������ֽ���]
FAST_API int FastCommSend(unsigned int port, void* pSend, int uSend, bool isClear = false);

// ���ڽ�������
// port�������߼����[Ψһ���]
// pRecv�����յ�����
// uRecv�����յ�ʵ�������ֽ���
// isClear�����պ���������
// ����ֵ��<0	ʧ��[-1����������|-2��ͨѶ����|-3�����������]
//		   >=0	�ɹ�[ʵ�ʷ��ͽ����ֽ���]
FAST_API int FastCommRecv(unsigned int port, void* pRecv, int uRecv, bool isClear = false);


////////////////////////////////////////////////////////////////////////////////
// EMail
////////////////////////////////////////////////////////////////////////////////

// ����EMail�ص�
// email��EMail��ַ
// subject������
// date������
// filenames��ģ����������[NULL�������ı�|��NULL�������ļ�����]
// streams����������[���ģ��ַ���|������������]
// lengths�������ֽ���
// uPartCount���ʼ�ģ������
// ����ֵ��<0	ɾ�����ʼ�
//		   =0	������
typedef int (CALLBACK *FastCallBackMailRecv)(const char* email, const char* subject, const char* date, 
	const char** filenames, const char** streams, unsigned int* lengths, unsigned int uPartCount);

// ����EMail
// host������վ���ַ[����smtp.126.com]
// username��EMail�˻�
// password��EMail����
// email��EMail��ַ
// targetEmail��Ŀ��EMail��ַ[��split�ָ���]
// subject������
// content������
// attachment������[��split�ָ���]
// split���ַ����ָ��
// ����ֵ��<0	ʧ��[-1����������|-2������ʧ��]
//		   >=0	�ɹ�
FAST_API int FastSmtpEMailSend(const char* host, const char* username, const char* password,
	const char* email, const char* emailTarget, const char* subject, const char* content, 
	const char* attachment = NULL, const char split = ';');

// ����EMail[��Ҫ�����俪��POP3Э��]
// host������վ���ַ[����pop3.126.com]
// username��EMail�˻�
// password��EMail����[��Ȩ����]
// cbMailRecv������EMail�Ļص�����
// targetEmail��Ŀ��EMail��ַ[ֻ���ոõ�ַ���ʼ�]
// filterSubject���������[ֻ���ձ�������ò������ʼ�]
// theLastFewTimes����������[ֻ����������ٷ��ʼ�]
// ����ֵ��<0	ʧ��[-1����������|-2������ʧ��]
//		   >=0	�ɹ�
FAST_API int FastPop3EMailRecv(const char* host, const char* username, const char* password, FastCallBackMailRecv cbMailRecv, 
	const char* targetEmail = NULL, const char* filterSubject = NULL, unsigned int theLastFewTimes = 0);
