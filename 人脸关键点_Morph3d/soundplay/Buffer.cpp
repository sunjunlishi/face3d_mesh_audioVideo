// Buffer.cpp: implementation of the CBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StdAfx.h"
#include "Buffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBuffer::CBuffer(DWORD size, bool AutoDelete)
{
	m_bAutoDelete = AutoDelete;
	try
	{
		m_chData = new char[size];
		if(m_chData != NULL) 
			ByteLen = size;
	}
	catch(...)
	{
		ErrorMsg("Out of memory!");
	}
	m_bHaveData = FALSE;
}

CBuffer::CBuffer(char* buffer, DWORD length)
{
	m_bAutoDelete = false;
	if(buffer)
	{
		m_chData = buffer;
		ByteLen = length;
	}
	else
	{
		m_chData = NULL;
		ByteLen = 0;
	}
	m_bHaveData = FALSE;
}

CBuffer::~CBuffer()
{
	// remember to delete the memory
	if(m_bAutoDelete && (m_chData != NULL))
		delete []m_chData;
}

void CBuffer::Erase()
{
	if(m_chData != NULL) 
		ZeroMemory(m_chData,ByteLen);
}
