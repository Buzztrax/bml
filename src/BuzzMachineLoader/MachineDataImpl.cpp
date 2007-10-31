#include "MachineInterface.h"
#include "MachineDataImpl.h"
#include "debug.h"

// virtual 'null' base implementation of CMachineDataInput and CMachineDataOutput
void CMachineDataInput::Read(void * pmem, const int n_size) {
	// clear ...
    DBG2("(pbuf=%p,numbytes=%d) : no buffer\n",pmem,n_size);
	memset(pmem, 0, n_size);
}
void CMachineDataOutput::Write(void * pmem, const int n_size) {
	// do nothing ...
    DBG2("(pbuf=%p,numbytes=%d) : no buffer\n",pmem,n_size);
}


// Implementation of Read/Write classes for internal machine data load/save operations

CMachineDataInputImpl::CMachineDataInputImpl(HANDLE hFile) : m_hFile(hFile), m_pbyBuffer(NULL), m_dwBufferLen(0) {};
CMachineDataInputImpl::CMachineDataInputImpl(BYTE * pbyBuffer, DWORD dwBufferLen) : m_hFile(NULL), m_pbyBuffer(pbyBuffer), m_dwBufferLen(dwBufferLen) {};
CMachineDataInputImpl::~CMachineDataInputImpl() {
	//
}
	
void CMachineDataInputImpl::Read(void* pmem, const int n_size) {
	if (m_pbyBuffer) {
		if (m_dwBufferLen >= (DWORD)n_size) {
			memcpy(pmem, m_pbyBuffer, n_size);
			m_pbyBuffer += n_size;
			m_dwBufferLen -= n_size;
		}
		else {
            DBG2("(pbuf=%p,numbytes=%d) : out of buffer\n",pmem,n_size);
			memcpy(pmem, m_pbyBuffer, m_dwBufferLen);
			m_pbyBuffer += m_dwBufferLen;
			m_dwBufferLen = 0;
		}
	}
	else if (m_hFile) {
		// clear ...
        DBG2("(pbuf=%p,numbytes=%d) : no file\n",pmem,n_size);
		memset(pmem, 0, n_size);
	}
	else {
		// clear ...
        DBG2("(pbuf=%p,numbytes=%d) : no buffer\n",pmem,n_size);
		memset(pmem, 0, n_size);
	}
}
	


CMachineDataOutputImpl::CMachineDataOutputImpl(HANDLE hFile) : m_hFile(hFile), m_pbyBuffer(NULL), m_dwBufferLen(0) {};
CMachineDataOutputImpl::CMachineDataOutputImpl(void) : m_hFile(NULL), m_pbyBuffer(NULL), m_dwBufferLen(0) {};

DWORD CMachineDataOutputImpl::GetCount() const {
	return m_dwBufferLen;
}

const BYTE * CMachineDataOutputImpl::GetOutputBuffer() const {
	return m_pbyBuffer;
}

CMachineDataOutputImpl::~CMachineDataOutputImpl() {
	///
}
