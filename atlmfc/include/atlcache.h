// This is a part of the Active Template Library.
// Copyright (C) 1996-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.


#pragma once
#include <atltime.h>
#include <atlutil.h>
#include <atlcoll.h>
#include <atlperf.h>
#include <atlcom.h>
#include <atlstr.h>
//#include <atlsession.h>
#include <atlsrvres.h>
#include <atldbcli.h>

namespace ATL {

//forward declarations;

class CStdStatClass;
class CPerfStatClass;

//Implementation of a cache that stores pointers to void
extern "C" __declspec(selectany) GUID HPSID_BLOBDATACACHE = { 0x86be8692, 0x3887, 0x11d3, { 0x97, 0x7b, 0x0, 0xc0, 0x4f, 0x8e, 0xe2, 0x5e } };
extern "C" __declspec(selectany) IID IID_IMemoryCacheClient = {0xb721b49d, 0xbb57, 0x47bc, { 0xac, 0x43, 0xa8, 0xd4, 0xc0, 0x7d, 0x18, 0x3d } };
extern "C" __declspec(selectany) IID IID_IMemoryCache = { 0x9c6cfb46, 0xfbde, 0x4f8b, { 0xb9, 0x44, 0x2a, 0xa0, 0x5d, 0x96, 0xeb, 0x5c } };
extern "C" __declspec(selectany) IID IID_IMemoryCacheControl = { 0x7634b28b, 0xd819, 0x409d, { 0xb9, 0x6e, 0xfc, 0x9f, 0x3a, 0xba, 0x32, 0x9f } };
extern "C" __declspec(selectany) IID IID_IMemoryCacheStats = { 0xd4b6df2d, 0x4bc0, 0x4734, { 0x8a, 0xce, 0xb7, 0x3a, 0xb, 0x97, 0x59, 0x56 } };

struct ATL_NO_VTABLE __declspec(uuid("b721b49d-bb57-47bc-ac43-a8d4c07d183d")) 
	IMemoryCacheClient : public IUnknown
{
	// IMemoryCacheClient methods
	STDMETHOD( Free )(const void *pvData) = 0;
};

struct ATL_NO_VTABLE __declspec(uuid("9c6cfb46-fbde-4f8b-b944-2aa05d96eb5c")) 
	IMemoryCache : public IUnknown
{
	// IMemoryCache Methods
	STDMETHOD(Add)(LPCSTR szKey, void *pvData, DWORD dwSize, 
				FILETIME *pftExpireTime,
				HINSTANCE hInstClient, HANDLE *phEntry,
				IMemoryCacheClient *pClient) = 0;

	STDMETHOD(LookupEntry)(LPCSTR szKey, HANDLE * phEntry) = 0;
	STDMETHOD(GetData)(const HANDLE hEntry, void **ppvData, DWORD *pdwSize) const = 0;
	STDMETHOD(ReleaseEntry)(const HANDLE hEntry) = 0;
	STDMETHOD(RemoveEntry)(const HANDLE hEntry) = 0;
	STDMETHOD(RemoveEntryByKey)(LPCSTR szKey) = 0;

	STDMETHOD(Flush)() = 0;
};

struct ATL_NO_VTABLE __declspec(uuid("7634b28b-d819-409d-b96e-fc9f3aba329f")) 
	IMemoryCacheControl : public IUnknown
{
	// IMemoryCacheControl Methods
	STDMETHOD(SetMaxAllowedSize)(DWORD dwSize) = 0;
	STDMETHOD(GetMaxAllowedSize)(DWORD *pdwSize) = 0;
	STDMETHOD(SetMaxAllowedEntries)(DWORD dwSize) = 0;
	STDMETHOD(GetMaxAllowedEntries)(DWORD *pdwSize) = 0;
	STDMETHOD(ResetCache)() = 0;
};

struct ATL_NO_VTABLE __declspec(uuid("d4b6df2d-4bc0-4734-8ace-b73a0b975956")) 
	IMemoryCacheStats : public IUnknown
{
	// IMemoryCacheStats Methods
	STDMETHOD(ClearStats)() = 0;
	STDMETHOD(GetHitCount)(DWORD *pdwSize) = 0;
	STDMETHOD(GetMissCount)(DWORD *pdwSize) = 0;
	STDMETHOD(GetCurrentAllocSize)(DWORD *pdwSize) = 0;
	STDMETHOD(GetMaxAllocSize)(DWORD *pdwSize) = 0;
	STDMETHOD(GetCurrentEntryCount)(DWORD *pdwSize) = 0;
	STDMETHOD(GetMaxEntryCount)(DWORD *pdwSize) = 0;

};

struct DLL_CACHE_ENTRY
{
	HINSTANCE hInstDll;
	DWORD dwRefs;
	CHAR szDllName[_MAX_PATH];
};

inline bool operator==(const DLL_CACHE_ENTRY& entry1, const DLL_CACHE_ENTRY& entry2)
{
	return (entry1.hInstDll == entry2.hInstDll);
}

struct ATL_NO_VTABLE __declspec(uuid("A12478AB-D261-42f9-B525-7589143C1C97")) 
	IDllCache : public IUnknown
{
	// IDllCache methods
	virtual HINSTANCE Load(LPCSTR szFileName, void **pPeerInfo) = 0;
	virtual BOOL Free(HINSTANCE hInstance) = 0;
	virtual BOOL AddRefModule(HINSTANCE hInstance) = 0;
	virtual BOOL ReleaseModule(HINSTANCE hInstance) = 0;
	virtual HRESULT GetEntries(DWORD dwCount, DLL_CACHE_ENTRY *pEntries, DWORD *pdwCopied) = 0;
    virtual HRESULT Flush() = 0;
};

#ifndef ATL_CACHE_KEY_LENGTH
#define ATL_CACHE_KEY_LENGTH 128
#endif

typedef CFixedStringT<CStringA, ATL_CACHE_KEY_LENGTH> CFixedStringKey;

// No flusher -- only expired entries will be removed from the cache
// Also gives the skeleton for all of the flushers
class CNoFlusher
{
public:
    struct CCacheData
    {
    };
    
    void Add(CCacheData * /*pItem*/) { }
    void Remove(CCacheData * /*pItem*/) { }
    void Access(CCacheData * /*pItem*/) { }
    CCacheData * GetStart() const { return NULL; }
    CCacheData * GetNext(CCacheData * /*pCur*/) const { return NULL; }
};

// Old flusher -- oldest items are flushed first
class COldFlusher
{
public:
    struct CCacheData
    {
        CCacheData * pNext;
        CCacheData * pPrev;
    };

    CCacheData * pHead;
    CCacheData * pTail;

    COldFlusher() : pHead(NULL), pTail(NULL)
    {
    }

    // Add it to the tail of the list
    void Add(CCacheData * pItem)
    {
        ATLASSERT(pItem);

        pItem->pNext = NULL;
        pItem->pPrev = pTail;
        if (pHead)
        {
            pTail->pNext = pItem;
            pTail = pItem;
        }
        else
        {
            pHead = pItem;
            pTail = pItem;
        }
    }

    void Remove(CCacheData * pItem)
    {
        ATLASSERT(pItem);

        CCacheData * pPrev = pItem->pPrev;
        CCacheData * pNext = pItem->pNext;

        if (pPrev)
            pPrev->pNext = pNext;
        else
            pHead = pNext;

        if (pNext)
            pNext->pPrev = pPrev;
        else
            pTail = pPrev;

    }

    void Access(CCacheData * /*pItem*/)
    {
    }

    void Release(CCacheData * /*pItem*/)
    {
    }

    CCacheData * GetStart() const
    {
        return pHead;
    }

    CCacheData * GetNext(CCacheData * pCur) const
    {
        if (pCur != NULL)
            return pCur->pNext;
        else
            return NULL;
    }
};

// Least recently used flusher -- the item that was accessed the longest time ago is flushed
class CLRUFlusher : public COldFlusher
{
public:
    // Move it to the tail of the list
    void Access(CCacheData * pItem)
    {
        ATLASSERT(pItem);

        Remove(pItem);
        Add(pItem);
    }
};

// Least often used flusher
class CLOUFlusher : public COldFlusher
{
public:
    struct CCacheData : public COldFlusher::CCacheData
    {
        DWORD dwAccessed;
    };

    CCacheData * pHead;
    CCacheData * pTail;

    // Adds to the tail of the list
    void Add(CCacheData * pItem)
    {
        ATLASSERT(pItem);
        pItem->dwAccessed = 1;
        COldFlusher::Add(pItem);
    }

    void Access(CCacheData * pItem)
    {
        ATLASSERT(pItem);
        pItem->dwAccessed++;

        CCacheData * pMark = static_cast<CCacheData *>(pItem->pPrev);
        if (!pMark)   // The item is already at the head
            return;

        if (pMark->dwAccessed >= pItem->dwAccessed) // The element before it has
			return;									// been accessed more times

        Remove(pItem);

        while (pMark && (pItem->dwAccessed < pMark->dwAccessed))
            pMark = static_cast<CCacheData *>(pMark->pPrev);

		// pMark points to the first element that has been accessed more times,
		// so add pItem after pMark
        if (pMark)
        {
            CCacheData *pNext = static_cast<CCacheData *>(pMark->pNext);
			pMark->pNext = pItem;
			pItem->pPrev = pMark;

			pItem->pNext = pNext;
			pNext->pPrev = pItem;
        }
        else // Ran out of items -- put it on the head
        {
            pItem->pNext = pHead;
            pItem->pPrev = NULL;
            if (pHead)
                pHead->pPrev = pItem;
            else // the list was empty
                pTail = pItem;
            pHead = pItem;
        }
    }

	// We start at the tail and move forward for this flusher
    CCacheData * GetStart() const
    {
        return pTail;
    }

    CCacheData * GetNext(CCacheData * pCur) const
    {
        if (pCur != NULL)
            return static_cast<CCacheData *>(pCur->pPrev);
        else
            return NULL;
    }

};

template <class CFirst, class CSecond>
class COrFlushers : public CFirst, public CSecond
{
    struct CCacheData : public CFirst::CCacheData, CSecond::CCacheData
    {
    };

    BOOL m_bWhich;
	COrFlushers() : CFirst(), CSecond()
	{
		m_bWhich = FALSE;
	}

    BOOL Switch()
    {
        m_bWhich = !m_bWhich;
        return m_bWhich;
    }

    void Add(CCacheData * pItem) 
    {
        ATLASSERT(pItem);
        CFirst::Add(pItem);
        CSecond::Add(pItem);
    }

    void Remove(CCacheData * pItem) 
    {
        ATLASSERT(pItem);
        CFirst::Remove(pItem);
        CSecond::Remove(pItem);
    }

    void Access(CCacheData * pItem) 
    {
        ATLASSERT(pItem);
        CFirst::Access(pItem);
        CSecond::Access(pItem);
    }
    void Release(CCacheData * pItem)
    {
        ATLASSERT(pItem);
        CFirst::Release(pItem);
        CSecond::Release(pItem);
    }

    CCacheData * GetStart() const 
    { 
        if (bWhich)
            return static_cast<CCacheData *>(CFirst::GetStart());
        else
            return static_cast<CCacheData *>(CSecond::GetStart());
    }

    CCacheData * GetNext(CCacheData * pCur) const 
    { 
        if (bWhich)
            return static_cast<CCacheData *>(CFirst::GetNext(pCur));
        else
            return static_cast<CCacheData *>(CSecond::GetNext(pCur));
    }
};

class CNoExpire
{
public:
    struct CCacheData
    {
    };

