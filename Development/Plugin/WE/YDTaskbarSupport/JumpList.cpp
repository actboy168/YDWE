#include "JumpList.h"
#include "Windows7.h"					
#include <Propvarutil.h>
#include <cassert>

#pragma comment(lib, "Shlwapi.lib")

DEFINE_PROPERTYKEY(PKEY_Title, 0xF29F85E0, 0x4FF9, 0x1068, 0xAB, 0x91, 0x08, 0x00, 0x2B, 0x27, 0xB3, 0xD9, 2);
DEFINE_PROPERTYKEY(PKEY_AppUserModel_IsDestListSeparator, 0x9F4C2855, 0x9F79, 0x4B39, 0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3, 6);

JumpList::JumpList(BOOL bAutoCommit) 
{
	m_bInitialized = FALSE;
	m_nMaxSlots = 0;
	m_bAutoCommit = bAutoCommit;
}

JumpList::~JumpList()
{
	if (m_bAutoCommit)
	{
		// calls ClearAll as well
		// can be called when m_bInitialized is FALSE
		CommitList();
	}
	else
	{
		ClearAll();
	}
}

void JumpList::ClearAll()
{
	ClearAllDestinations();
	ReleaseObjectArray(m_tasksPtr.Detach());
	ReleaseObjectArray(m_removedItemsPtr.Detach());

	m_bInitialized = FALSE;
}

void JumpList::AbortList()
{
	if (m_bInitialized)
	{
		assert(m_destListPtr != NULL);
		m_destListPtr->AbortList();
		ClearAll();
		m_bInitialized = FALSE;
	}
}

BOOL JumpList::InitializeList()
{
	HRESULT hr = S_OK;
	if (m_destListPtr == NULL)
	{
		hr = m_destListPtr.CoCreateInstance(CLSID_DestinationList, NULL, CLSCTX_INPROC_SERVER);
		if (FAILED(hr))
		{
			return FALSE;
		}
	}

	if (m_tasksPtr == NULL)
	{
		hr = m_tasksPtr.CoCreateInstance(CLSID_EnumerableObjectCollection, NULL, CLSCTX_INPROC_SERVER);
		if (FAILED(hr))
		{
			return FALSE;
		}
	}

	if (!m_bInitialized)
	{
		if (!m_strAppID.empty())
		{
			m_destListPtr->SetAppID(m_strAppID.c_str());
		}

		hr = m_destListPtr->BeginList(&m_nMaxSlots, IID_IObjectArray, (LPVOID*)&m_removedItemsPtr);
		if (FAILED(hr))
		{
			return FALSE;
		}
		m_bInitialized = TRUE;
	}

	return TRUE;
}

void JumpList::SetAppID(LPCWSTR strAppID)
{
	m_strAppID = strAppID;
}

UINT JumpList::GetMaxSlots() const
{
	assert(m_bInitialized);
	return m_nMaxSlots;
}

BOOL JumpList::AddKnownCategory(KNOWNDESTCATEGORY category)
{
	if (!InitializeList()) // always returns FALSE if OS < Win7 
	{
		return FALSE;
	}

	HRESULT hr = m_destListPtr->AppendKnownCategory(category);
	if (FAILED(hr))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL JumpList::AddTask(LPCWSTR strTitle, std::function<void(CComPtr<IShellLinkW>&)> callback)
{
	if (!InitializeList()) // always returns FALSE if OS < Win7 
	{
		return FALSE;
	}

	CComPtr<IShellLinkW> shellLinkPtr;
	if (FAILED(shellLinkPtr.CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER)))
	{
		return FALSE;
	}

	callback(shellLinkPtr);

	CComQIPtr<IPropertyStore> propPtr = shellLinkPtr;

	if (propPtr != NULL)
	{
		PROPVARIANT var;
		if (FAILED(InitPropVariantFromString(strTitle, &var)))
		{
			return FALSE;
		}

		if (FAILED(propPtr->SetValue(PKEY_Title, var)))
		{
			PropVariantClear(&var);
			return FALSE;
		}

		HRESULT hr = propPtr->Commit();
		PropVariantClear(&var);
		if (FAILED(hr))
		{
			return FALSE;
		}
	}

	return (SUCCEEDED(m_tasksPtr->AddObject(shellLinkPtr.Detach())));
}

