/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "navitab/window.h"

namespace navitab {

void PixelBuffer::PaintRegion(int destX, int destY, PixelBuffer &src)
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
        auto dr = Row(destY++);
        auto sr = src.Row(srcYoff++);
        memcpy(dr + destX, sr + srcXoff, srcCols * sizeof(uint32_t));
        --srcRows;
    }
}

void PixelBuffer::BlendRegion(int destX, int destY, PixelBuffer &src)
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
        BlendRow(destY++, destX, src.Row(srcYoff++) + srcXoff, srcCols);
        --srcRows;
    }
}

void PixelBuffer::BlendRow(unsigned row, unsigned offset, uint32_t *s32, int n)
{
    uint32_t *d32 = Row(row) + offset;
    while (n-- > 0) {
        auto spix = *s32 & 0xffffff;
        auto dpix = *d32 & 0xffffff;
        float alpha = (*s32 >> 24) / 255.0;
        uint8_t *d8 = reinterpret_cast<uint8_t *>(d32);
        uint8_t *s8 = reinterpret_cast<uint8_t *>(s32);
        *d8++ = (*d8 * (1.0 - alpha)) + (*s8++ * alpha); // R
        *d8++ = (*d8 * (1.0 - alpha)) + (*s8++ * alpha); // G
        *d8++ = (*d8 * (1.0 - alpha)) + (*s8++ * alpha); // B
        ++s32; ++d32;
    }
}

}