    void Add(CCacheData * /*pItem*/) { }
    void Commit(CCacheData * /*pItem*/) { }
    void Access(CCacheData * /*pItem*/) { }
    void Remove(CCacheData * /*pItem*/) { }
    void Start() { }
	BOOL IsExpired(CCacheData * /*pItem*/) { return FALSE; }
    CCacheData * GetExpired() { return NULL; }
};

class CExpireCuller
{
public:
    struct CCacheData
    {
        CFileTime cftExpireTime;
        CCacheData * pNext;
        CCacheData * pPrev;
    };

    CFileTime m_cftCurrent;
    CCacheData *pHead;
    CCacheData *pTail;

    CExpireCuller()
    {
        pHead = NULL;
        pTail = NULL;
    }

    // Element is being added -- perform necessary initialization
    void Add(CCacheData * pItem)
    {
		pItem;
        ATLASSERT(pItem);
    }

    // Expiration data has been set -- add to main list
    // Head is the first item to expire
	// a FILETIME of 0 indicates that the item should never expire
    void Commit(CCacheData * pItem)
    {
        ATLASSERT(pItem);
        if (!pHead)
        {
            pHead = pItem;
            pTail = pItem;
            pItem->pNext = NULL;
            pItem->pPrev = NULL;
            return;
        }

		if (CFileTime(pItem->cftExpireTime) == 0)
		{
			pTail->pNext = pItem;
			pItem->pPrev = pTail;
			pItem->pNext = NULL;
			pTail = pItem;
			return;
		}

        CCacheData * pMark = pHead;
        while (pMark && (pMark->cftExpireTime < pItem->cftExpireTime))
            pMark = pMark->pNext;

        if (pMark) // An entry was found that expires after the added entry
        {
            CCacheData *pPrev = pMark->pPrev;
            if (pPrev)
                pPrev->pNext = pItem;
            else
                pHead = pItem;

            pItem->pNext = pMark;
            pItem->pPrev = pPrev;
            pMark->pPrev = pItem;
        }
        else // Ran out of items -- put it on the tail
        {
            if (pTail)
                pTail->pNext = pItem;
            pItem->pPrev = pTail;
            pItem->pNext = NULL;
            pTail = pItem;
        }
    }

    void Access(CCacheData * /*pItem*/)
    {
    }

    void Release(CCacheData * /*pItem*/)
    {
    }

    void Remove(CCacheData * pItem)
    {
        ATLASSERT(pItem);
        CCacheData *pPrev = pItem->pPrev;
        CCacheData *pNext = pItem->pNext;
        
        if (pPrev)
            pPrev->pNext = pNext;
        else
            pHead = pNext;

        if (pNext)
            pNext->pPrev = pPrev;
        else
            pTail = pPrev;

    }

    // About to start culling
    void Start()
    {
        m_cftCurrent = CFileTime::GetCurrentTime();
    }

	BOOL IsExpired(CCacheData *pItem)
	{
        if ((pItem->cftExpireTime != 0) && 
            m_cftCurrent > pItem->cftExpireTime)
            return TRUE;

		return FALSE;
	}

    // Get the next expired entry
    CCacheData * GetExpired()
    {
        if (!pHead)
            return NULL;
        if (IsExpired(pHead))
            return pHead;

        return NULL;
    }
};

class CLifetimeCuller : public CExpireCuller
{
public:
    struct CCacheData : public CExpireCuller::CCacheData
    {
        ULONGLONG nLifespan;
    };

    void Add(CCacheData * pItem)
    {
        ATLASSERT(pItem);
        pItem->nLifespan = 0;
        CExpireCuller::Add(pItem);
    }

    void Commit(CCacheData * pItem)
    {        
        ATLASSERT(pItem);
		if (pItem->nLifespan == 0)
			pItem->cftExpireTime = 0;
		else
			pItem->cftExpireTime = CFileTime(CFileTime::GetCurrentTime().GetTime() + pItem->nLifespan);
        CExpireCuller::Commit(pItem);
    }

    void Access(CCacheData * pItem)
    {
        ATLASSERT(pItem);
        CExpireCuller::Remove(pItem);
        Commit(pItem);
    }

    CCacheData * GetExpired() 
    { 
        return static_cast<CCacheData *>(CExpireCuller::GetExpired());
    }
};

template <__int64 ftLifespan>
class CFixedLifetimeCuller : public CExpireCuller
{
public:
    void Commit(CCacheData * pItem)
    {
        ATLASSERT(pItem);

		if (ftLifespan == 0)
			pItem->cftExpireTime = 0;
		else
			pItem->cftExpireTime = CFileTime::GetCurrentTime() + CFileTimeSpan(ftLifespan);

        CExpireCuller::Commit(pItem);
    }

    void Access(CCacheData * pItem)
    {
        ATLASSERT(pItem);
        CExpireCuller::Remove(pItem);
        Commit(pItem);
    }

    CCacheData * GetExpired() 
    { 
        return static_cast<CCacheData *>(CExpireCuller::GetExpired());
    }
};


template <class CFirst, class CSecond>
class OrCullers : public CFirst, public CSecond
{
    struct CCacheData : public CFirst::CCacheData, public CSecond::CCacheData
    {
    };

    void Add(CCacheData * pItem)
    {
        CFirst::Add(pItem);
        CSecond::Add(pItem);
    }

    void Access(CCacheData * pItem)
    {
        CFirst::Access(pItem);
        CSecond::Access(pItem);
    }

    void Remove(CCacheData * pItem)
    {
        CFirst::Remove(pItem);
        CSecond::Remove(pItem);
    }

    void Start(CCacheData * pItem)
    {
        CFirst::Start(pItem);
        CSecond::Start(pItem);
    }

	CCacheData * GetExpired() 
    { 
		CCacheData *pItem = static_cast<CCacheData *>(CFirst::GetExpired());
		if (!pItem)
			pItem = static_cast<CCacheData *>(CSecond::GetExpired());

		return pItem;
    }

    BOOL IsExpired(CCacheData * pItem)
    {
        return (CFirst::IsExpired(pItem) || CSecond::IsExpired(pItem))
    }
};

//
//CMemoryCacheBase
// Description:
//	This class provides the implementation of a generic cache that stores
//	elements in memory. CMemoryCacheBase uses the CCacheDataBase generic
//	cache element structure to hold items in the cache. The cache is
//	implemented using the CAtlMap map class. CMemoryCache uses a wide
//	character string as it's Key type to identify entries. Entries must
//	have unique key values. If you try to add an entry with a key that
//	is exactly the same as an existing key, the existing entry will be
//	overwritten.
//
// Template Parameters:
//  T: The class that inherits from this class. This class must implement
//     void OnDestroyEntry(NodeType *pEntry);
//	DataType: Specifies the type of the element to be stored in the memory
//			  cache such as CString or void*
//  NodeInfo: Specifies any additional data that should be stored in each item
//            in the cache
//  keyType, keyTrait : specifies the key type and traits (see CAtlMap)
//  Flusher : the class responsible for determining which data should be flushed
//            when the cache is at a configuration limit
//  Culler  : the class responsible for determining which data should be removed
//            from the cache due to expiration
//	SyncClass:Specifies the class that will be used for thread synchronization
//			  when accessing the cache. The class interface for SyncClass must
//			  be identical to that of CComCriticalSection (see atlbase.h)
//  StatClass: Class used to contain statistics about this cache.
// REVIEW: modify interface to Flusher system
template <class T,
         class DataType,
         class NodeInfo=CCacheDataBase<DataType>,
		 class keyType=CFixedStringKey,
		 class KeyTrait=CStringElementTraits<CFixedStringKey >,
         class Flusher=COldFlusher,
         class Culler=CExpireCuller,
		 class SyncClass=CComCriticalSection,
		 class StatClass=CStdStatClass >
		 class CMemoryCacheBase
{
protected:
	typedef keyType keytype;
    struct NodeType : public NodeInfo, public Flusher::CCacheData, public Culler::CCacheData
    {
	    DataType Data;
        POSITION pos;
	    DWORD dwSize;	
        DWORD dwRef;
    };

	typedef CAtlMap<keyType, NodeType *, KeyTrait> mapType;
	SyncClass m_syncObj;
	StatClass m_statObj;
    Flusher m_flusher;
    Culler m_culler;

	//memory cache configuration parameters
	DWORD m_dwMaxAllocationSize;
	DWORD m_dwMaxEntries;

    BOOL m_bInitialized;
public:

	mapType m_hashTable;
	CMemoryCacheBase() throw() :
	  m_dwMaxAllocationSize(0xFFFFFFFF),
	  m_dwMaxEntries(0xFFFFFFFF),
      m_bInitialized(FALSE)
	{

	}

	//Initializes the cache and the cache synchronization object
	//Also the performance monitoring
	HRESULT Initialize() throw()
	{
        if (m_bInitialized)
            return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);
        HRESULT hr;
		hr =  m_syncObj.Init();

		if (hr == S_OK)
			hr = m_statObj.Initialize();

        m_bInitialized = TRUE;

		return hr;
	}

	//removes all entries whether or not they are initialized.
	HRESULT Uninitialize() throw()
	{
        if (!m_bInitialized)
            return S_OK;

		//clear out the hash table
		m_syncObj.Lock();

		RemoveAllEntries();
		m_statObj.Uninitialize();

		m_syncObj.Unlock();
		m_syncObj.Term();

        m_bInitialized = FALSE;

		return S_OK;
	}
	
	//Adds an entry to the cache.
	//Also, adds an initial reference on the entry if phEntry is not NULL
	HRESULT AddEntry(
					const keyType &Key,				//key for entry
					const DataType &data,				//See the DataType template parameter
					DWORD dwSize,						//Size of memory to be stored in the cache 
 					HANDLE *phEntry = NULL				//out pointer that will contain a handle to the new
														//cache entry on success.
					) throw()
	{
        ATLASSERT(m_bInitialized);

		NodeType * pEntry;

		pEntry = (NodeType *)malloc(sizeof(NodeType));
		if (!pEntry)
			return E_OUTOFMEMORY;

		memset(pEntry, 0, sizeof(NodeType));

		//fill entry
		if (phEntry)
		{
			*phEntry = (HANDLE)pEntry;
			pEntry->dwRef++;
		}
		pEntry->Data = data;
		pEntry->dwSize = dwSize;

		m_syncObj.Lock();

		POSITION pos = (POSITION)m_hashTable.Lookup(Key);

		if (pos != NULL)
        {
            RemoveAt(pos, FALSE);
            m_hashTable.GetValueAt(pos) = pEntry;
        }
        else
            pos = m_hashTable.SetAt(Key, pEntry);

        pEntry->pos = pos;
		m_statObj.AddElement(dwSize);
        m_flusher.Add(pEntry);
        m_culler.Add(pEntry);

		m_syncObj.Unlock();

        if (!phEntry)
            return CommitEntry((HANDLE)pEntry);

		return S_OK;
	}

        // Commits the entry to the cache
    HRESULT CommitEntry(const HANDLE hEntry)
    {
        ATLASSERT(m_bInitialized);
        if (!hEntry || hEntry == INVALID_HANDLE_VALUE)
            return E_INVALIDARG;

        m_syncObj.Lock();
        NodeType *pEntry = static_cast<NodeType *>(hEntry);
        m_culler.Commit(pEntry);
        m_syncObj.Unlock();
        return S_OK;
    }

