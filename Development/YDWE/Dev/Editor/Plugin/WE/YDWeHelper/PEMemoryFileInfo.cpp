#include <utility>
#include <boost/swap.hpp>
#include <boost/format.hpp>
#include <aero/aero.hpp>

#include "PEMemoryFileInfo.h"

namespace NYDWE
{

CPEMemoryFileInfo::CPEMemoryFileInfo()
	: base_((uintptr_t)GetModuleHandleW(NULL)) {}

CPEMemoryFileInfo::CPEMemoryFileInfo(HMODULE module)
	: base_((uintptr_t)module) {}

CPEMemoryFileInfo::CPEMemoryFileInfo(LPCSTR moduleName)
	: base_((uintptr_t)GetModuleHandleA(moduleName)) {}

CPEMemoryFileInfo::CPEMemoryFileInfo(LPCWSTR moduleName)
	: base_((uintptr_t)GetModuleHandleW(moduleName)) {}

CPEMemoryFileInfo::~CPEMemoryFileInfo() {}

CPEMemoryFileInfo::TSectionInfoResult CPEMemoryFileInfo::querySectionTable() const
{
	TSectionInfoResult result;

	if (reinterpret_cast<PIMAGE_DOS_HEADER>(base_)->e_magic != IMAGE_DOS_SIGNATURE)
		return std::move(result);

	PIMAGE_NT_HEADERS pNtHeader = aero::p_sum<PIMAGE_NT_HEADERS>(
		base_,
		reinterpret_cast<PIMAGE_DOS_HEADER>(base_)->e_lfanew
	);

	if (pNtHeader->Signature != IMAGE_NT_SIGNATURE)
		return std::move(result);

	result = boost::in_place((uintptr_t)(pNtHeader + 1), pNtHeader->FileHeader.NumberOfSections);

	return std::move(result);
}

CPEMemoryFileInfo::TSectionInfoResult CPEMemoryFileInfo::querySection(const std::string &sectionName) const
{
	TSectionInfoResult table = querySectionTable();
	TSectionInfoResult result;

	if (table)
	{
		PIMAGE_SECTION_HEADER pSectionHeader = reinterpret_cast<PIMAGE_SECTION_HEADER>(table->get<0>());
		uint32_t number = table->get<1>();
		PIMAGE_SECTION_HEADER last = pSectionHeader + number;

		for (PIMAGE_SECTION_HEADER itr = pSectionHeader; itr != last; ++itr)
		{
			if (sectionName == reinterpret_cast<const char *>(&(itr->Name[0])))
			{
				result = boost::in_place(
					aero::p_sum<uintptr_t>(
						base_, itr->VirtualAddress
					),
					itr->Misc.VirtualSize
				);

				break;
			}
		}
	}

	return std::move(result);
}

void CPEMemoryFileInfo::enumerateSection(std::function<bool (PIMAGE_SECTION_HEADER)> f) const
{
	TSectionInfoResult table = querySectionTable();
	if (table)
	{
		PIMAGE_SECTION_HEADER pSectionHeader = reinterpret_cast<PIMAGE_SECTION_HEADER>(table->get<0>());
		uint32_t number = table->get<1>();
		PIMAGE_SECTION_HEADER last = pSectionHeader + number;

		for (PIMAGE_SECTION_HEADER itr = pSectionHeader; itr != last; ++itr)
		{
			if (!f(itr))
				break;
		}
	}
}

uintptr_t CPEMemoryFileInfo::getBase()
{
	return base_;
}

void CPEMemoryFileInfo::swap(CPEMemoryFileInfo &other)
{
	if (this != &other)
	{
		boost::swap(base_, other.base_);
	}
}

void swap(CPEMemoryFileInfo &a, CPEMemoryFileInfo &b)
{
	a.swap(b);
}

} // namespace NYDWE
