#ifndef MACHINE_DATA_IMPL_H
#define MACHINE_DATA_IMPL_H

#ifdef WIN32
#include "stdafx.h"
#include <windows.h>
#else
#include "windows_compat.h"
#endif
#include <stdio.h>
#include "MachineInterface.h"

class CMachineDataInputImpl : public CMachineDataInput
{
public:
	CMachineDataInputImpl(HANDLE hFile);
	CMachineDataInputImpl(BYTE * pbyBuffer, DWORD dwBufferLen);
	virtual ~CMachineDataInputImpl();
	
	virtual void Read(void* pmem, int const n_size);
	
private:
	const HANDLE m_hFile;
	const BYTE * m_pbyBuffer;
	DWORD m_dwBufferLen;
};

class CMachineDataOutputImpl : public CMachineDataOutput
{
public:
	CMachineDataOutputImpl(HANDLE hFile);
	CMachineDataOutputImpl(void);
	DWORD GetCount() const;
	const BYTE * GetOutputBuffer() const;
	virtual ~CMachineDataOutputImpl();
private:
	HANDLE m_hFile;
	BYTE * m_pbyBuffer;
	DWORD m_dwBufferLen;
};


#endif // BUZZ_MACHINE_DATA_H
