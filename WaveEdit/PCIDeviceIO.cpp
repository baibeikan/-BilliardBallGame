#include "stdafx.h"
#include "PCIDeviceIO.h"

#ifdef __PCI2310__
#include "Pci2310/Pci2310.h"
#define PciCreateDevice(nIndex)  PCI2310_CreateDevice(nIndex)
#define PciGetDeviceDI(handle, arrInput) PCI2310_GetDeviceDI(handle, arrInput)
#define PciReleaseDevice(handle) PCI2310_ReleaseDevice(handle);
#define PciSetDeviceDO(handle, arrOutput) PCI2310_SetDeviceDO(handle, arrOutput)
#else
#ifdef __PCI2312__
#include "PCI2312/PCI2312.h"
#define PciCreateDevice(nIndex)  PCI2312_CreateDevice(nIndex)
#define PciGetDeviceDI(handle, arrInput) PCI2312_GetDeviceDI(handle, arrInput)
#define PciReleaseDevice(handle) PCI2312_ReleaseDevice(handle);
#define PciSetDeviceDO(handle, arrOutput) PCI2312_SetDeviceDO(handle, arrOutput)
#else
#ifdef __PCI2312A__
#include "PCI2312A/PCI2312A.h"
#define PciCreateDevice(nIndex)  PCI2312A_CreateDevice(nIndex)
#define PciGetDeviceDI(handle, arrInput) PCI2312A_GetDeviceDI(handle, arrInput)
#define PciSetDeviceDO(handle, arrOutput) PCI2312A_SetDeviceDO(handle, arrOutput)
#define PciReleaseDevice(handle) PCI2312A_ReleaseDevice(handle);
#endif
#endif
#endif // __PCI2310__

PciDeviceIO::PciDeviceIO()
{
	m_InputDeviceHandle = INVALID_HANDLE_VALUE;
	memset(m_arrLastInput, 0, sizeof(m_arrLastInput));
	memset(m_arrCurInput, 0, sizeof(m_arrCurInput));
}

PciDeviceIO::~PciDeviceIO()
{
	memset(m_arrLastInput, 0, sizeof(m_arrLastInput));
	memset(m_arrCurInput, 0, sizeof(m_arrCurInput));
	ReleaseDevice();
}

void PciDeviceIO::CreateDevice()
{
	m_InputDeviceHandle = PciCreateDevice(0);
	if(!DeviceIsEnable()) { return; }
	PciGetDeviceDI(m_InputDeviceHandle, m_arrCurInput);
	memcpy(m_arrLastInput, m_arrCurInput, min(sizeof(m_arrCurInput), sizeof(m_arrLastInput)));
}

void PciDeviceIO::ReleaseDevice()
{
	if(!DeviceIsEnable()) { return; }

	PciReleaseDevice(m_InputDeviceHandle);
	m_InputDeviceHandle = INVALID_HANDLE_VALUE;
}

void PciDeviceIO::UpdateIO()
{
	if(DeviceIsEnable()) 
	{
		memcpy(m_arrLastInput, m_arrCurInput, min(sizeof(m_arrCurInput), sizeof(m_arrLastInput)));
		PciGetDeviceDI(m_InputDeviceHandle, m_arrCurInput);
	}
}

bool PciDeviceIO::GetInputDown(int _nIndex)
{
	if(_nIndex < 0 || _nIndex >= MAX_PORT) { return false; }

	return m_arrCurInput[_nIndex] > 0;
}

bool PciDeviceIO::GetInputUp(int _nIndex)
{
	if(_nIndex < 0 || _nIndex >= MAX_PORT) { return false; }

	return m_arrCurInput[_nIndex] == 0;
}

bool PciDeviceIO::GetInputClicked(int _nIndex)
{
	if(_nIndex < 0 || _nIndex >= MAX_PORT) { return false; }

	return m_arrLastInput[_nIndex] == 0 && m_arrCurInput[_nIndex] > 0;
}
bool PciDeviceIO::GetInputRelease(int _nIndex)
{
	if(_nIndex < 0 || _nIndex >= MAX_PORT) { return false; }

	return m_arrLastInput[_nIndex] > 0 && m_arrCurInput[_nIndex] == 0;
}

bool PciDeviceIO::DeviceIsEnable()
{
	return m_InputDeviceHandle != INVALID_HANDLE_VALUE;
}

void PciDeviceIO::SetInputDown(int _nIndex)
{
	SetInputVal(_nIndex, true);
}

void PciDeviceIO::SetInputUp(int _nIndex)
{
	SetInputVal(_nIndex, false);
}

void PciDeviceIO::SetInputVal(int _nIndex, bool _bIsDown)
{
	if(_nIndex < 0 || _nIndex >= MAX_PORT) { return; }
}

void PciDeviceIO::SetOutputVal(int _nIndex, bool _bIsDown)
{
	if(_nIndex < 0 || _nIndex >= MAX_PORT) { return; }
	if(DeviceIsEnable())
	{
		BYTE arrOutput[MAX_PORT] = {};
		arrOutput[_nIndex] = _bIsDown ? 1 : 0;
		PciSetDeviceDO(m_InputDeviceHandle, arrOutput);
	}
}
