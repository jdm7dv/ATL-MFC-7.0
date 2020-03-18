
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

/////////////////////////////////////////////////////////////////////////////
//
// Implementation of parameterized Array
//
/////////////////////////////////////////////////////////////////////////////
// NOTE: we allocate an array of 'm_nMaxSize' elements, but only
//  the current size 'm_nSize' contains properly constructed
//  objects.

#include "stdafx.h"

#ifdef AFX_COLL_SEG
#pragma code_seg(AFX_COLL_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

#pragma warning(disable: 4244)  //REVIEW: new[]

/////////////////////////////////////////////////////////////////////////////

CDWordArray::CDWordArray()
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

CDWordArray::~CDWordArray()
{
	ASSERT_VALID(this);

	delete[] (BYTE*)m_pData;
}

void CDWordArray::SetSize(INT_PTR nNewSize, INT_PTR nGrowBy)
{
	ASSERT_VALID(this);
	ASSERT(nNewSize >= 0);

	if (nGrowBy != -1)
		m_nGrowBy = nGrowBy;  // set new size

	if (nNewSize == 0)
	{
		// shrink to nothing
		delete[] (BYTE*)m_pData;
		m_pData = NULL;
		m_nSize = m_nMaxSize = 0;
	}
	else if (m_pData == NULL)
	{
		// create one with exact size
#ifdef SIZE_T_MAX
		ASSERT(nNewSize <= SIZE_T_MAX/sizeof(DWORD));    // no overflow
#endif
		m_pData = (DWORD*) new BYTE[nNewSize * sizeof(DWORD)];

		memset(m_pData, 0, nNewSize * sizeof(DWORD));  // zero fill

		m_nSize = m_nMaxSize = nNewSize;
	}
	else if (nNewSize <= m_nMaxSize)
	{
		// it fits
		if (nNewSize > m_nSize)
		{
			// initialize the new elements

			memset(&m_pData[m_nSize], 0, (nNewSize-m_nSize) * sizeof(DWORD));

		}

		m_nSize = nNewSize;
	}
	else
	{
		// otherwise, grow array
		INT_PTR nGrowBy = m_nGrowBy;
		if (nGrowBy == 0)
		{
			// heuristically determine growth when nGrowBy == 0
			//  (this avoids heap fragmentation in many situations)
			nGrowBy = min(1024, max(4, m_nSize / 8));
		}
		INT_PTR nNewMax;
		if (nNewSize < m_nMaxSize + nGrowBy)
			nNewMax = m_nMaxSize + nGrowBy;  // granularity
		else
			nNewMax = nNewSize;  // no slush

		ASSERT(nNewMax >= m_nMaxSize);  // no wrap around
#ifdef SIZE_T_MAX
		ASSERT(nNewMax <= SIZE_T_MAX/sizeof(DWORD)); // no overflow
#endif
		DWORD* pNewData = (DWORD*) new BYTE[nNewMax * sizeof(DWORD)];

		// copy new data from old
		memcpy(pNewData, m_pData, m_nSize * sizeof(DWORD));

		// construct remaining elements
		ASSERT(nNewSize > m_nSize);

		memset(&pNewData[m_nSize], 0, (nNewSize-m_nSize) * sizeof(DWORD));


		// get rid of old stuff (note: no destructors called)
		delete[] (BYTE*)m_pData;
		m_pData = pNewData;
		m_nSize = nNewSize;
		m_nMaxSize = nNewMax;
	}
}

INT_PTR CDWordArray::Append(const CDWordArray& src)
{
	ASSERT_VALID(this);
	ASSERT(this != &src);   // cannot append to itself

	INT_PTR nOldSize = m_nSize;
	SetSize(m_nSize + src.m_nSize);

	memcpy(m_pData + nOldSize, src.m_pData, src.m_nSize * sizeof(DWORD));

	return nOldSize;
}

void CDWordArray::Copy(const CDWordArray& src)
{
	ASSERT_VALID(this);
	ASSERT(this != &src);   // cannot append to itself

	SetSize(src.m_nSize);

	memcpy(m_pData, src.m_pData, src.m_nSize * sizeof(DWORD));

}

void CDWordArray::FreeExtra()
{
	ASSERT_VALID(this);

	if (m_nSize != m_nMaxSize)
	{
		// shrink to desired size
#ifdef SIZE_T_MAX
		ASSERT(m_nSize <= SIZE_T_MAX/sizeof(DWORD)); // no overflow
#endif
		DWORD* pNewData = NULL;
		if (m_nSize != 0)
		{
			pNewData = (DWORD*) new BYTE[m_nSize * sizeof(DWORD)];
			// copy new data from old
			memcpy(pNewData, m_pData, m_nSize * sizeof(DWORD));
		}

		// get rid of old stuff (note: no destructors called)
		delete[] (BYTE*)m_pData;
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

/////////////////////////////////////////////////////////////////////////////

void CDWordArray::SetAtGrow(INT_PTR nIndex, DWORD newElement)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0);

	if (nIndex >= m_nSize)
		SetSize(nIndex+1);
	m_pData[nIndex] = newElement;
}





