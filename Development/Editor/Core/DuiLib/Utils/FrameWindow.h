#pragma once
#include "stdafx.h"

namespace DuiLib
{
	class UILIB_API CFrameWindow : public CWindowWnd
	{
	public:
		CFrameWindow();

	protected:
		const wchar_t* GetWindowClassName() const;
		UINT GetClassStyle() const;
		void OnFinalMessage(HWND /*hWnd*/);

		virtual void InitWindow() = 0;
		virtual fs::path GetSkinZip() const = 0;
		virtual const wchar_t* GetSkinXml() const = 0;

	private:
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:
		CPaintManagerUI m_pm;
	};
}
