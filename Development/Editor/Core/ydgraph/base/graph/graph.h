#pragma once

#include <base/graph/config.h>
#include <Windows.h>

#pragma warning(push)
#pragma warning(disable:4996)
#include <boost/gil/gil_all.hpp>
#pragma warning(pop)

namespace ydwe { namespace graph {
	typedef boost::gil::argb8_image_t image_t;
	
	_GRAPH_API image_t from_png (char* ptr, size_t size);
	_GRAPH_API image_t from_jpg (char* ptr, size_t size);
	_GRAPH_API image_t from_bmp (char* ptr, size_t size);
	_GRAPH_API std::pair<HBITMAP, bool> to_bitmap(image_t& img, HDC hdc, uint32_t mask);
}}