void CDWordArray::InsertAt(INT_PTR nIndex, DWORD newElement, INT_PTR nCount)
{

	ASSERT_VALID(this);
	ASSERT(nIndex >= 0);    // will expand to meet need
	ASSERT(nCount > 0);     // zero or negative size not allowed

	if (nIndex >= m_nSize)
	{
		// adding after the end of the array
		SetSize(nIndex + nCount);  // grow so nIndex is valid
	}
	else
	{
		// inserting in the middle of the array
		INT_PTR nOldSize = m_nSize;
		SetSize(m_nSize + nCount);  // grow it to new size
		// shift old data up to fill gap
		memmove(&m_pData[nIndex+nCount], &m_pData[nIndex],
			(nOldSize-nIndex) * sizeof(DWORD));

		// re-init slots we copied from

		memset(&m_pData[nIndex], 0, nCount * sizeof(DWORD));

	}

	// insert new value in the gap
	ASSERT(nIndex + nCount <= m_nSize);



	// copy elements into the empty space
	while (nCount--)
		m_pData[nIndex++] = newElement;

}



void CDWordArray::RemoveAt(INT_PTR nIndex, INT_PTR nCount)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0);
	ASSERT(nCount >= 0);
	ASSERT(nIndex + nCount <= m_nSize);

	// just remove a range
	INT_PTR nMoveCount = m_nSize - (nIndex + nCount);

	if (nMoveCount)
		memmove(&m_pData[nIndex], &m_pData[nIndex + nCount],
			nMoveCount * sizeof(DWORD));
	m_nSize -= nCount;
}

void CDWordArray::InsertAt(INT_PTR nStartIndex, CDWordArray* pNewArray)
{
	ASSERT_VALID(this);
	ASSERT(pNewArray != NULL);
	ASSERT_KINDOF(CDWordArray, pNewArray);
	ASSERT_VALID(pNewArray);
	ASSERT(nStartIndex >= 0);

	if (pNewArray->GetSize() > 0)
	{
		InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
		for (INT_PTR i = 0; i < pNewArray->GetSize(); i++)
			SetAt(nStartIndex + i, pNewArray->GetAt(i));
	}
}




/////////////////////////////////////////////////////////////////////////////
// Serialization

void CDWordArray::Serialize(CArchive& ar)
{
	UINT_PTR nDWORDsLeft;
	UINT nDWORDsToWrite;
	UINT nDWORDsToRead;
	DWORD* pdwData;

	ASSERT_VALID(this);

	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		ar.WriteCount(m_nSize);
#ifdef _AFX_BYTESWAP
		if (!ar.IsByteSwapping())
#endif
		{
			nDWORDsLeft = m_nSize;
			pdwData = m_pData;
			while(nDWORDsLeft > 0)
			{
				nDWORDsToWrite = UINT(min(nDWORDsLeft, INT_MAX/sizeof(DWORD)));
				ar.Write(pdwData, nDWORDsToWrite*sizeof(DWORD));
				nDWORDsLeft -= nDWORDsToWrite;
				pdwData += nDWORDsToWrite;
			}
		}
#ifdef _AFX_BYTESWAP
		else
		{
			// write each item individually so that it will be byte-swapped
			for (INT_PTR i = 0; i < m_nSize; i++)
				ar << m_pData[i];
		}
#endif
	}
	else
	{
		DWORD_PTR nOldSize = ar.ReadCount();
		SetSize(nOldSize);
		nDWORDsLeft = m_nSize;
		pdwData = m_pData;
		while(nDWORDsLeft > 0)
		{
			nDWORDsToRead = UINT(min(nDWORDsLeft, INT_MAX/sizeof(DWORD)));
			ar.Read(pdwData, nDWORDsToRead*sizeof(DWORD));
			nDWORDsLeft -= nDWORDsToRead;
			pdwData += nDWORDsToRead;
		}
#ifdef _AFX_BYTESWAP
		if (ar.IsByteSwapping())
		{
			for (INT_PTR i = 0; i < m_nSize; i++)
				_AfxByteSwap(m_pData[i], (BYTE*)&m_pData[i]);
		}
#endif
	}
}

/////////////////////////////////////////////////////////////////////////////
// Diagnostics

#ifdef _DEBUG
void CDWordArray::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);

	dc << "with " << m_nSize << " elements";
	if (dc.GetDepth() > 0)
	{
		for (INT_PTR i = 0; i < m_nSize; i++)
			dc << "\n\t[" << i << "] = " << m_pData[i];
	}

	dc << "\n";
}

void CDWordArray::AssertValid() const
{
	CObject::AssertValid();

	if (m_pData == NULL)
	{
		ASSERT(m_nSize == 0);
		ASSERT(m_nMaxSize == 0);
	}
	else
	{
		ASSERT(m_nSize >= 0);
		ASSERT(m_nMaxSize >= 0);
		ASSERT(m_nSize <= m_nMaxSize);
		ASSERT(AfxIsValidAddress(m_pData, m_nMaxSize * sizeof(DWORD)));
	}
}
#endif //_DEBUG

#ifdef AFX_INIT_SEG
#pragma code_seg(AFX_INIT_SEG)
#endif


IMPLEMENT_SERIAL(CDWordArray, CObject, 0)

/////////////////////////////////////////////////////////////////////////////