/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "bglfilestream.h"
#include <fmt/core.h>
#include <cassert>

namespace navbuilder {

BglFileStream::BglFileStream(std::filesystem::path f)
:   fname(f)
{
    fp.open(fname, std::ios::binary);
}

bool BglFileStream::read(void* buffer, size_t size)
{
    fp.read(reinterpret_cast<char*>(buffer), size);
    return fp.good();
}

template<>
bool BglFileStream::read<>(uint8_t& v)
{
    return read(&v, 1);
}

template<>
bool BglFileStream::read<>(uint16_t& v)
{
    return read(&v, 2);
}

template<>
bool BglFileStream::read<>(uint32_t& v)
{
    return read(&v, 4);
}

template<>
bool BglFileStream::read<>(float& v)
{
    return read(&v, 4);
}

bool BglFileStream::readHeader(RecordHeader& hdr)
{
    read(&hdr.id, 2);
    return read(&hdr.size, 4);
}

bool BglFileStream::readLongitude(double& lonx)
{
    int32_t x = 0;
    read(&x, 4);
    lonx = ((double)x * (360.0f / (3 * 0x10000000))) - 180.0f;
    return fp.good();
}

bool BglFileStream::readLatitude(double& laty)
{
    int32_t y = 0;
    read(&y, 4);
    laty = 90.0f - (((double)y) * (180.0f / (2 * 0x10000000)));
    return fp.good();
}

bool BglFileStream::readElevation(float& elev)
{
    int32_t e = 0;
    read(&e, 4);
    elev = (float)e / 1000.0f;
    return fp.good();
}

bool BglFileStream::readCodedId(std::string& id, bool isRegion)
{
    uint32_t v = 0;
    read(&v, 4);
    if (!isRegion) v >>= 5;
    std::string s;

    while (v) {
        int c = v % 38;
        v = (v - c) / 38;
        if (c >= 12) {
            s.push_back('A' + c - 12);
        }
        else if (c > 1) {
            s.push_back('0' + c - 2);
        }
        else if (c == 0) {
            s.push_back(' ');
        }
        else {
            assert(0);
        }
    }
    std::reverse(s.begin(), s.end());
    id = s;
    return fp.good();
}

}