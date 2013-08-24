#include <ydwe/graph/graph.h>
#include <ydwe/util/buffer.h>
#pragma warning(push)
#pragma warning(disable:4996)
#include <boost/gil/extension/io/bmp_all.hpp>
#include <boost/gil/extension/io/jpeg_all.hpp>
#include <boost/gil/extension/io/png_all.hpp>
#pragma warning(pop)

namespace ydwe { namespace graph {
	namespace detail
	{
		template <typename Image, typename FormatTag>
		inline Image from_memory(char* ptr, size_t size, const FormatTag& settings)
		{
			Image img;
			boost::gil::read_image(ydwe::util::buffer_stream(ptr, ptr + size)
				, img
				, settings
				);
	
			return std::move(img);
		}

		std::pair<HBITMAP, bool> to_bitmap(const image_t::view_t& v, HDC hdc, uint32_t mask)
		{
			BITMAPINFO bmi = {0};
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = v.width();
			bmi.bmiHeader.biHeight = v.height();
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = v.size();

			void* bits;
			HBITMAP bmp = ::CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0x0);
			bool alpha_channel = false;

			if (bmp) 
			{
				boost::gil::bgra8_view_t dst = boost::gil::interleaved_view(
					v.width(), 
					v.height(),
					(boost::gil::bgra8_ptr_t)bits,
					v.width() * sizeof(boost::gil::bgra8_pixel_t));

				boost::gil::copy_pixels(boost::gil::flipped_up_down_view(boost::gil::color_converted_view<boost::gil::bgra8_pixel_t>(v)), dst);

				boost::gil::bgra8_pixel_t zero_pixel(0, 0, 0, 0);
				boost::gil::bgra8_pixel_t mask_pixel(
					  ((unsigned char*)&mask)[0]
					, ((unsigned char*)&mask)[1]
					, ((unsigned char*)&mask)[2]
					, ((unsigned char*)&mask)[3]);

				for (std::ptrdiff_t y = 0; y < dst.height(); ++y)
				{
					std::for_each(dst.row_begin(y), dst.row_end(y), [&](boost::gil::bgra8_pixel_t& p)
					{
						if (p == mask_pixel) 
						{
							p = zero_pixel;
							alpha_channel = true;
						} 

						if (p[3] < 255)
						{
							p[0] = p[0] * p[3] / 255;
							p[1] = p[1] * p[3] / 255;
							p[2] = p[2] * p[3] / 255;
							alpha_channel = true;
						}
					});
				}
			}

			return std::make_pair(bmp, alpha_channel);
		}
	}
	
	image_t from_png(char* ptr, size_t size)
	{
		return std::move(detail::from_memory<image_t>(ptr, size, boost::gil::png_tag()));
	}

	image_t from_jpg(char* ptr, size_t size)
	{
		boost::gil::rgb8_image_t img = detail::from_memory<boost::gil::rgb8_image_t>(ptr, size, boost::gil::jpeg_tag());
		image_t argb_img(img.width(), img.height());
		boost::gil::copy_and_convert_pixels(boost::gil::const_view(img), boost::gil::view(argb_img));
		return std::move(argb_img);
	}
	
	image_t from_bmp(char* ptr, size_t size)
	{
		return std::move(detail::from_memory<image_t>(ptr, size, boost::gil::bmp_tag()));
	}

	std::pair<HBITMAP, bool> to_bitmap(image_t& img, HDC hdc, uint32_t mask)
	{
		return detail::to_bitmap(boost::gil::view(img), hdc, mask);
	}
}}
