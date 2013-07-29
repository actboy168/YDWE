#include <typeinfo>
#include <boost/any.hpp>
#include <boost/chrono.hpp>
#include <windows.h>

#include "YDJAPI.h"
#include "BuiltinJAPI.h"

namespace NYDWE
{

struct STimePoint
{
	boost::any anyTimePoint;
};

static STimePoint *__cdecl JAPITimeGet(INT method)
{
	STimePoint *result = new STimePoint;

	switch (method)
	{
	case 1:
		result->anyTimePoint = boost::chrono::system_clock::now();
		break;
	case 2:
		result->anyTimePoint = boost::chrono::high_resolution_clock::now();
		break;
	default:
		result->anyTimePoint = boost::chrono::process_cpu_clock::now();
		break;
	}

	return result;
}

static INT __cdecl JAPITimeDiff(STimePoint *pBegin, STimePoint *pEnd)
{
	FLOAT result = 0;

	if (pBegin && pEnd && pBegin->anyTimePoint.type() == pEnd->anyTimePoint.type())
	{
		const std::type_info &typeInfo = pBegin->anyTimePoint.type();
		if (typeInfo == typeid(boost::chrono::system_clock::time_point))
		{
			boost::chrono::duration<float> dur = boost::any_cast<boost::chrono::system_clock::time_point>(pEnd->anyTimePoint)
				- boost::any_cast<boost::chrono::system_clock::time_point>(pBegin->anyTimePoint);
			result = dur.count();
		}
		else if (typeInfo == typeid(boost::chrono::high_resolution_clock::time_point))
		{
			boost::chrono::duration<float> dur = boost::any_cast<boost::chrono::high_resolution_clock::time_point>(pEnd->anyTimePoint)
				- boost::any_cast<boost::chrono::high_resolution_clock::time_point>(pBegin->anyTimePoint);
			result = dur.count();
		}
		else if (typeInfo == typeid(boost::chrono::process_cpu_clock::time_point))
		{
			boost::chrono::duration<float> dur = boost::any_cast<boost::chrono::process_cpu_clock::time_point>(pEnd->anyTimePoint)
				- boost::any_cast<boost::chrono::process_cpu_clock::time_point>(pBegin->anyTimePoint);
			result = dur.count();
		}
	}

	return (INT &)result;
}

static VOID __cdecl JAPITimeDelete(STimePoint *pTimePoint)
{
	delete pTimePoint;
}

void RegisterBuiltinJAPI()
{
	RegisterJAPINative(JAPITimeGet, "JAPITimeGet", "(I)I");
	RegisterJAPINative(JAPITimeDelete, "JAPITimeDelete", "(I)V");
	RegisterJAPINative(JAPITimeDiff, "JAPITimeDiff", "(II)R");
}

} // namespace NYDWE
