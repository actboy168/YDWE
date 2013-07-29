/*
 * Search pattern in memory
 */
#ifndef MEMORY_PATTERN_SEARCH_H_INCLUDED
#define MEMORY_PATTERN_SEARCH_H_INCLUDED

#include <cstdint>

namespace NYDWE
{

/** \brief Search pattens in memory
 *
 * Search patterns in memory. The pattern must be array of uint16_t, whose high 8-bit is the mask and 
 * low 8-bit is the value
 * 
 * \param first1 First of iterator of contents
 * \param last Last of iterator of contents
 * \param first2 First of iterator of pattern
 * \param last2 Last of iterator of pattern
 * \return pointer if found, otherwise NULL
 */
void *MemoryPatternSearch(void *first1, void *last1, const uint16_t *first2, const uint16_t *last2);

/** \brief Search for pattern
 *
 * Search for specific pattern
 *
 * \param searchStart Search start address
 * \param searchLength Length of the search
 * \param patchPattern Pattern of the code to search
 * \param patternLength Length of the pattern in bytes
 * \return Searched address. NULL if not found
 */
void *MemoryPatternSearch(void *searchStart, uintptr_t searchLength, uint16_t *patchPattern, uintptr_t patternLength);

} // namespace NYDWE

#endif // MEMORY_PATTERN_SEARCH_H_INCLUDED