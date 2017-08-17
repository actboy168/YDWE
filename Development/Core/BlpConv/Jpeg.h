#pragma once

#include "Buffer.h"

namespace IMAGE { namespace JPEG {

	bool Write(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, int Width, int Height, int Quality);
	bool Read(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, unsigned int Width, unsigned int Height);
}}