BOOL JumpList::AddTask(LPCWSTR strTargetExecutablePath, LPCWSTR strCommandLineArgs, 
						LPCWSTR strTitle, LPCWSTR strIconPath, int iIconIndex)
{
	if (!InitializeList()) // always returns FALSE if OS < Win7 
	{
		return FALSE;
	}

	if (strTargetExecutablePath == NULL || strTitle == NULL)
	{
		return FALSE;
	}

	CComPtr<IShellLinkW> shellLinkPtr;
	if (FAILED(shellLinkPtr.CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER)))
	{
		return FALSE;
	}

	shellLinkPtr->SetPath(strTargetExecutablePath);
	shellLinkPtr->SetArguments(strCommandLineArgs);
	shellLinkPtr->SetIconLocation(strIconPath, iIconIndex);

	CComQIPtr<IPropertyStore> propPtr = shellLinkPtr;

	if (propPtr != NULL)
	{
		PROPVARIANT var;
		if (FAILED(InitPropVariantFromString(strTitle, &var)))
		{
			return FALSE;
		}

		if (FAILED(propPtr->SetValue(PKEY_Title, var)))
		{
			PropVariantClear(&var);
			return FALSE;
		}

		HRESULT hr = propPtr->Commit();
		PropVariantClear(&var);
		if (FAILED(hr))
		{
			return FALSE;
		}
	}

	return (SUCCEEDED(m_tasksPtr->AddObject(shellLinkPtr.Detach())));
}

BOOL JumpList::AddTaskSeparator()
{
	if (!InitializeList()) // always returns FALSE if OS < Win7 
	{
		return FALSE;
	}

	CComPtr<IShellLinkW> shellLinkPtr;
	if (FAILED(shellLinkPtr.CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER)))
	{
		return FALSE;
	}

	CComQIPtr<IPropertyStore> propPtr = shellLinkPtr;
	PROPVARIANT var;
	var.vt = VT_BOOL;
	var.boolVal = VARIANT_TRUE;

	if (FAILED(propPtr->SetValue(PKEY_AppUserModel_IsDestListSeparator, var)))
	{
		PropVariantClear(&var);
		return FALSE;
	}

	HRESULT hr = propPtr->Commit();
	PropVariantClear(&var);
	if (FAILED(hr))
	{
		return FALSE;
	}

	return (SUCCEEDED(m_tasksPtr->AddObject(shellLinkPtr.Detach())));
}

BOOL JumpList::AddTask(IShellLinkW* pShellLink)
{
	assert(pShellLink != NULL);

	if (pShellLink == NULL)
	{
		return FALSE;
	}

	if (!InitializeList())
	{
		return FALSE;
	}

	if (FAILED(m_tasksPtr->AddObject(pShellLink)))
	{
		return FALSE;
	}

	pShellLink->AddRef();
	return TRUE;
}

BOOL JumpList::AddTasks(IObjectArray* pObjectCollection)
{
	if (!InitializeList()) // always returns FALSE if OS < Win7 
	{
		return FALSE;
	}

	assert(pObjectCollection != NULL);

	if (pObjectCollection == NULL)
	{
		return FALSE;
	}

	return SUCCEEDED(m_tasksPtr->AddFromArray(pObjectCollection));
}

BOOL JumpList::AddDestination(LPCWSTR lpcszCategoryName, LPCWSTR lpcszDestinationPath)
{
	assert(lpcszCategoryName != NULL);
	assert(lpcszDestinationPath != NULL);

	if (lpcszCategoryName == NULL || lpcszDestinationPath == NULL)
	{
		return FALSE;
	}

	IObjectCollection* pColl = GetObjectCollection(lpcszCategoryName);

	if (pColl == NULL)
	{
		return FALSE;
	}

	IShellItem* psi = NULL;
	HRESULT hr = windows7::ShellCreateItemFromParsingName(lpcszDestinationPath, NULL, &IID_IShellItem, (LPVOID*)&psi);

	if (FAILED(hr))
	{
		return FALSE;
	}

	hr = pColl->AddObject(psi);

	if (FAILED(hr))
	{
		psi->Release();
		return FALSE;
	}

	return TRUE;
}

