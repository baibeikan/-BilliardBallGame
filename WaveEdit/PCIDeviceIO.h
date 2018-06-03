#ifndef __DEVICEINPUT_H__
#define __DEVICEINPUT_H__
#include <map>
//#define __PCI2310__
//#define __PCI2312__
#define __PCI2312A__

#ifdef __PCI2310__
#define MAX_PORT (32)
#undef __PCI2312__
#undef __PCI2312A__
#endif // __PCI2310__

#ifdef __PCI2312__
#define MAX_PORT (16)
#undef __PCI2312A__
#endif // __PCI2312__

#ifdef __PCI2312A__
#define MAX_PORT (16)
#endif // __PCI2312A__


class PciDeviceIO
{
public:
	PciDeviceIO();
	~PciDeviceIO();

public:
	void CreateDevice();
	void ReleaseDevice();

	void UpdateIO();

public:
	bool GetInputDown(int _nIndex);

	bool GetInputUp(int _nIndex);

	bool GetInputClicked(int _nIndex);

	bool GetInputRelease(int _nIndex);

	bool DeviceIsEnable();

public:
	void SetInputDown(int _nIndex);

	void SetInputUp(int _nIndex);

	void SetInputVal(int _nIndex, bool _bIsDown);

	void SetOutputVal(int _nIndex, bool _bIsDown);
private:
	byte m_arrLastInput[MAX_PORT];
	byte m_arrCurInput[MAX_PORT];
	HANDLE m_InputDeviceHandle;
};

#endif