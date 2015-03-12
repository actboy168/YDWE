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

/** \brief Memory patch
 * 
 * Patches memory
 *
 * \param first Source start to copy
 * \param last Source last
 * \param result Destination address
 * \return pointer past the copied data in destination area, nullptr if fail
 */
void *MemoryPatch(void *first, void *last, void *result);

/** \brief Test memory block equality
 *
 * Test whether the two memory block is equal
 * \param first First of sequence 1
 * \param last Last of sequence 1
 * \param first2 First of sequence 2
 * \return Whether the two is equal
 */
bool MemoryEqual(void *first, void *last, void *first2);

/** \brief Memory patch and check
 * 
 * Patches memory and then check if it is correct.
 *
 * \param first Source start to copy
 * \param last Source last
 * \param result Destination address
 * \return Whether patch is success
 */
bool MemoryPatchAndVerify(void *first, void *last, void *result);

/// Template function for patch fixed size data
template <typename Type>
inline void *MemoryPatchData(void *location, Type data)
{
	return MemoryPatch(&data, &data + 1, location);
}

/// Template function for patch and check fixed size data
template <typename Type>
inline bool MemoryPatchAndVerifyData(void *location, Type data)
{
	return MemoryPatchAndVerify(&data, &data + 1, location);
}

/** \brief Make patch
 *
 * Make patch
 *
 * \param patchPoint Patch start address
 * \param patch Code used to patch
 * \param patchLength Length of the patch in bytes
 * \return Whether the patch secceeds.
 */
void *MemoryPatch(void *patchPoint, uint8_t *patch, uintptr_t patchLength);

/** \brief Test memory block equality
 *
 * Test whether the two memory block is equal
 * \param first First of sequence 1
 * \param last Last of sequence 1
 * \param length Length of the sequence
 * \return Whether the two is equal
 */
bool MemoryEqual(void *first, void *last, uintptr_t length);

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
	CMemoryPatch(void *patchPoint, void *patchData, size_t patchDataLength);
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