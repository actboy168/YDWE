#pragma once

#if defined(YDWE_GRAPH_EXPORTS)
#	define _GRAPH_API __declspec(dllexport)
#else
#	define _GRAPH_API __declspec(dllimport)
#endif
