/*
 * Information of portable executable files in memory
 */
#ifndef PE_MEMORY_FILE_INFO_H_INCLUDED
#define PE_MEMORY_FILE_INFO_H_INCLUDED

#include <string>
#include <cstdint>
#include <functional>
#include <boost/optional.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <windows.h>

namespace NYDWE
{

/** \brief Information of pe in memory
 *
 * Information of pe in memory
 */
class CPEMemoryFileInfo
{
public:
	/// Type of secton information
	typedef boost::tuple<uintptr_t, uint32_t> TSectionInfo;
	/// Result of query function
	typedef boost::optional<TSectionInfo> TSectionInfoResult;

public:
	/** \brief Constructs current module information
	 *
	 * Current module information
	 */
	CPEMemoryFileInfo();

	/** \brief Specific module information
	 *
	 * Specific module information
	 *
	 * \param moduleName The name of the module to scan
	 */
	explicit CPEMemoryFileInfo(LPCSTR moduleName);

	/** \brief Specific module information
	 *
	 * Specific module information
	 *
	 * \param moduleName The name of the module to scan
	 */
	explicit CPEMemoryFileInfo(LPCWSTR moduleName);

	/** \brief Specific module information
	 *
	 * Specific module information
	 *
	 * \param module The module to scan
	 */
	explicit CPEMemoryFileInfo(HMODULE module);

	/** \brief Cleanup
	 *
	 * Cleanup
	 */
	~CPEMemoryFileInfo();

	/** \brief Query section table information
	 *
	 * Query section table information
	 *
	 * \return Section table information. Evaluates to false if something goes wrong
	 */
	TSectionInfoResult querySectionTable() const;

	/** \brief Query section information
	 *
	 * Query section information
	 *
	 * \param sectionName Name of section
	 * \return Section info. Evaluates to false if not found
	 */
	TSectionInfoResult querySection(const std::string &sectionName) const;

	/** \brief Enumerate each section
	 *
	 * For each section, call function f
	 *
	 * \param f The function to call
	 */
	void enumerateSection(std::function<bool (PIMAGE_SECTION_HEADER)> f) const;

	/// Get base_ address
	uintptr_t getBase();

	/** \brief swap two objects
	 * 
	 * Swap two objects
	 */
	void swap(CPEMemoryFileInfo &other);

private:
	/// Module base_ address
	uintptr_t base_;
};

/** \brief Swap two CPEMemoryFileInfo objects
 *
 * Swap two CPEMemoryFileInfo objects
 *
 * \param a one object
 * \param b the other
 */
void swap(CPEMemoryFileInfo &a, CPEMemoryFileInfo &b);

} // namespace NYDWE

#endif // PE_MEMORY_FILE_INFO_H_INCLUDED