	// Looks up an entry and returns a handle to it,
	// also updates access count and reference count
	HRESULT LookupEntry(const keyType &Key, HANDLE * phEntry) throw()
	{
        ATLASSERT(m_bInitialized);
		HRESULT hr = E_FAIL;

		m_syncObj.Lock();
		POSITION pos = (POSITION)m_hashTable.Lookup(Key);
		if (pos != NULL)
		{
			NodeType * pEntry = m_hashTable.GetValueAt(pos);
            m_flusher.Access(pEntry);
            m_culler.Access(pEntry);
            if (phEntry)
            {
				pEntry->dwRef++;
				*phEntry = (HANDLE)pEntry;
            }

			m_statObj.Hit();

			hr = S_OK;
		}
		else
		{
			*phEntry = NULL;
			m_statObj.Miss();
		}
		m_syncObj.Unlock();
	
		return hr;
	}

	// Gets the data based on the handle.  Is thread-safe as long as there is a
	// reference on the data
	HRESULT GetEntryData(const HANDLE hEntry, DataType *pData, DWORD *pdwSize) const throw()
	{
        ATLASSERT(m_bInitialized);
		ATLASSERT(pData != NULL || pdwSize != NULL);  // At least one should not be NULL

        if (!hEntry || hEntry == INVALID_HANDLE_VALUE)
            return E_INVALIDARG;

		NodeType * pEntry = (NodeType *)hEntry;
		if (pData) 
			*pData = pEntry->Data;
		if (pdwSize) 
			*pdwSize = pEntry->dwSize;
	
		return S_OK;
	}

	// Unreferences the entry based on the handle
	DWORD ReleaseEntry(const HANDLE hEntry) throw()
	{
        ATLASSERT(m_bInitialized);
        if (!hEntry || hEntry == INVALID_HANDLE_VALUE)
            return (DWORD)-1;

        m_syncObj.Lock();
		NodeType * pEntry = (NodeType *)hEntry;
        m_flusher.Release(pEntry);
        m_culler.Release(pEntry);
		ATLASSERT(pEntry->dwRef > 0);

		DWORD dwRef = --pEntry->dwRef;
		if ((pEntry->pos == NULL) && (pEntry->dwRef == 0))
				InternalRemoveEntry(pEntry);

		m_syncObj.Unlock();

		return dwRef;
	}

	// Increments the entry's reference count
	DWORD AddRefEntry(const HANDLE hEntry) throw()
	{
        ATLASSERT(m_bInitialized);
        if (!hEntry || hEntry == INVALID_HANDLE_VALUE)
            return (DWORD)-1;

		m_syncObj.Lock();
		NodeType * pEntry = (NodeType *)hEntry;
        m_flusher.Access(pEntry);
        m_culler.Access(pEntry);
		DWORD dwRef = ++pEntry->dwRef;
		m_syncObj.Unlock();

		return dwRef;
	}

	// Removes an entry from the cache regardless of whether or 
	// not it has expired.  If there are references, it detaches
	// the entry so that future lookups will fail, and when
	// the ref count drops to zero, it will be deleted
	HRESULT RemoveEntryByKey(const keyType &Key) throw()
	{
        ATLASSERT(m_bInitialized);
        HANDLE hEntry;
        HRESULT hr = LookupEntry(Key, &hEntry);
        if (hr == S_OK)
            hr = RemoveEntry(hEntry);

        return hr;
	}

    // Removes the element from the cache.  If there are still
    // references, then the entry is detached.
	HRESULT RemoveEntry(const HANDLE hEntry) throw()
	{
        ATLASSERT(m_bInitialized);
        if (!hEntry || hEntry == INVALID_HANDLE_VALUE)
            return E_INVALIDARG;

		m_syncObj.Lock();
		NodeType * pEntry = (NodeType *)hEntry;
        m_flusher.Release(pEntry);
        m_culler.Release(pEntry);
		ATLASSERT(pEntry->dwRef > 0);
		pEntry->dwRef--;
        RemoveAt(pEntry->pos, TRUE);
        m_syncObj.Unlock();

        return S_OK;
    }

    // CullEntries removes all expired items
    HRESULT CullEntries() throw()
    {
        ATLASSERT(m_bInitialized);
		m_syncObj.Lock();

        m_culler.Start();

        while (NodeType *pNode = static_cast<NodeType *>(m_culler.GetExpired()))
            RemoveAt(pNode->pos, TRUE);

		m_syncObj.Unlock();

        return S_OK;
    }

