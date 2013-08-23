#pragma once

#include <ydwe/graph/config.h>
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
	_GRAPH_API HBITMAP to_bitmap(image_t& img, HDC hdc);
}}
