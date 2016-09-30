#ifndef __UIFILE_H__
#define __UIFILE_H__

#pragma once
#include <memory>
namespace DuiLib
{
	struct UILIB_API CUIBuffer
	{
		CUIBuffer();
		CUIBuffer(size_t size);
		CUIBuffer(CUIBuffer&& that);
		void reset(size_t size = 0);
		operator bool() const;
		CUIBuffer& operator=(CUIBuffer&& that);

#pragma warning(push)
#pragma warning(disable:4251)
		std::unique_ptr<unsigned char[]> ptrData;
#pragma warning(pop)
		size_t nSize;
	};

	struct UILIB_API CUIFile
	{
	public:
		static CUIBuffer LoadZip(fs::path const& name);
		static CUIBuffer LoadFile(fs::path const& name);
	};
}

#endif
