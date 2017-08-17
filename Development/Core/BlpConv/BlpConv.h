#pragma once

#include <vector>

namespace IMAGE {
	typedef std::vector<unsigned char> BUFFER;

	namespace BLP {
		bool Write(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, int Width, int Height, int Quality);
		bool Read(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, int* Width = NULL, int* Height = NULL);
	}
	namespace BMP {
		bool Write(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, int Width, int Height, int Quality);
		bool Read(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, int* Width = NULL, int* Height = NULL);
	}
	namespace JPEG {
		bool Write(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, int Width, int Height, int Quality);
		bool Read(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, unsigned int Width, unsigned int Height);
	}
}
