/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "texbuffer.h"
#include "navitab/window.h"

namespace navitab {

void TextureBuffer::CopyRegionsFrom(const FrameBuffer *src, const std::vector<ImageRegion> &regions)
{
    // TODO - have a stab at doing something a bit more optimal.
    // Simple implementation just copies the entire image

    std::copy(src->Data().begin(), src->Data().end(), data.begin());
}

} // namespace navitab
