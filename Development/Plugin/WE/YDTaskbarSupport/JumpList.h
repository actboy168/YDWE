#pragma once
#include <Windows.h>
#include <atlcomcli.h>
#include <Shobjidl.h>
#include <string>
#include <map>
#include <functional>

class JumpList
{
public:
	JumpList(BOOL bAutoCommit = TRUE);
	~JumpList();
	BOOL InitializeList();
	void SetAppID(LPCWSTR strAppID);

	UINT GetMaxSlots() const;
	ICustomDestinationList* GetDestinationList()
	{
		return m_destListPtr;
	}

	IObjectArray* GetRemovedItems()
	{
		return m_removedItemsPtr;
	}
	void AbortList();
	BOOL AddDestination(LPCWSTR lpcszCategoryName, LPCWSTR strDestinationPath);
	BOOL AddDestination(LPCWSTR strCategoryName, IShellItem* pShellItem);
	BOOL AddDestination(LPCWSTR strCategoryName, IShellLinkW* pShellLink);
	BOOL AddKnownCategory(KNOWNDESTCATEGORY category);
	BOOL AddTask(LPCWSTR strTitle, std::function<void(CComPtr<IShellLinkW>&)> callback);
	BOOL AddTask(LPCWSTR strTargetExecutablePath, LPCWSTR strCommandLineArgs, LPCWSTR strTitle, LPCWSTR strIconLocation, int iIconIndex);
	BOOL AddTask(IShellLinkW* pShellLink);
	BOOL AddTasks(IObjectArray* pObjectCollection);
	BOOL AddTaskSeparator();
	void ClearAllDestinations();
	void ClearAll();
	BOOL CommitList();

protected:
	IObjectCollection* CheckRemovedItems(IObjectCollection* pColl);
	void ReleaseObjectArray(IObjectArray* pColl, BOOL bReleaseItemsOnly = FALSE);
	IObjectCollection* GetObjectCollection(LPCWSTR lpcszCategoryName);

protected:
	std::wstring m_strAppID;
	BOOL		 m_bInitialized;
	UINT		 m_nMaxSlots;
	BOOL		 m_bAutoCommit; // Automatically call commit in destructor

	::ATL::CComPtr<IObjectArray>           m_removedItemsPtr;
	::ATL::CComPtr<ICustomDestinationList> m_destListPtr;
	::ATL::CComPtr<IObjectCollection>      m_tasksPtr;

	std::map<std::wstring, IObjectCollection*>  m_mapDestinations;
};
