#include "asset.h"

namespace boop
{
	CompressionMode parse_compression(const char *f)
	{
        if (strcmp(f, "LZ4") == 0)
            return CompressionMode::LZ4;
        else
            return CompressionMode::None;
	}
}