BOOL JumpList::AddDestination(LPCWSTR lpcszCategoryName, IShellItem* pShellItem)
{
	assert(pShellItem != NULL);

	if (pShellItem == NULL)
	{
		return FALSE;
	}

	IObjectCollection* pColl = GetObjectCollection(lpcszCategoryName);

	if (pColl == NULL)
	{
		return FALSE;
	}

	HRESULT hr = pColl->AddObject(pShellItem);

	if (FAILED(hr))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL JumpList::AddDestination(LPCWSTR lpcszCategoryName, IShellLinkW* pShellLink)
{
	assert(pShellLink != NULL);

	if (pShellLink == NULL)
	{
		return FALSE;
	}

	IObjectCollection* pColl = GetObjectCollection(lpcszCategoryName);

	if (pColl == NULL)
	{
		return FALSE;
	}

	HRESULT hr = pColl->AddObject(pShellLink);

	if (FAILED(hr))
	{
		return FALSE;
	}

	return TRUE;
}

IObjectCollection* JumpList::GetObjectCollection(LPCWSTR lpcszCategoryName)
{
	assert(lpcszCategoryName);

	if (lpcszCategoryName == NULL)
	{
		return NULL;
	}

	auto it = m_mapDestinations.find(lpcszCategoryName);
	if(it == m_mapDestinations.end())
	{	   
		IObjectCollection* pColl = NULL;
		HRESULT hr = CoCreateInstance(CLSID_EnumerableObjectCollection, NULL, CLSCTX_INPROC_SERVER, 
			IID_IObjectCollection, (LPVOID*) &pColl);
		if (FAILED(hr))
		{
			return NULL;
		}

		m_mapDestinations.insert(std::make_pair(lpcszCategoryName, pColl));
		return pColl;
	}

	return it->second;
}

BOOL JumpList::CommitList()
{
	if (!InitializeList()) // always returns FALSE if OS < Win7 
	{
		return FALSE;
	}

	for each (auto it in m_mapDestinations)
	{
		std::wstring const& strCategory = it.first;
		IObjectCollection* pColl = it.second;

		// remove items that could be in "removed" list from the collection being added  
		IObjectCollection* pFinalCollection = CheckRemovedItems(pColl);

		assert(pFinalCollection != NULL);
		if (pFinalCollection == NULL)
		{
			continue;
		}

		UINT nDocCount = 0;
		pFinalCollection->GetCount(&nDocCount);

		if (nDocCount == 0)
		{
		}

		HRESULT hr = m_destListPtr->AppendCategory(strCategory.c_str(), pFinalCollection);

		if(FAILED(hr))
		{
		}

		ReleaseObjectArray(pFinalCollection);
	}

	assert(m_tasksPtr != NULL);
	UINT nTaskCount = 0;
	m_tasksPtr->GetCount(&nTaskCount);

	if (nTaskCount > 0)
	{
		HRESULT hr = m_destListPtr->AddUserTasks(m_tasksPtr);
		if (FAILED(hr))
		{
		}
		IObjectCollection* pTaskColl = m_tasksPtr.Detach();
		ReleaseObjectArray(pTaskColl);
	}

	m_mapDestinations.clear();

	BOOL bResult = TRUE;
	HRESULT hr = m_destListPtr->CommitList();

	if (FAILED(hr))
	{
		bResult = FALSE;
	}

	ClearAll(); //set m_bInitialized to FALSE

	// prevent calling CommitList for second time from destructor
	m_bAutoCommit = FALSE;

	return bResult;
}

IObjectCollection* JumpList::CheckRemovedItems(IObjectCollection* pColl)
{
	assert(pColl != NULL);
	assert(m_bInitialized);
	assert(m_destListPtr != NULL);

	if (m_removedItemsPtr == NULL)
	{
		return pColl;
	}

	UINT nRemovedObjCount = 0; 
	UINT nCheckObjCount = 0;

	if (FAILED(m_removedItemsPtr->GetCount(&nRemovedObjCount)) || nRemovedObjCount == 0)
	{
		return pColl;
	}

	if (FAILED(pColl->GetCount(&nCheckObjCount)) || nCheckObjCount == 0)
	{
		return pColl;
	}

	IObjectCollection* pFinalCollection = NULL;

	HRESULT hr = CoCreateInstance(CLSID_EnumerableObjectCollection, NULL, CLSCTX_INPROC_SERVER, 
		IID_IObjectCollection, (LPVOID*) &pFinalCollection);

	if (FAILED(hr))
	{
		return pColl;
	}

	for (UINT i = 0; i < nCheckObjCount; i++)
	{
		IShellItem* pShellItemChk = NULL;
		if (FAILED (pColl->GetAt(i, IID_IShellItem, (LPVOID*) &pShellItemChk)) || pShellItemChk == NULL)
		{
			continue;
		}

		for (UINT j = 0; j < nRemovedObjCount; j++)
		{
			IShellItem* pShellItemRemoved = NULL;
			if (FAILED (m_removedItemsPtr->GetAt(i, IID_IShellItem, (LPVOID*) &pShellItemRemoved)) || pShellItemRemoved == NULL)
			{
				continue;
			}

			int nOrder = 0;
			if (SUCCEEDED(pShellItemChk->Compare(pShellItemRemoved, 0, &nOrder)) && nOrder != 0)
			{
				pFinalCollection->AddObject(pShellItemChk);
			}
			else
			{
				pShellItemChk->Release();
			}
		}
	}

	pColl->Release();
	return pFinalCollection;

}

void JumpList::ClearAllDestinations()
{
	for each (auto it in m_mapDestinations)
	{
		IObjectCollection* pColl = it.second;

		ReleaseObjectArray(pColl);
	}

	m_mapDestinations.clear();
}

void JumpList::ReleaseObjectArray(IObjectArray* pColl, BOOL bReleaseItemsOnly)
{
	if (pColl == NULL)
	{
		return;
	}

	UINT nObjCount = 0;
	HRESULT hr = pColl->GetCount(&nObjCount);
	if (FAILED(hr))
	{
		return;
	}

	for (UINT i = 0; i < nObjCount; i++)
	{
		IUnknown* pUnk = NULL;
		if (SUCCEEDED (pColl->GetAt(i, IID_IUnknown, (LPVOID*) &pUnk)) && pUnk != NULL)
		{
			pUnk->Release();
		}
	}

	if (!bReleaseItemsOnly)
	{
		pColl->Release();
	}
}
