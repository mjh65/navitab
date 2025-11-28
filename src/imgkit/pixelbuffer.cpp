/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "navitab/window.h"

namespace navitab {

void PixelBuffer::PaintRectangle(const PixelBuffer& src, int destX, int destY)
{
    int srcCols = src.width;
    int srcRows = src.height;
    int srcXoff = 0;
    int srcYoff = 0;
    if (destX < 0) {
        srcXoff = 0 - destX;
        srcCols += destX;
        destX = 0;
    }
    if ((destX + srcCols) >= (int)width) {
        srcCols = width - destX;
    }
    if (srcCols <= 0) return;
    if (destY < 0) {
        srcYoff = 0 - destY;
        srcRows += destY;
        destY = 0;
    }
    if ((destY + srcRows) >= (int)height) {
        srcRows = height - destY;
    }
    if (srcRows <= 0) return;
    while (srcRows > 0) {
        auto dr = GetRowPtr(destY++);
        auto sr = src.GetRowPtrRO(srcYoff++);
        memcpy(dr + destX, sr + srcXoff, srcCols * sizeof(uint32_t));
        --srcRows;
    }
}

void PixelBuffer::BlendRectangle(const PixelBuffer& src, int destX, int destY)
{
    int srcCols = src.width;
    int srcRows = src.height;
    int srcXoff = 0;
    int srcYoff = 0;
    if (destX < 0) {
        srcXoff = 0 - destX;
        srcCols += destX;
        destX = 0;
    }
    if ((destX + srcCols) >= (int)width) {
        srcCols = width - destX;
    }
    if (srcCols <= 0) return;
    if (destY < 0) {
        srcYoff = 0 - destY;
        srcRows += destY;
        destY = 0;
    }
    if ((destY + srcRows) >= (int)height) {
        srcRows = height - destY;
    }
    if (srcRows <= 0) return;
    while (srcRows > 0) {
        BlendRow(destY++, destX, src.GetRowPtrRO(srcYoff++) + srcXoff, srcCols);
        --srcRows;
    }
}

void PixelBuffer::BlendRow(unsigned row, unsigned offset, const uint32_t *s32, int n)
{
    uint32_t *d32 = GetRowPtr(row) + offset;
    while (n-- > 0) {
        auto spix = *s32 & 0xffffff;
        auto dpix = *d32 & 0xffffff;
        float alpha = (*s32 >> 24) / 255.0;
        uint8_t *d8 = reinterpret_cast<uint8_t *>(d32);
        const uint8_t *s8 = reinterpret_cast<const uint8_t *>(s32);
        *d8++ = (*d8 * (1.0 - alpha)) + (*s8++ * alpha); // R
        *d8++ = (*d8 * (1.0 - alpha)) + (*s8++ * alpha); // G
        *d8++ = (*d8 * (1.0 - alpha)) + (*s8++ * alpha); // B
        ++s32; ++d32;
    }
}

void PixelBuffer::Copy(const PixelBuffer& src)
{
    if ((width == src.width) && (height == src.height)) {
        // images are the same size, is the layout the same?
        if ((span == src.span) && (span == width)) {
            // simple memcpy
            memcpy(pBuffer, src.pBuffer, (width * height * sizeof(uint32_t)));
            return;
        }
        // a row by row copy will do
        const uint32_t* sr = src.GetRowPtrRO(0);
        uint32_t* dr = this->GetRowPtr(0);
        for (auto r = 0; r < height; ++r) {
            memcpy(dr, sr, (width * sizeof(uint32_t)));
        }
        return;
    }
    // variant not yet implemented
    assert(0);
}

void PixelBuffer::Copy(const PixelBuffer& src, const ImageRegion& region)
{
    // ignore empty regions
    if (region.Empty()) return;

    // if the region matches the src PixelBuffer then copy the whole source
    if ((region.left == 0) && (region.top == 0) && (region.right == src.width) && (region.bottom == src.height)) {
        this->Copy(src);
        return;
    }

    // create a (transient) PixelBuffer object which matches the region
    ImageRegion sr(src.Region(), region);
    if (sr.Empty()) return;
    uint32_t *rbptr = const_cast<uint32_t *>(src.GetPixelPtrRO(sr.left, sr.top));
    PixelBuffer s2((unsigned)sr.Width(), (unsigned)sr.Height(), src.span, rbptr);
    this->Copy(s2);
    return;

    // variant not yet implemented
    assert(0);
}

void PixelBuffer::Copy(const PixelBuffer& src, const std::vector<ImageRegion>& regions)
{
    for (auto r : regions) {
        this->Copy(src, r);
    }
}


}