    // FlushEntries reduces the cache to meet the configuration requirements
	HRESULT FlushEntries() throw()
	{        
        ATLASSERT(m_bInitialized);
        CullEntries(); 

		m_syncObj.Lock();

        NodeType * pNode = static_cast<NodeType *>(m_flusher.GetStart());

		while (pNode &&
               (((m_statObj.GetCurrentEntryCount() > m_dwMaxEntries)) ||
			    ((m_statObj.GetCurrentAllocSize() > m_dwMaxAllocationSize))))
		{
            NodeType *pNext = static_cast<NodeType *>(m_flusher.GetNext(pNode));

            if (pNode->dwRef == 0)
                RemoveAt(pNode->pos, TRUE);

            pNode = pNext;
		}
		m_syncObj.Unlock();

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SetMaxAllowedSize(DWORD dwSize) throw()
	{
		m_dwMaxAllocationSize = dwSize;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetMaxAllowedSize(DWORD *pdwSize) throw()
	{
		if (!pdwSize)
			return E_POINTER;
		*pdwSize = m_dwMaxAllocationSize;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SetMaxAllowedEntries(DWORD dwSize) throw()
	{
		m_dwMaxEntries = dwSize;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetMaxAllowedEntries(DWORD *pdwSize) throw()
	{
		if (!pdwSize)
			return E_POINTER;
		*pdwSize = m_dwMaxEntries;
		return S_OK;
	}


	HRESULT ResetCache() throw()
	{
        ATLASSERT(m_bInitialized);
		m_syncObj.Lock();
		ClearStats();
		m_hashTable.RemoveAll();
		m_syncObj.Unlock();
		return S_OK;
	}

    HRESULT ClearStats() throw()
    {
        m_statObj.ResetCounters();
        return S_OK;
    }

    HRESULT RemoveAllEntries() throw()
    {
        ATLASSERT(m_bInitialized);
        m_syncObj.Lock();
        m_hashTable.DisableAutoRehash();
    	POSITION pos = m_hashTable.GetStartPosition();
		POSITION oldpos;
		while (pos != NULL)
		{
			oldpos = pos;
			m_hashTable.GetNext(pos);
            RemoveAt(oldpos, TRUE);
		}
        m_hashTable.EnableAutoRehash();
        m_syncObj.Unlock();

        return S_OK;
    }

protected:

    // Checks to see if the cache can accommodate any new entries within
    // its allocation and entry count limits.  
	bool CanAddEntry(DWORD dwSizeToAdd) throw()
	{
		return CheckAlloc(dwSizeToAdd) && CheckEntryCount(1);
	}

    // Checks to see if the cache can accommodate dwSizeToAdd additional
    // allocation within its allocation limit.
	bool CheckAlloc(DWORD dwSizeToAdd) throw()
	{
		if (m_dwMaxAllocationSize == 0xFFFFFFFF)
			return true; //max allocation size setting hasn't been set
		DWORD dwNew = m_statObj.GetCurrentAllocSize() + dwSizeToAdd;
		return dwNew < m_dwMaxAllocationSize;
	}


    // Checks to see if the cache can accommodate dwNumEntriesToAdd
    // additional entries within its limits.
	bool CheckEntryCount(DWORD dwNumEntriesToAdd) throw()
	{
		if (m_dwMaxEntries == 0xFFFFFFFF)
			return true; //max entry size hasn't been set
		DWORD dwNew = m_statObj.GetCurrentEntryCount() + dwNumEntriesToAdd;
		return dwNew < m_dwMaxEntries;

	}

protected:
	// Takes the element at pos in the hash table and removes it from
    // the cache.  If there are no references, then the entry is
    // deleted, otherwise it is deleted by ReleaseEntry when the
    // refcount goes to zero.
	HRESULT RemoveAt(POSITION pos, BOOL bDelete) throw()
	{
		HRESULT hr = S_OK;
		ATLASSERT(pos != NULL);
		NodeType * pEntry = m_hashTable.GetValueAt(pos);
        m_flusher.Remove(pEntry);
        m_culler.Remove(pEntry);
        if (bDelete)
            m_hashTable.RemoveAtPos(pos);

		if ((long)pEntry->dwRef == 0)
			hr = InternalRemoveEntry(pEntry);
		else
			pEntry->pos = NULL;

		return S_OK;
	}
	
    // Does the actual destruction of the node.  Deletes the
    // NodeType struct and calls the inherited class's 
    // OnDestroyEntry function, where other necessary destruction
    // can take place.  Also updates the cache statistics.
    // Inherited classes should call RemoveAt unless the element's
    // refcount is zero and it has been removed from the
    // culler and flusher lists.
	HRESULT InternalRemoveEntry(NodeType * pEntry) throw()
	{
		ATLASSERT(pEntry != NULL);

		T* pT = static_cast<T*>(this);

		ATLASSERT((long)pEntry->dwRef == 0);

		pT->OnDestroyEntry(pEntry);
		
		m_statObj.ReleaseElement(pEntry->dwSize);

		free(pEntry);

		return S_OK;
	}
}; // CMemoryCacheBase

class CCacheDataBase
{
};

struct CCacheDataEx : public CCacheDataBase
{
    HINSTANCE hInstance;
    IMemoryCacheClient * pClient;
};


template <typename DataType, 
        class StatClass=CStdStatClass,
        class FlushClass=COldFlusher,
		class keyType=CFixedStringKey,  class KeyTrait=CStringElementTraits<CFixedStringKey >,
		class SyncClass=CComCriticalSection >
class CMemoryCache:
	public CMemoryCacheBase<CMemoryCache, DataType, CCacheDataEx, 
        keyType, KeyTrait, FlushClass, CExpireCuller, SyncClass, StatClass>
{
protected:
	CComPtr<IServiceProvider> m_spServiceProv;
	CComPtr<IDllCache> m_spDllCache;
    typedef CMemoryCacheBase<CMemoryCache, DataType, CCacheDataEx, 
        keyType, KeyTrait, FlushClass, CExpireCuller, SyncClass, StatClass> baseClass;
public:

    HRESULT Initialize(IServiceProvider * pProvider)
    {
        baseClass::Initialize();
        m_spServiceProv = pProvider;
        if (pProvider)
            return m_spServiceProv->QueryService(__uuidof(IDllCache), __uuidof(IDllCache), (void**)&m_spDllCache);
        else
            return S_OK;
    }

    HRESULT AddEntry(
					const keyType &Key,
					const DataType &data,
					DWORD dwSize,
                    FILETIME * pftExpireTime = NULL,
                    HINSTANCE hInstance = NULL,
                    IMemoryCacheClient * pClient = NULL,
 					HANDLE *phEntry = NULL
					) throw()
    {
        HRESULT hr;
        NodeType * pEntry = NULL;
        hr = baseClass::AddEntry(Key, data, dwSize, (HANDLE *)&pEntry);
        if (hr != S_OK)
            return hr;

        pEntry->hInstance = hInstance;
        pEntry->pClient = pClient;
        if (pftExpireTime)
            pEntry->cftExpireTime = *pftExpireTime;

		if (hInstance && m_spDllCache)
			m_spDllCache->AddRefModule(hInstance);

        baseClass::CommitEntry((HANDLE)pEntry);

        if (phEntry)
            *phEntry = (HANDLE)pEntry;
        else
            baseClass::ReleaseEntry((HANDLE)pEntry);

        return S_OK;
    }

   	virtual void OnDestroyEntry(const NodeType * pEntry) throw()
	{
		ATLASSERT(pEntry);
		if (!pEntry)
			return;

		if (pEntry->pClient)
			pEntry->pClient->Free((void *)&pEntry->Data);
		if (pEntry->hInstance && m_spDllCache)
			m_spDllCache->ReleaseModule(pEntry->hInstance);
	}
}; // CMemoryCache

#define ATL_CACHE_STAT_NUMCOUNTERS 6

//
//CStdStatClass
// Description
// This class provides the implementation of a standard cache statistics accounting class
class CStdStatClass
{ 
protected:

	DWORD * m_rgCounters;
	enum {
			hitcount=0, misscount, currentallocations, maxallocations,
			currententries, maxentries
	};
public:

	CStdStatClass() throw()
	{
	}

	HRESULT Initialize() throw()
	{
		m_rgCounters = NULL;
		ATLTRY(m_rgCounters = new DWORD[ATL_CACHE_STAT_NUMCOUNTERS]);
		if (m_rgCounters == NULL)
			return E_OUTOFMEMORY;

		ResetCounters();
		return S_OK;
	}

	HRESULT Uninitialize() throw()
	{
		if (m_rgCounters)
			delete [] m_rgCounters;
		return S_OK;
	}

	void Hit() throw()
	{
		ATLASSERT(m_rgCounters != NULL);

		m_rgCounters[hitcount]++; 
	}

	void Miss() throw()
	{ 
		ATLASSERT(m_rgCounters != NULL);

		m_rgCounters[misscount]++;
	}

	void AddElement(DWORD dwBytes) throw()
	{
		ATLASSERT(m_rgCounters != NULL);

		m_rgCounters[currententries]++;
		m_rgCounters[currentallocations] += dwBytes;

		if (m_rgCounters[currententries] > m_rgCounters[maxentries])
			m_rgCounters[maxentries] = m_rgCounters[currententries];

		if (m_rgCounters[currentallocations] > m_rgCounters[maxallocations])
			m_rgCounters[maxallocations]= m_rgCounters[currentallocations];
	}

	void ReleaseElement(DWORD dwBytes) throw()
	{
		ATLASSERT(m_rgCounters != NULL);

		m_rgCounters[currententries]--;
		m_rgCounters[currentallocations] -= dwBytes;
	}

	DWORD GetHitCount() throw()
	{
		ATLASSERT(m_rgCounters != NULL);

		return m_rgCounters[hitcount];
	}

	DWORD GetMissCount() throw()
	{
		ATLASSERT(m_rgCounters != NULL);

		return m_rgCounters[misscount];
	}

	DWORD GetCurrentAllocSize() throw()
	{
		ATLASSERT(m_rgCounters != NULL);

		return m_rgCounters[currentallocations];
	}

	DWORD GetMaxAllocSize() throw()
	{
		ATLASSERT(m_rgCounters != NULL);

		return m_rgCounters[maxallocations];
	}

	DWORD GetCurrentEntryCount() throw()
	{
		ATLASSERT(m_rgCounters != NULL);

		return m_rgCounters[currententries];
	}

	DWORD GetMaxEntryCount() throw()
	{	
		ATLASSERT(m_rgCounters != NULL);

		return m_rgCounters[maxentries];
	}

	void ResetCounters() throw()
	{
		ATLASSERT(m_rgCounters != NULL);

		memset(m_rgCounters, 0, ATL_CACHE_STAT_NUMCOUNTERS * sizeof(DWORD));
	}
}; // CStdStatClass

// CStdStatData - contains the data that CStdStatClass keeps track of
struct CPerfStatObject : public CPerfObject
{
	DWORD m_rgCounters[ATL_CACHE_STAT_NUMCOUNTERS];
};

#define ATL_PERF_CACHE_OBJECT 100
#define ATL_PERF_CACHE_HITCOUNT 101
#define ATL_PERF_CACHE_MISSCOUNT 102
#define ATL_PERF_CACHE_CURRENTALLOCATIONS 103
#define ATL_PERF_CACHE_MAXALLOCATIONS 104
#define ATL_PERF_CACHE_CURRENTENTRIES 105
#define ATL_PERF_CACHE_MAXENTRIES 106

// CCachePerfMon - the interface to CPerfMon, with associated definitions
class CCachePerfMon : public CPerfMon
{
public:
	BEGIN_PERF_MAP(_T("ATL Server:Cache"))
		DEFINE_PERF_OBJECT_RES(ATL_PERF_CACHE_OBJECT, IDS_PERFMON_CACHE, IDS_PERFMON_CACHE_HELP, sizeof(CPerfStatObject))
		DEFINE_PERF_COUNTER_RES(ATL_PERF_CACHE_HITCOUNT, IDS_PERFMON_HITCOUNT, IDS_PERFMON_HITCOUNT_HELP, PERF_COUNTER_RAWCOUNT, offsetof(CPerfStatObject, m_rgCounters[0]), -1)
		DEFINE_PERF_COUNTER_RES(ATL_PERF_CACHE_MISSCOUNT, IDS_PERFMON_MISSCOUNT, IDS_PERFMON_MISSCOUNT_HELP, PERF_COUNTER_RAWCOUNT, offsetof(CPerfStatObject, m_rgCounters[1]), -1)
		DEFINE_PERF_COUNTER_RES(ATL_PERF_CACHE_CURRENTALLOCATIONS, IDS_PERFMON_CURRENTALLOCATIONS, IDS_PERFMON_CURRENTALLOCATIONS_HELP, PERF_COUNTER_RAWCOUNT, offsetof(CPerfStatObject, m_rgCounters[2]), -3)
		DEFINE_PERF_COUNTER_RES(ATL_PERF_CACHE_MAXALLOCATIONS, IDS_PERFMON_MAXALLOCATIONS, IDS_PERFMON_MAXALLOCATIONS_HELP, PERF_COUNTER_RAWCOUNT, offsetof(CPerfStatObject, m_rgCounters[3]), -3)
		DEFINE_PERF_COUNTER_RES(ATL_PERF_CACHE_CURRENTENTRIES, IDS_PERFMON_CURRENTENTRIES, IDS_PERFMON_CURRENTENTRIES_HELP, PERF_COUNTER_RAWCOUNT, offsetof(CPerfStatObject, m_rgCounters[4]), -1)
		DEFINE_PERF_COUNTER_RES(ATL_PERF_CACHE_MAXENTRIES, IDS_PERFMON_MAXENTRIES, IDS_PERFMON_MAXENTRIES_HELP, PERF_COUNTER_RAWCOUNT, offsetof(CPerfStatObject, m_rgCounters[5]), -1)
	END_PERF_MAP()
};


//
//CPerfStatClass
// Description
// This class provides the implementation of a cache statistics gathering class
// with PerfMon support
class CPerfStatClass : public CStdStatClass
{
	CPerfStatObject * m_pPerfObject;
	CCachePerfMon m_PerfMon;

public:

	HRESULT Initialize(LPWSTR szName=NULL) throw()
	{
		HRESULT hr;

		if (!szName)
			szName = L"Object 1";

		m_pPerfObject = NULL;
		ATLTRACE(atlTraceCache, 2, _T("Initializing m_PerfMon\n"));
		hr = m_PerfMon.Initialize();
		if (SUCCEEDED(hr))
		{
			CPerfLock lock(&m_PerfMon);
			if (FAILED(hr = lock.GetStatus()))
			{
				return hr;
			}

			hr = m_PerfMon.CreateInstance(ATL_PERF_CACHE_OBJECT, 0, szName, reinterpret_cast<CPerfObject**>(&m_pPerfObject));
			if (FAILED(hr))
			{
				return hr;
			}
			m_rgCounters = m_pPerfObject->m_rgCounters;
			
			ResetCounters();
		}
		else
			ATLASSERT(m_pPerfObject == NULL);

		return hr;
	}

	HRESULT Uninitialize() throw()
	{
		if (m_pPerfObject != NULL) // Initialized m_pPerfObject successfully above
		{
			HRESULT hr = m_PerfMon.ReleaseInstance(m_pPerfObject);
			if (hr != S_OK)
				return hr;

			return m_PerfMon.UnInitialize();
		}
		else // m_PerfMon did not initialize
			delete [] m_rgCounters;

		return S_OK;
	}
}; // CPerfStatClass

#ifndef ATL_BLOB_CACHE_TIMEOUT
#define ATL_BLOB_CACHE_TIMEOUT 1000
#endif

//
//CBlobCache
// Description:
// Implements a cache that stores pointers to void. Uses the generic CMemoryCacheBase class
// as the implementation.
template<class MonitorClass, class StatClass = CStdStatClass, class SyncObj = CComCriticalSection>
class CBlobCache : public CMemoryCache<void*, StatClass, COldFlusher, CFixedStringKey, 
	CStringElementTraits<CFixedStringKey >, SyncObj>,
	public IMemoryCache,
	public IMemoryCacheControl,
    public IMemoryCacheStats,
	public IWorkerThreadClient
{
	typedef CMemoryCache<void*, StatClass, COldFlusher, CFixedStringKey, 
		CStringElementTraits<CFixedStringKey>, SyncObj> cacheBase;

	MonitorClass m_Monitor;

protected:
	HANDLE m_hTimer;

public:
	CBlobCache() : m_hTimer(NULL)
	{
	}

	HRESULT Initialize(IServiceProvider *pProv) throw()
	{
		HRESULT hr = cacheBase::Initialize(pProv);
		if (FAILED(hr))
			return hr;
		hr = m_Monitor.Initialize();
		if (FAILED(hr))
			return hr;
		return m_Monitor.AddTimer(ATL_BLOB_CACHE_TIMEOUT, 
			static_cast<IWorkerThreadClient*>(this), (DWORD_PTR) this, &m_hTimer);
	}

	template <class ThreadTraits>
	HRESULT Initialize(IServiceProvider *pProv, CWorkerThread<ThreadTraits> *pWorkerThread) throw()
	{
		ATLASSERT(pWorkerThread);

		HRESULT hr = S_OK;

		if (S_OK == cacheBase::Initialize(pProv))
		{
			hr = m_Monitor.InitializeWorker(pWorkerThread);
			if (FAILED(hr))
				return hr;
			return m_Monitor.AddTimer(ATL_BLOB_CACHE_TIMEOUT, 
				static_cast<IWorkerThreadClient*>(this), (DWORD_PTR) this, &m_hTimer);
		}
		return S_OK;
	}

	HRESULT Execute(DWORD_PTR dwParam, HANDLE /*hObject*/) throw()
	{
		CBlobCache* pCache = (CBlobCache*)dwParam;
	
		if (pCache)
			pCache->Flush();
		return S_OK;
	}

	HRESULT CloseHandle(HANDLE hObject) throw()
	{
		ATLASSERT(m_hTimer == hObject);
		m_hTimer = NULL;
		::CloseHandle(hObject);
		return S_OK;
	}

	~CBlobCache() throw()
	{
		if (m_hTimer)
			m_Monitor.RemoveHandle(m_hTimer);
	}

	HRESULT Uninitialize() throw()
	{
		if (m_hTimer)
		{
			m_Monitor.RemoveHandle(m_hTimer);
			m_hTimer = NULL;
		}
		m_Monitor.Shutdown();
		return cacheBase::Uninitialize();
	}
	// IUnknown methods
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppv) throw()
	{
		HRESULT hr = E_NOINTERFACE;
		if (!ppv)
			hr = E_POINTER;
		else
		{
			if (InlineIsEqualGUID(riid, __uuidof(IUnknown)) ||
				InlineIsEqualGUID(riid, __uuidof(IMemoryCache)))
			{
				*ppv = (IUnknown *) (IMemoryCache *) this;
				AddRef();
				hr = S_OK;
			}
			if (InlineIsEqualGUID(riid, __uuidof(IMemoryCacheStats)))
			{
				*ppv = (IUnknown *) (IMemoryCacheStats*)this;
				AddRef();
				hr = S_OK;
			}
			if (InlineIsEqualGUID(riid, __uuidof(IMemoryCacheControl)))
			{
				*ppv = (IUnknown *) (IMemoryCacheControl*)this;
				AddRef();
				hr = S_OK;
			}

		}
		return hr;
	}
	
	ULONG STDMETHODCALLTYPE AddRef() throw()
	{
		return 1;
	}
	
	ULONG STDMETHODCALLTYPE Release() throw()
	{
		return 1;
	}
	
	// IMemoryCache Methods
	HRESULT STDMETHODCALLTYPE Add(LPCSTR szKey, void *pvData, DWORD dwSize, 
		FILETIME *pftExpireTime, 
		HINSTANCE hInstClient,
		HANDLE *phEntry,
		IMemoryCacheClient *pClient) throw()
	{
		HRESULT hr = E_FAIL;
		//if it's a multithreaded cache monitor we'll let the monitor take care of
		//cleaning up the cache so we don't overflow our configuration settings.
		//if it's not a threaded cache monitor, we need to make sure we don't
		//overflow the configuration settings by adding a new element
		if (m_Monitor.GetThreadHandle()==NULL)
		{
			if (!cacheBase::CanAddEntry(dwSize))
			{
				//flush the entries and check again to see if we can add
				cacheBase::FlushEntries();
				if (!cacheBase::CanAddEntry(dwSize))
					return E_OUTOFMEMORY;
			}
		}
		hr = cacheBase::AddEntry(szKey, pvData, dwSize,
			pftExpireTime, hInstClient, pClient, phEntry);
		return hr;
	}
	
	HRESULT STDMETHODCALLTYPE LookupEntry(LPCSTR szKey, HANDLE * phEntry) throw()
	{
		return cacheBase::LookupEntry(szKey, phEntry);
	}

	HRESULT STDMETHODCALLTYPE GetData(const HANDLE hKey, void **ppvData, DWORD *pdwSize) const throw()
	{
		return cacheBase::GetEntryData(hKey, ppvData, pdwSize);
	}
	
	HRESULT STDMETHODCALLTYPE ReleaseEntry(const HANDLE hKey) throw()
	{
		return cacheBase::ReleaseEntry(hKey);
	}

	HRESULT STDMETHODCALLTYPE RemoveEntry(const HANDLE hKey) throw()
	{
		return cacheBase::RemoveEntry(hKey);
	}
	
	HRESULT STDMETHODCALLTYPE RemoveEntryByKey(LPCSTR szKey) throw()
	{
		return cacheBase::RemoveEntryByKey(szKey);
	}
	
	HRESULT STDMETHODCALLTYPE Flush() throw()
	{
		return cacheBase::FlushEntries();
	}


	HRESULT STDMETHODCALLTYPE SetMaxAllowedSize(DWORD dwSize) throw()
	{
		return cacheBase::SetMaxAllowedSize(dwSize);
	}

	HRESULT STDMETHODCALLTYPE GetMaxAllowedSize(DWORD *pdwSize) throw()
	{
		return cacheBase::GetMaxAllowedSize(pdwSize);
	}

	HRESULT STDMETHODCALLTYPE SetMaxAllowedEntries(DWORD dwSize) throw()
	{
		return cacheBase::SetMaxAllowedEntries(dwSize);
	}

	HRESULT STDMETHODCALLTYPE GetMaxAllowedEntries(DWORD *pdwSize) throw()
	{
		return cacheBase::GetMaxAllowedEntries(pdwSize);
	}

	HRESULT STDMETHODCALLTYPE ResetCache() throw()
	{
		return cacheBase::ResetCache();
	}

   	// IMemoryCacheStats methods
	HRESULT STDMETHODCALLTYPE ClearStats() throw()
	{
		m_statObj.ResetCounters();
		return S_OK;
	}
	
	HRESULT STDMETHODCALLTYPE GetHitCount(DWORD *pdwSize) throw()
	{
		if (!pdwSize)
			return E_POINTER;
		*pdwSize = m_statObj.GetHitCount();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetMissCount(DWORD *pdwSize) throw()
	{
		if (!pdwSize)
			return E_POINTER;
		*pdwSize = m_statObj.GetMissCount();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetMaxAllocSize(DWORD *pdwSize) throw()
	{
		if (!pdwSize)
			return E_POINTER;
		*pdwSize = m_statObj.GetMaxAllocSize();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetCurrentAllocSize(DWORD *pdwSize) throw()
	{
		if (!pdwSize)
			return E_POINTER;
		*pdwSize = m_statObj.GetCurrentAllocSize();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetMaxEntryCount(DWORD *pdwSize) throw()
	{
		if (!pdwSize)
			return E_POINTER;
		*pdwSize = m_statObj.GetMaxEntryCount();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetCurrentEntryCount(DWORD *pdwSize) throw()
	{
		if (!pdwSize)
			return E_POINTER;
		*pdwSize = m_statObj.GetCurrentEntryCount();
		return S_OK;
	}

}; // CBlobCache

//
// IDllCache
// an interface that is used to load and unload Dlls
// To use it, construct an instance, call Initialize and pass
// it an IWorkerThreadClient.  The thread procedure will
// call IWorkerThreadClient::Execute after the timeout expires
// To terminate the thread, call Shutdown
//

//
// CDllCache
// This class manages a cache to handle calls to LoadLibrary
// and FreeLibrary.  
// It keeps dlls loaded even after the last call to free library
// a worker thread then calls FreeLibrary on unused dlls
//
#ifndef ATL_DLL_CACHE_TIMEOUT
	#ifdef _DEBUG
		#define ATL_DLL_CACHE_TIMEOUT	1000		// 1 sec default for debug builds
	#else
		#define ATL_DLL_CACHE_TIMEOUT	10*60000	// 10 minute default for retail builds
	#endif
#endif

class CNoDllCachePeer
{
public:
	struct DllInfo
	{
	};

	BOOL Add(HINSTANCE /*hInst*/, DllInfo * /*pInfo*/)
	{
		return TRUE;
	}

	void Remove(HINSTANCE /*hInst*/, DllInfo * /*pInfo*/)
	{
	}
};


template <class CMonitorClass, class Peer=CNoDllCachePeer>
class CDllCache : public IDllCache, 
	public IWorkerThreadClient
{
protected:
	CComCriticalSection m_critSec;
	CSimpleArray<DLL_CACHE_ENTRY> m_Dlls;
	CSimpleArray<Peer::DllInfo> m_DllInfos;
	CMonitorClass m_Monitor;
	HANDLE m_hTimer;


public:
	Peer m_Peer;

	CDllCache() :
		m_hTimer(INVALID_HANDLE_VALUE)
	{

	}

__if_exists(CMonitorClass::Initialize)
{
	HRESULT Initialize(DWORD dwTimeout=ATL_DLL_CACHE_TIMEOUT) throw()
	{
		HRESULT hr = m_critSec.Init();
		if (FAILED(hr))
			return hr;
		hr = m_Monitor.Initialize();
		if (FAILED(hr))
			return hr;
		return m_Monitor.AddTimer(dwTimeout, this, 0, &m_hTimer);
	}
}

__if_exists(CMonitorClass::InitializeWorker)
{
	template <class ThreadTraits>
	HRESULT Initialize(CWorkerThread<ThreadTraits> *pWorkerThread,
			DWORD dwTimeout=ATL_DLL_CACHE_TIMEOUT) throw()
	{
		HRESULT hr = m_critSec.Init();
		if (FAILED(hr))
			return hr;
		hr = m_Monitor.InitializeWorker(pWorkerThread);
		if (FAILED(hr))
			return hr;
		return m_Monitor.AddTimer(dwTimeout, this, 0, &m_hTimer);
	}
}

	HRESULT Shutdown() throw()
	{
        HRESULT hr = S_OK;

		if (m_hTimer)
		{
			m_Monitor.RemoveHandle(m_hTimer);
			m_hTimer = NULL;
		}
		m_Monitor.Shutdown();

		// free all the libraries we've cached
		int nLen = m_Dlls.GetSize();
		for (int i=0; i<nLen; i++)
		{
			DLL_CACHE_ENTRY& entry = m_Dlls[i];
			ATLASSERT(entry.dwRefs == 0);
			BOOL bRet = ::FreeLibrary(entry.hInstDll);

            if (!bRet)
            {
                hr = AtlHresultFromLastError();
                ATLTRACE(atlTraceCache, 0, _T("Free library failed on shutdown of dll cache : hr = 0x%08x)"), hr);
            }

			nLen--;
		}
		m_Dlls.RemoveAll();
		m_critSec.Term();
		return hr;

	}

	// IUnknown methods
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppv) throw()
	{
		if (!ppv)
			return E_POINTER;
		if (InlineIsEqualGUID(riid, __uuidof(IUnknown)) ||
			InlineIsEqualGUID(riid, __uuidof(IDllCache)))
		{
			*ppv = (IUnknown *) this;
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}
	
	ULONG STDMETHODCALLTYPE AddRef() throw()
	{
		return 1;
	}
	
	ULONG STDMETHODCALLTYPE Release() throw()
	{
		return 1;
	}

	// IDllCache methods
	HINSTANCE Load(LPCSTR szDllName, void **pPeerInfo) throw()
	{
		m_critSec.Lock();
		int nLen = m_Dlls.GetSize();
		for (int i=0; i<nLen; i++)
		{
			DLL_CACHE_ENTRY& entry = m_Dlls[i];
			if (!_stricmp(entry.szDllName, szDllName))
			{
				entry.dwRefs++;
				m_critSec.Unlock();
				if (pPeerInfo)
					*pPeerInfo = &m_DllInfos[i];
				return entry.hInstDll;
			}
		}
		DLL_CACHE_ENTRY entry;
		entry.hInstDll = ::LoadLibraryA(szDllName);
		if (!entry.hInstDll)
		{
			m_critSec.Unlock();
			return NULL;
		}
		strcpy(entry.szDllName, szDllName);
		entry.dwRefs = 1;
		m_Dlls.Add(entry);

		Peer::DllInfo dllInfo;

		if (!m_Peer.Add(entry.hInstDll, &dllInfo))
		{
			::FreeLibrary(entry.hInstDll);
			m_Dlls.RemoveAt(m_Dlls.GetSize()-1);
			m_critSec.Unlock();
			return NULL;
		}

		m_DllInfos.Add(dllInfo);

		if (pPeerInfo)
			*pPeerInfo = &m_DllInfos[m_DllInfos.GetSize()-1];
		m_critSec.Unlock();
		return entry.hInstDll;
	}

	BOOL Free(HINSTANCE hInstDll) throw()
	{
		m_critSec.Lock();
		int nLen = m_Dlls.GetSize();
		for (int i=0; i<nLen; i++)
		{
			DLL_CACHE_ENTRY &entry = m_Dlls[i];
			if (entry.hInstDll == hInstDll)
			{
				ATLASSERT(entry.dwRefs > 0);
				entry.dwRefs--;
				m_critSec.Unlock();
				return TRUE;
			}
		}

		m_critSec.Unlock();
		// the dll wasn't found
		// in the cache, so just
		// pass along to ::FreeLibrary
		return ::FreeLibrary(hInstDll);
	}

	BOOL AddRefModule(HINSTANCE hInstDll) throw()
	{
		m_critSec.Lock();
		int nLen = m_Dlls.GetSize();
		for (int i=0; i<nLen; i++)
		{
			DLL_CACHE_ENTRY &entry = m_Dlls[i];
			if (entry.hInstDll == hInstDll)
			{
				ATLASSERT(entry.dwRefs > 0);
				entry.dwRefs++;
				m_critSec.Unlock();
				return TRUE;
			}
		}

		m_critSec.Unlock();
		return FALSE;
	}

	BOOL ReleaseModule(HINSTANCE hInstDll) throw()
	{
		m_critSec.Lock();
		int nLen = m_Dlls.GetSize();
		for (int i=0; i<nLen; i++)
		{
			DLL_CACHE_ENTRY &entry = m_Dlls[i];
			if (entry.hInstDll == hInstDll)
			{
				ATLASSERT(entry.dwRefs > 0);
				entry.dwRefs--;
				m_critSec.Unlock();
				return TRUE;
			}
		}
		m_critSec.Unlock();
		return FALSE;
	}

	HRESULT GetEntries(DWORD dwCount, DLL_CACHE_ENTRY *pEntries, DWORD *pdwCopied) throw()
	{
		if (!pdwCopied)
			return E_POINTER;

		m_critSec.Lock();
		if (dwCount==0 || pEntries==NULL)
		{
			// just return the required size
			*pdwCopied = m_Dlls.GetSize();
			m_critSec.Unlock();
			return S_OK;
		}

		if (dwCount > (DWORD) m_Dlls.GetSize())
			dwCount = m_Dlls.GetSize();
		memcpy(pEntries, m_Dlls.GetData(), dwCount*sizeof(DLL_CACHE_ENTRY));
		*pdwCopied = dwCount;
		m_critSec.Unlock();
		return S_OK;
	}

    HRESULT Flush()
    {
		m_critSec.Lock();
		int nLen = m_Dlls.GetSize();
		for (int i=0; i<nLen; i++)
		{
			DLL_CACHE_ENTRY &entry = m_Dlls[i];
			if (entry.dwRefs == 0)
			{
				m_Peer.Remove(entry.hInstDll, &m_DllInfos[i]);
				::FreeLibrary(entry.hInstDll);
				m_Dlls.RemoveAt(i);
				m_DllInfos.RemoveAt(i);
				i--;
				nLen--;
			}
		}

		m_critSec.Unlock();
        return S_OK;
    }

	HRESULT Execute(DWORD_PTR /*dwParam*/, HANDLE /*hObject*/) throw()
	{
        Flush();
		return S_OK;
	}

	HRESULT CloseHandle(HANDLE hObject) throw()
	{
		ATLASSERT(m_hTimer == hObject);
		m_hTimer = NULL;
		::CloseHandle(hObject);
		return S_OK;
	}
}; // CDllCache


//
//IStencilCache
//IStencilCache is used by a stencil processor to cache pointers to CStencil
//derived objects.


// {8702269B-707D-49cc-AEF8-5FFCB3D6891B}
extern "C" __declspec(selectany) IID IID_IStencilCache = { 0x8702269b, 0x707d, 0x49cc, { 0xae, 0xf8, 0x5f, 0xfc, 0xb3, 0xd6, 0x89, 0x1b } };

struct ATL_NO_VTABLE __declspec(uuid("8702269B-707D-49cc-AEF8-5FFCB3D6891B")) 
	IStencilCache : public IUnknown
{
	// IStencilCache methods
	STDMETHOD(CacheStencil)(LPCSTR szName, //a name for this cache entry
								void *pStencil, //a pointer to a CStencil derived object
								DWORD dwSize, //sizeof pStencil
								HANDLE *pHandle, //out pointer to a handle to the this cache entry
								HINSTANCE hInst, //HINSTANCE of the module putting this entry
												 //in the cache.
								IMemoryCacheClient *pClient //Interface used to free this instance
								) = 0;
	STDMETHOD(LookupStencil)(LPCSTR szName, HANDLE * phStencil) = 0;
	STDMETHOD(GetStencil)(const HANDLE hStencil, void ** ppStencil) const = 0;
	STDMETHOD(AddRefStencil)(const HANDLE hStencil) = 0;
	STDMETHOD(ReleaseStencil)(const HANDLE hStencil) = 0;
};

// {55DEF119-D7A7-4eb7-A876-33365E1C5E1A}
extern "C" __declspec(selectany) IID IID_IStencilCacheControl = { 0x55def119, 0xd7a7, 0x4eb7, { 0xa8, 0x76, 0x33, 0x36, 0x5e, 0x1c, 0x5e, 0x1a } };
struct ATL_NO_VTABLE __declspec(uuid("55DEF119-D7A7-4eb7-A876-33365E1C5E1A"))
IStencilCacheControl : public IUnknown
{
	//IStencilCacheControl
	STDMETHOD(RemoveStencil)(const HANDLE hStencil) = 0; // Removes the stencil if there are no references,
                                                         // otherwise detaches it
	STDMETHOD(RemoveStencilByName)(LPCSTR szStencil) = 0; //removes a stencil if there are no
													//references to it
	STDMETHOD(RemoveAllStencils)() = 0; //removes all stencils that don't have references on them
	STDMETHOD(SetDefaultLifespan)(unsigned __int64 dwdwLifespan) = 0; //sets the lifespan for all stencils
															 //in the cache (in 100 nanosecond units (10,000,000=1 second)).
	STDMETHOD(GetDefaultLifespan)(unsigned __int64 *pdwdwLifespan) = 0;
};

#ifndef ATL_STENCIL_CACHE_TIMEOUT
	#define ATL_STENCIL_CACHE_TIMEOUT 1000
#endif

#ifndef ATL_STENCIL_LIFESPAN
#ifdef _DEBUG                    
	#define ATL_STENCIL_LIFESPAN CFileTime::Second
#else
	#define ATL_STENCIL_LIFESPAN CFileTime::Hour
#endif
#endif

// timeout before we check if the file
// has changed in m.s.
#ifndef ATL_STENCIL_CHECK_TIMEOUT
	#define ATL_STENCIL_CHECK_TIMEOUT 1000
#endif

template <class MonitorClass, class StatClass=CStdStatClass, class SyncClass=CComCriticalSection>
class CStencilCache :
	public CMemoryCacheBase<CStencilCache, void *, CCacheDataEx, 
        CFixedStringKey,  CStringElementTraitsI<CFixedStringKey >, 
        COldFlusher, CLifetimeCuller, SyncClass, StatClass>,
	public IStencilCache,
	public IStencilCacheControl,
	public IWorkerThreadClient,
    public IMemoryCacheStats,
	public CComObjectRootEx<CComGlobalsThreadModel>
{
protected:
	typedef CMemoryCacheBase<CStencilCache, void *, CCacheDataEx, 
        CFixedStringKey,  CStringElementTraitsI<CFixedStringKey >, 
        COldFlusher, CLifetimeCuller, SyncClass, StatClass> cacheBase;
	unsigned __int64 m_dwdwStencilLifespan;

	MonitorClass m_Monitor;
	HANDLE m_hTimer;
	CComPtr<IDllCache> m_spDllCache;

public:

	CStencilCache() throw() :
		m_dwdwStencilLifespan(ATL_STENCIL_LIFESPAN)
	{

	}

	~CStencilCache() throw()
	{
		if (m_hTimer)
			m_Monitor.RemoveHandle(m_hTimer);
	}

	HRESULT Execute(DWORD_PTR dwParam, HANDLE /*hObject*/) throw()
	{
		CStencilCache* pCache = (CStencilCache*)dwParam;
		if (pCache)
			pCache->FlushEntries();
		return S_OK;
	}

	HRESULT CloseHandle(HANDLE hObject) throw()
	{
		ATLASSERT(m_hTimer == hObject);
		m_hTimer = NULL;
		::CloseHandle(hObject);
		return S_OK;
	}

__if_exists(MonitorClass::Initialize)
{
	HRESULT Initialize(IServiceProvider *pProv, DWORD dwStencilCacheTimeout=ATL_STENCIL_CACHE_TIMEOUT, 
		__int64 dwdwStencilLifespan=ATL_STENCIL_LIFESPAN) throw()
	{
		m_dwdwStencilLifespan = dwdwStencilLifespan;
		HRESULT hr = cacheBase::Initialize();
		if (FAILED(hr))
			return hr;
        hr = E_FAIL;
        if (pProv)
            hr = pProv->QueryService(__uuidof(IDllCache), __uuidof(IDllCache), (void**)&m_spDllCache);
		if (FAILED(hr))
			return hr;
		hr = m_Monitor.Initialize();
		if (FAILED(hr))
			return hr;
		return m_Monitor.AddTimer(dwStencilCacheTimeout, this, (DWORD_PTR) this, &m_hTimer);
	}
}

__if_exists(MonitorClass::InitializeWorker)
{
	template <class ThreadTraits>
	HRESULT Initialize(IServiceProvider *pProv, CWorkerThread<ThreadTraits> *pWorkerThread, 
		DWORD dwStencilCacheTimeout=ATL_STENCIL_CACHE_TIMEOUT, __int64 dwdwStencilLifespan=ATL_STENCIL_LIFESPAN) throw()
	{
		m_dwdwStencilLifespan = dwdwStencilLifespan;
		HRESULT hr = cacheBase::Initialize();
		if (FAILED(hr))
			return hr;
        hr = E_FAIL;
        if (pProv)
            hr = pProv->QueryService(__uuidof(IDllCache), __uuidof(IDllCache), (void**)&m_spDllCache);
		if (FAILED(hr))
			return hr;
		hr = m_Monitor.InitializeWorker(pWorkerThread);
		if (FAILED(hr))
			return hr;
		return m_Monitor.AddTimer(dwStencilCacheTimeout, this, (DWORD_PTR) this, &m_hTimer);
	}
}

	BEGIN_COM_MAP(CStencilCache)
		COM_INTERFACE_ENTRY(IMemoryCacheStats)
		COM_INTERFACE_ENTRY(IStencilCache)
		COM_INTERFACE_ENTRY(IStencilCacheControl)
	END_COM_MAP()
//IStencilCache methods
	STDMETHOD(CacheStencil)(LPCSTR szName, void *pStencil, DWORD dwSize, HANDLE *phEntry,
				HINSTANCE hInstance, IMemoryCacheClient *pClient) throw()
	{
        NodeType * pEntry = NULL;
        m_syncObj.Lock();
		HRESULT hr = cacheBase::AddEntry(szName, pStencil, dwSize,	(HANDLE *)&pEntry);
        if (hr != S_OK)
        {
            m_syncObj.Unlock();
            return hr;
        }

        pEntry->hInstance = hInstance;
        pEntry->pClient = pClient;
        pEntry->nLifespan = m_dwdwStencilLifespan;
		if (hInstance && m_spDllCache)
			m_spDllCache->AddRefModule(hInstance);

        cacheBase::CommitEntry((HANDLE)pEntry);

        if (phEntry)
            *phEntry = (HANDLE)pEntry;
        else
            cacheBase::ReleaseEntry(pEntry);

        m_syncObj.Unlock();
		return hr;
	}

	STDMETHOD(LookupStencil)(LPCSTR szName, HANDLE * phStencil) throw()
	{
		return cacheBase::LookupEntry(szName, phStencil);
	}

	STDMETHOD(GetStencil)(const HANDLE hStencil, void ** pStencil) const throw()
	{
		return cacheBase::GetEntryData(hStencil, pStencil, NULL);
	}
	
	STDMETHOD(AddRefStencil)(const HANDLE hStencil) throw()
	{
		return cacheBase::AddRefEntry(hStencil);
	}

	STDMETHOD(ReleaseStencil)(const HANDLE hStencil) throw()
	{
		return cacheBase::ReleaseEntry(hStencil);
	}

	//IStencilCacheControl

	STDMETHOD(RemoveStencil)(const HANDLE hStencil) throw()
	{
		return cacheBase::RemoveEntry(hStencil);
	}

	STDMETHOD(RemoveStencilByName)(LPCSTR szStencil)
	{
		return cacheBase::RemoveEntryByKey(szStencil);
	}

	STDMETHOD(RemoveAllStencils)() throw()
	{
		return cacheBase::RemoveAllEntries();
	}

	STDMETHOD(SetDefaultLifespan)(unsigned __int64 dwdwLifespan) throw()
	{
		m_dwdwStencilLifespan = dwdwLifespan;
		return S_OK;
	}

	STDMETHOD(GetDefaultLifespan)(unsigned __int64 *pdwdwLifepsan) throw()
	{
		HRESULT hr = E_POINTER;
		if (pdwdwLifepsan)
		{
			*pdwdwLifepsan = m_dwdwStencilLifespan;
			hr = S_OK;
		}
		return hr;
	}

   	virtual void OnDestroyEntry(const NodeType * pEntry) throw()
	{
		ATLASSERT(pEntry);
		if (!pEntry)
			return;

		if (pEntry->pClient)
			pEntry->pClient->Free((void *)&pEntry->Data);
		if (pEntry->hInstance && m_spDllCache)
			m_spDllCache->ReleaseModule(pEntry->hInstance);
    }

	HRESULT Uninitialize() throw()
	{
		if (m_hTimer)
		{
			m_Monitor.RemoveHandle(m_hTimer);
			m_hTimer = NULL;
		}
		m_Monitor.Shutdown();
		return cacheBase::Uninitialize();
	}
   	// IMemoryCacheStats methods
	HRESULT STDMETHODCALLTYPE ClearStats() throw()
	{
		m_statObj.ResetCounters();
		return S_OK;
	}
	
	HRESULT STDMETHODCALLTYPE GetHitCount(DWORD *pdwSize) throw()
	{
		if (!pdwSize)
			return E_POINTER;
		*pdwSize = m_statObj.GetHitCount();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetMissCount(DWORD *pdwSize) throw()
	{
		if (!pdwSize)
			return E_POINTER;
		*pdwSize = m_statObj.GetMissCount();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetMaxAllocSize(DWORD *pdwSize) throw()
	{
		if (!pdwSize)
			return E_POINTER;
		*pdwSize = m_statObj.GetMaxAllocSize();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetCurrentAllocSize(DWORD *pdwSize) throw()
	{
		if (!pdwSize)
			return E_POINTER;
		*pdwSize = m_statObj.GetCurrentAllocSize();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetMaxEntryCount(DWORD *pdwSize) throw()
	{
		if (!pdwSize)
			return E_POINTER;
		*pdwSize = m_statObj.GetMaxEntryCount();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetCurrentEntryCount(DWORD *pdwSize) throw()
	{
		if (!pdwSize)
			return E_POINTER;
		*pdwSize = m_statObj.GetCurrentEntryCount();
		return S_OK;
	}
}; // CStencilCache

// {105A8866-4059-45fe-86AE-FA0EABBFBBB4}
extern "C" __declspec(selectany) IID IID_IFileCache = { 0x105a8866, 0x4059, 0x45fe, { 0x86, 0xae, 0xfa, 0xe, 0xab, 0xbf, 0xbb, 0xb4 } };

struct ATL_NO_VTABLE __declspec(uuid("105A8866-4059-45fe-86AE-FA0EABBFBBB4"))
	IFileCache : public IUnknown
{
	// IFileCache Methods

	STDMETHOD(AddFile)( 
		LPCSTR szFileName,
        LPCSTR szTempFileName,
		FILETIME *pftExpireTime,
		HANDLE * phKey) = 0;
	STDMETHOD(LookupFile)(LPCSTR szFileName, HANDLE * phKey) = 0;
	STDMETHOD(GetFile)(const HANDLE hKey, LPSTR * pszFileName) = 0;
	STDMETHOD(ReleaseFile)(const HANDLE hKey) = 0;
	STDMETHOD(RemoveFile)(const HANDLE hKey) = 0;
	STDMETHOD(RemoveFileByName)(LPCSTR szFileName) = 0;
	STDMETHOD(Flush)() = 0;
};

#ifndef ATL_FILE_CACHE_TIMEOUT
	#define ATL_FILE_CACHE_TIMEOUT 1000
#endif

// A class to keep track of files, with maintenance -- maximum size of cache,
// maximum number of entries, expiration of entries, etc. -- inherits from
// CMemoryCacheBase
template <
        class MonitorClass,
        class StatClass=CStdStatClass,
        class FlushClass=COldFlusher,
		class SyncClass=CComCriticalSection >
class CFileCache:
	public CMemoryCacheBase<CFileCache, LPSTR, CCacheDataBase, 
            CFixedStringKey,  CStringElementTraits<CFixedStringKey >, 
            FlushClass, CExpireCuller, SyncClass, StatClass>, 
	public IWorkerThreadClient,
	public IFileCache,
	public IMemoryCacheControl,
    public IMemoryCacheStats
{
    typedef CMemoryCacheBase<CFileCache, LPSTR, CCacheDataBase, 
            CFixedStringKey,  CStringElementTraits<CFixedStringKey >, 
            FlushClass, CExpireCuller, SyncClass, StatClass> cacheBase;

	MonitorClass m_Monitor;

protected:
	HANDLE m_hTimer;

public:
	HRESULT Initialize() throw()
	{
		HRESULT hr = cacheBase::Initialize();
		if (FAILED(hr))
			return hr;
		hr = m_Monitor.Initialize();
		if (FAILED(hr))
	    	return hr;
		return m_Monitor.AddTimer(ATL_FILE_CACHE_TIMEOUT, 
			static_cast<IWorkerThreadClient*>(this), (DWORD_PTR) this, &m_hTimer);
	}

	template <class ThreadTraits>
	HRESULT InitializeWorker(CWorkerThread<ThreadTraits> *pWorkerThread) throw()
	{
		ATLASSERT(pWorkerThread);

		HRESULT hr = S_OK;

		if (S_OK == cacheBase::Initialize())
		{
			hr = m_Monitor.InitializeWorker(pWorkerThread);
			if (FAILED(hr))
				return hr;
			return m_Monitor.AddTimer(ATL_FILE_CACHE_TIMEOUT, 
				static_cast<IWorkerThreadClient*>(this), (DWORD_PTR) this, &m_hTimer);
		}
		return S_OK;
	}


	// Callback for CWorkerThread
	HRESULT Execute(DWORD_PTR dwParam, HANDLE /*hObject*/) throw()
	{
		CFileCache* pCache = (CFileCache*)dwParam;
	
		if (pCache)
			pCache->Flush();
		return S_OK;
	}

	HRESULT CloseHandle(HANDLE hObject) throw()
	{
		ATLASSERT(m_hTimer == hObject);
		m_hTimer = NULL;
		::CloseHandle(hObject);
		return S_OK;
	}

	~CFileCache() throw()
	{
		if (m_hTimer)
		{
			m_Monitor.RemoveHandle(m_hTimer);
			m_hTimer = NULL;
		}
	}

	HRESULT Uninitialize() throw()
	{
		if (m_hTimer)
		{
			m_Monitor.RemoveHandle(m_hTimer);
			m_hTimer = NULL;
		}
		m_Monitor.Shutdown();
		return cacheBase::Uninitialize();
	}


	// IUnknown methods
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppv) throw()
	{
		HRESULT hr = E_NOINTERFACE;
		if (!ppv)
			hr = E_POINTER;
		else
		{
			if (InlineIsEqualGUID(riid, __uuidof(IUnknown)) ||
				InlineIsEqualGUID(riid, __uuidof(IFileCache)))
			{
				*ppv = (IUnknown *) (IFileCache *) this;
				AddRef();
				hr = S_OK;
			}
			if (InlineIsEqualGUID(riid, __uuidof(IMemoryCacheStats)))
			{
				*ppv = (IMemoryCacheStats*)this;
				AddRef();
				hr = S_OK;
			}
			if (InlineIsEqualGUID(riid, __uuidof(IMemoryCacheControl)))
			{
				*ppv = (IMemoryCacheControl*)this;
				AddRef();
				hr = S_OK;
			}

		}
		return hr;
	}
	
	ULONG STDMETHODCALLTYPE AddRef() throw()
	{
		return 1;
	}
	
	ULONG STDMETHODCALLTYPE Release() throw()
	{
		return 1;
	}

	// Adds a file to the cache.  A file is created with a 
	// temporary name, and then Add is called with the temp
	// file name and the final file name, along with expiration data,
	// etc.  A search on the file name will return the name of
	// the file on disk (i.e. the temporary file)
	HRESULT STDMETHODCALLTYPE AddFile(
		LPCSTR szFileName,
        LPCSTR szTempFileName, 
		FILETIME *pftExpireTime, 
		HANDLE * phKey = NULL) throw()
	{
		WIN32_FILE_ATTRIBUTE_DATA fadData;
		BOOL bRet = GetFileAttributesExA(szTempFileName, GetFileExInfoStandard, &fadData);
        if (!bRet)
            return AtlHresultFromLastError();

		__int64 ddwFileSize = (fadData.nFileSizeHigh << 32) + fadData.nFileSizeLow;

		DWORD dwRecordedFileSize = (DWORD) (ddwFileSize >> 10);
		// Round the file size up to 1K if it is < 1K
		if (dwRecordedFileSize == 0) 
			dwRecordedFileSize = 1;

		if (m_Monitor.GetThreadHandle()==NULL)
		{
			if (!cacheBase::CanAddEntry(dwRecordedFileSize))
			{
				cacheBase::FlushEntries();
				if (!cacheBase::CanAddEntry(dwRecordedFileSize))
					return E_OUTOFMEMORY;
			}
		}

		// Make a private copy of the file name
		LPSTR szTempFileCopy = (CHAR *)malloc(MAX_PATH*sizeof(CHAR));

		strcpy(szTempFileCopy, szTempFileName);
        NodeType *pEntry = NULL;
        m_syncObj.Lock();
		HRESULT hr = cacheBase::AddEntry(szFileName, szTempFileCopy, dwRecordedFileSize,  
			(HANDLE *)&pEntry);
        if (hr != S_OK)
        {
            m_syncObj.Unlock();
            return hr;
        }


		if (pftExpireTime)
			pEntry->cftExpireTime = *pftExpireTime;

        cacheBase::CommitEntry((HANDLE)pEntry);
        if (phKey)
            *phKey = (HANDLE)pEntry;
        else
            cacheBase::ReleaseEntry(pEntry);

        m_syncObj.Unlock();
		return hr;
	}

    // Action to take when the entry is removed from the cache
	virtual void OnDestroyEntry(const NodeType * pEntry) throw()
	{
		ATLASSERT(pEntry);
		if (!pEntry)
			return;
        DeleteFileA(pEntry->Data);
        free(pEntry->Data);
	}

	// Looks up a file by name.  Must be released after use
	HRESULT STDMETHODCALLTYPE LookupFile(LPCSTR szFileName, HANDLE * phKey) throw()
	{
		return cacheBase::LookupEntry(szFileName, phKey);
	}

	// Gets the name of the file on disk
	HRESULT STDMETHODCALLTYPE GetFile(const HANDLE hKey, LPSTR * pszFileName) throw()
	{
		return cacheBase::GetEntryData(hKey, pszFileName, NULL);
	}

	// Releases a file
	HRESULT STDMETHODCALLTYPE ReleaseFile(const HANDLE hKey) throw()
	{
		return cacheBase::ReleaseEntry(hKey);
	}

	// Releases a file and marks it for deletion
	HRESULT STDMETHODCALLTYPE RemoveFile(const HANDLE hKey) throw()
	{
		return cacheBase::RemoveEntry(hKey);
	}

	// Removes a file by name -- this calls IMemoryCacheClient->Free
	// on the file name, which by default (for CFileCache) deletes the
	// file.
	HRESULT STDMETHODCALLTYPE RemoveFileByName(LPCSTR szFileName) throw()
	{
		return cacheBase::RemoveEntryByKey(szFileName);
	}

	// Flushes the entries in the cache, eliminates expired entries,
	// or if the cache exceeds the parameters (alloc size, num entries),
	// culls items based on the sweep mode
	HRESULT STDMETHODCALLTYPE Flush() throw()
	{
		return cacheBase::FlushEntries();
	}

	// IMemoryCacheControl methods
	HRESULT STDMETHODCALLTYPE SetMaxAllowedSize(DWORD dwSize) throw()
	{
		return cacheBase::SetMaxAllowedSize(dwSize);
	}

	HRESULT STDMETHODCALLTYPE GetMaxAllowedSize(DWORD *pdwSize) throw()
	{
		return cacheBase::GetMaxAllowedSize(pdwSize);
	}

	HRESULT STDMETHODCALLTYPE SetMaxAllowedEntries(DWORD dwSize) throw()
	{
		return cacheBase::SetMaxAllowedEntries(dwSize);
	}

	HRESULT STDMETHODCALLTYPE GetMaxAllowedEntries(DWORD *pdwSize) throw()
	{
		return cacheBase::GetMaxAllowedEntries(pdwSize);
	}

	HRESULT STDMETHODCALLTYPE ResetCache() throw()
	{
		return cacheBase::ResetCache();
	}


   	// IMemoryCacheStats methods
	HRESULT STDMETHODCALLTYPE ClearStats() throw()
	{
		m_statObj.ResetCounters();
		return S_OK;
	}
	
	HRESULT STDMETHODCALLTYPE GetHitCount(DWORD *pdwSize) throw()
	{
		if (!pdwSize)
			return E_POINTER;
		*pdwSize = m_statObj.GetHitCount();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetMissCount(DWORD *pdwSize) throw()
	{
		if (!pdwSize)
			return E_POINTER;
		*pdwSize = m_statObj.GetMissCount();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetMaxAllocSize(DWORD *pdwSize) throw()
	{
		if (!pdwSize)
			return E_POINTER;
		*pdwSize = m_statObj.GetMaxAllocSize();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetCurrentAllocSize(DWORD *pdwSize) throw()
	{
		if (!pdwSize)
			return E_POINTER;
		*pdwSize = m_statObj.GetCurrentAllocSize();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetMaxEntryCount(DWORD *pdwSize) throw()
	{
		if (!pdwSize)
			return E_POINTER;
		*pdwSize = m_statObj.GetMaxEntryCount();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetCurrentEntryCount(DWORD *pdwSize) throw()
	{
		if (!pdwSize)
			return E_POINTER;
		*pdwSize = m_statObj.GetCurrentEntryCount();
		return S_OK;
	}
}; // CFileCache

class CDataConnection; // see atldbcli.h
struct __declspec(uuid("52E7759B-D6CC-4a03-BDF3-80A6BDCA1F94")) 
IDataSourceCache : public IUnknown
{	
	// Method: Add
	// Params:
	//   szConn: Connection string of data source to connect to
	//   ppDS: Out pointer to the newly added data source
	// Comments:
	//   Attempts to open a connection to the specified data source
	//   using a CDataSource object. Once the connection is open, the
	//   CDatasource is cached.
	STDMETHOD(Add)(LPCTSTR szID, LPCTSTR szConn, CDataConnection *pDS) = 0;

	// Method: Remove
	// Params:
	//   szConn: Specifies the connection string of the connection to close
	// Comments:
	//   Closes the specified connection and removes it's entry from the cache
	STDMETHOD(Remove)(LPCTSTR szID) = 0;

	// Method: Lookup
	// Params:
	//   szConn: Specifies the connection string of the connection to look up
	//   ppDS: Out pointer to CDataSource object that is connected to the specified
	//        data source.
	STDMETHOD(Lookup)(LPCTSTR szID, CDataConnection *pDS) = 0;

	// Method: Shutdown
	// Params: 
	//   None
	// Comments:
	//   Closes removes all connections from the cache.
	STDMETHOD(Shutdown)() = 0;

};
#ifndef ATL_DS_CONN_STRING_LEN
	#define ATL_DS_CONN_STRING_LEN 512
#endif

template <>
class CElementTraitsBase< CDataConnection >
{
public:
	typedef const CDataConnection& INARGTYPE;
	typedef CDataConnection& OUTARGTYPE;
	typedef DWORD SORTKEY;
	typedef CUINTHash HASHTYPE;
	
	static SORTKEY GetSortKey( INARGTYPE t )
	{
		(void)t;
		return( 0 );
	}

	static ULONG Hash( INARGTYPE t )
	{
		return( ULONG( ULONG_PTR( &t ) ) );
	}

	static void CopyElements( T* pDest, const T* pSrc, size_t nElements )
	{
		if ( nElements > 0 && pDest && pSrc)
		{
			for( size_t iElement = 0; iElement < nElements; iElement++ )
			{
				pDest[iElement] = pSrc[iElement];
			}
		}
	}

	static void RelocateElements( T* pDest, T* pSrc, size_t nElements )
	{
		// A simple memmove works for nearly all types.
		// You'll have to override this for types that have pointers to their
		// own members.
		memmove( pDest, pSrc, nElements*sizeof( T ) );
	}

	static bool CompareElements( INARGTYPE element1, INARGTYPE element2 )
	{
		return( element1.m_session.m_spOpenRowset == element2.m_session.m_spOpenRowset);
	}

	static int CompareElementsOrdered( INARGTYPE /*element1*/, INARGTYPE /*element2*/ )
	{
		ATLASSERT(FALSE);
		return -1;
	}

};

typedef CFixedStringT<CString, ATL_DS_CONN_STRING_LEN> atlDataSourceKey;
typedef CAtlMap<atlDataSourceKey, CDataConnection, 
			CStringElementTraits<atlDataSourceKey>, CElementTraits<CDataConnection> > atlDataSourceCacheMap;

template <class TCritSec=CComFakeCriticalSection>
class CDataSourceCache :
	public IDataSourceCache,
	public CComObjectRootEx<CComGlobalsThreadModel>
{
public:
	BEGIN_COM_MAP(CDataSourceCache)
		COM_INTERFACE_ENTRY(IDataSourceCache)
	END_COM_MAP()

	CDataSourceCache() throw()
	{
		m_cs.Init();
	}

	virtual ~CDataSourceCache () throw()
	{
		Shutdown();
	}

	STDMETHOD(Shutdown)() throw()
	{
		m_cs.Lock();
		m_ConnectionMap.RemoveAll();
		m_cs.Unlock();
		return S_OK;
	}

	STDMETHOD(Add)(LPCTSTR szID, LPCTSTR szConn, CDataConnection *pSession) throw()
	{
		HRESULT hr = E_FAIL;
		if (pSession)
			*pSession = NULL;

		if (!szID)
			return E_INVALIDARG; // must have session name

		// Do a lookup to make sure we don't add multiple entries
		// with the same name. Adding multiple entries with the same name
		// could cause some entries to get orphaned.
		m_cs.Lock();
		const atlDataSourceCacheMap::CPair *pPair = 
			m_ConnectionMap.Lookup(szID);
		if (!pPair)
		{
			// try to open connection
			CDataConnection DS;
			hr = DS.Open(szConn);		
			if (hr == S_OK)
			{
				if (m_ConnectionMap.SetAt(szID, DS))
				{
					if (pSession)
						*pSession = DS; // copy connection to output.
					hr = S_OK;
				}
				else
					hr = E_FAIL; // map add failed
			}
		}
		else // lookup succeeded, entry is already in cache
		{
			// Instead of opening a new connection, just copy
			// the one we already have in the cache.
			if (pSession)
				*pSession = pPair->m_value;
			hr = S_OK;
		}
		m_cs.Unlock();
		return hr;
	}

	STDMETHOD(Remove)(LPCTSTR szID) throw()
	{
		HRESULT hr = E_INVALIDARG;
		if (!szID)
			return hr; // must have session name

		m_cs.Lock();
		hr = m_ConnectionMap.RemoveKey(szID) ? S_OK : E_FAIL;
		m_cs.Unlock();
		return hr;
	}

	STDMETHOD(Lookup)(LPCTSTR szID, CDataConnection *pSession) throw()
	{
		if (!szID||!pSession)
			return E_POINTER;
			
		m_cs.Lock();
		bool bRet = m_ConnectionMap.Lookup(szID, *pSession);
		m_cs.Unlock();
		return (bRet && (bool)*pSession)? S_OK : E_FAIL;
	}

protected:
	atlDataSourceCacheMap m_ConnectionMap;
	TCritSec m_cs;
};


// Some helpers for using the datasource cache.
//
// Function: GetDataSource
// Params:
//   pProvider: Pointer to IServiceProvider that provides the 
//              data source cache service
//   szID: The name of the connection (can be same as szDS)
//   szDS: OLEDB connection string for data source
//   ppDS: Out pointer to CDataSource. The CDataSource will be connected
//         to the OLEDB provider specified by szDS on successful return.
// RetVal:
//   Returns S_OK on success.
static HRESULT ATL_NOINLINE GetDataSource(IServiceProvider *pProvider, 
										  LPCTSTR szID, LPCTSTR szConn, 
										  CDataConnection *pSession) throw()
{
	if (!pProvider || !szID || !szConn || !pSession)
		return E_POINTER;

	CComPtr<IDataSourceCache> spDSCache;
	HRESULT hr;
	hr = pProvider->QueryService(__uuidof(IDataSourceCache), __uuidof(IDataSourceCache), (void**)&spDSCache);
	if (hr == S_OK && spDSCache)
	{
		hr = spDSCache->Add(szID, szConn, pSession);
	}
	return hr;
}

//
// Function: RemoveDataSource
// Params:
//   pProvider: Pointer to IServiceProvider that provides the
//              data source cache service
//   szID: Name of the datasource connection to remove from the cache
// RetVal:
//   none
// Comments:
//   Removes the datasource entry from the datasource cache. Since entries are
//   copied to the client on calls to lookup and add, removing an entry will not
//   release the connections of existing clients.
static void ATL_NOINLINE RemoveDataSource(IServiceProvider *pProvider, LPCTSTR szID) throw()
{
	if (!pProvider || !szID)
		return;

	CComPtr<IDataSourceCache> spDSCache;
	pProvider->QueryService(__uuidof(IDataSourceCache), __uuidof(IDataSourceCache), (void**)&spDSCache);
	if (spDSCache)
		spDSCache->Remove(szID);
}

} // namespace ATL
