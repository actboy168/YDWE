#pragma once

#include "Buffer.h"

namespace IMAGE
{
	bool Blp2Bmp(const BUFFER& SourceBuffer, BUFFER& TargetBuffer);
	bool Bmp2Blp(const BUFFER& SourceBuffer, BUFFER& TargetBuffer);
	bool BlpBlend(const BUFFER& BlpA, const BUFFER& BlpB, BUFFER& BlpC);
}
