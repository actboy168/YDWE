/*
 * Patch memory
 */
#ifndef MEMORY_PATCH_H_INCLUDED
#define MEMORY_PATCH_H_INCLUDED

#include <cstdint>
#include <cstdlib>
#include <string>
#include <base/util/noncopyable.h>

namespace NYDWE
{

/** \brief Make patch
 *
 * Make patch and verify it
 *
 * \param patchPoint Patch start address
 * \param patch Code used to patch
 * \param patchLength Length of the patch in bytes
 * \return Whether the patch secceeds.
 */
bool MemoryPatchAndVerify(void *patchPoint, uint8_t *patch, uintptr_t patchLength);

class CMemoryPatch : base::noncopyable
{
public:
	CMemoryPatch(void *patchPoint, void *patchDataBegin, void *patchDataEnd);
	CMemoryPatch(void *patchPoint, const void *patchData, size_t patchDataLength);
	~CMemoryPatch();

public:
	bool patch();
	bool restore();
	bool isPatched();

private:
	void *patchPoint_;
	std::string patchData_;
	std::string restoreData_;
	bool isPatched_;
};

}

#endif // MEMORY_PATCH_H_INCLUDED