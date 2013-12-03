#ifndef __UTILS_H__
#define __UTILS_H__

#pragma once

#include <map>
#include <string>
#include <vector>

#if defined(_MSC_VER)
#	define foreach(VAL, COL) \
	__pragma(warning(push)) \
	__pragma(warning(disable: 4239)) \
	for each (VAL in COL) \
	__pragma(warning(pop))
#else
#	define foreach(VAL, COL) for (VAL : COL)
#endif

namespace DuiLib
{
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class STRINGorID
	{
	public:
		STRINGorID(const wchar_t* lpString) : m_lpstr(lpString)
		{ }
		STRINGorID(UINT nID) : m_lpstr(MAKEINTRESOURCEW(nID))
		{ }
		const wchar_t* m_lpstr;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CPoint : public tagPOINT
	{
	public:
		CPoint();
		CPoint(const POINT& src);
		CPoint(int x, int y);
		CPoint(LPARAM lParam);
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CSize : public tagSIZE
	{
	public:
		CSize();
		CSize(const SIZE& src);
		CSize(const RECT rc);
		CSize(int cx, int cy);
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiRect : public tagRECT
	{
	public:
		CDuiRect();
		CDuiRect(const RECT& src);
		CDuiRect(int iLeft, int iTop, int iRight, int iBottom);

		int GetWidth() const;
		int GetHeight() const;
		void Empty();
		bool IsNull() const;
		void Join(const RECT& rc);
		void ResetOffset();
		void Normalize();
		void Offset(int cx, int cy);
		void Inflate(int cx, int cy);
		void Deflate(int cx, int cy);
		void Union(CDuiRect& rc);
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CStdPtrArray
	{
	public:
		CStdPtrArray(int iPreallocSize = 0);
		CStdPtrArray(const CStdPtrArray& src);
		~CStdPtrArray();

		void Empty();
		void Resize(int iSize);
		bool IsEmpty() const;
		int Find(LPVOID iIndex) const;
		bool Add(LPVOID pData);
		bool SetAt(int iIndex, LPVOID pData);
		bool InsertAt(int iIndex, LPVOID pData);
		bool Remove(int iIndex);
		int GetSize() const;
		LPVOID* GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPVOID* m_ppVoid;
		int m_nCount;
		int m_nAllocated;
	};

#pragma warning(push)
#pragma warning(disable:4251)
	template <class T> class UILIB_API CStdStringPtrMap : public std::map<std::wstring, T*>
	{
	public:	
		T* Find(const wchar_t* key) const
		{
			auto it = find(key);
			if (it == end()) 
				return nullptr;
			else
				return it->second;
		}

		bool Insert(const wchar_t* key, T* pData)
		{
			return insert(std::make_pair(key, pData)).second;
		}

		bool Remove(const wchar_t* key)
		{
			auto it = find(key);
			if (it == end()) return false;
			erase(it);
			return true;
		}
	};
#pragma warning(pop)

	typedef std::vector<std::pair<std::wstring, std::wstring>> CAttributesList;

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CWaitCursor
	{
	public:
		CWaitCursor();
		~CWaitCursor();

	protected:
		HCURSOR m_hOrigCursor;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class CVariant : public VARIANT
	{
	public:
		CVariant() 
		{ 
			VariantInit(this); 
		}
		CVariant(int i)
		{
			VariantInit(this);
			this->vt = VT_I4;
			this->intVal = i;
		}
		CVariant(float f)
		{
			VariantInit(this);
			this->vt = VT_R4;
			this->fltVal = f;
		}
		CVariant(LPOLESTR s)
		{
			VariantInit(this);
			this->vt = VT_BSTR;
			this->bstrVal = s;
		}
		CVariant(IDispatch *disp)
		{
			VariantInit(this);
			this->vt = VT_DISPATCH;
			this->pdispVal = disp;
		}

		~CVariant() 
		{ 
			VariantClear(this); 
		}
	};

}// namespace DuiLib

#endif // __UTILS_H__