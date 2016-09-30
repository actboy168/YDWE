#include <algorithm>
#include <windows.h>
#include "MemoryPatch.h"

namespace NYDWE
{

void *MemoryPatch(void *first, void *last, void *result)
{
	uint8_t *pFirst = reinterpret_cast<uint8_t *>(first),
		*pLast = reinterpret_cast<uint8_t *>(last),
		*pOut = reinterpret_cast<uint8_t *>(result);

	DWORD oldProtect;
	uint8_t *pResult = nullptr;
	if (VirtualProtect(pOut, std::distance(pFirst, pLast), PAGE_WRITECOPY, &oldProtect))
	{
		__try
		{
			pResult = std::copy(pFirst, pLast, pOut);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {}

		VirtualProtect(pOut, std::distance(pFirst, pLast), oldProtect, &oldProtect);
	}

	return pResult;
}

bool MemoryEqual(void *first, void *last, void *first2)
{
	uint8_t *pFirst = reinterpret_cast<uint8_t *>(first),
		*pLast = reinterpret_cast<uint8_t *>(last),
		*pFirst2 = reinterpret_cast<uint8_t *>(first2);

	bool result = false;
	__try
	{
		result = std::equal(pFirst, pLast, pFirst2);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {}

	return result;
}

bool MemoryPatchAndVerify(void *first, void *last, void *result)
{
	return MemoryPatch(first, last, result) == (void *)((uintptr_t)last - (uintptr_t)first + (uintptr_t)result)
		&& MemoryEqual(first, last, result);
}

void *MemoryPatch(void *patchPoint, uint8_t *patch, uint32_t patchLength)
{
	return MemoryPatch(
		&patch[0],
		&patch[patchLength],
		patchPoint
	);
}

bool MemoryEqual(void *first, void *last, uintptr_t length)
{
	return MemoryEqual(first, last, (char *)first + length); 
}

bool MemoryPatchAndVerify(void *patchPoint, uint8_t *patch, uint32_t patchLength)
{
	return MemoryPatchAndVerify(
		&patch[0],
		&patch[patchLength],
		patchPoint
	);
}

CMemoryPatch::CMemoryPatch(void *patchPoint, void *patchDataBegin, void *patchDataEnd)
	: patchPoint_(patchPoint), patchData_((const char *)patchDataBegin, (const char *)patchDataEnd)
	, restoreData_((const char *)patchPoint, std::distance((const char *)patchDataBegin, (const char *)patchDataEnd))
	, isPatched_(false) {}

CMemoryPatch::CMemoryPatch(void *patchPoint, void *patchData, size_t patchDataLength)
	: patchPoint_(patchPoint), patchData_((const char *)patchData, patchDataLength)
	, restoreData_((const char *)patchPoint, patchDataLength)
	, isPatched_(false) {}

CMemoryPatch::~CMemoryPatch()
{
	restore();
}

bool CMemoryPatch::patch()
{
	bool result = false;

	if (!isPatched_)
	{
		isPatched_ = result = MemoryPatchAndVerify(patchPoint_, (uint8_t *)patchData_.data(), patchData_.size());
	}

	return result;
}

bool CMemoryPatch::restore()
{
	bool result = false;

	if (isPatched_)
	{
		result = MemoryPatchAndVerify(patchPoint_, (uint8_t *)restoreData_.data(), restoreData_.size());
		isPatched_ = !result;
	}

	return result;
}

bool CMemoryPatch::isPatched()
{
	return isPatched_;
}

} // namespace NYDWE
