/*
 *  Navitab - Navigation Tablet for VR flight simulation
 *  Copyright (c) 2024 Michael Hasling
 *  Significantly derived from Avitab
 *  Copyright (c) 2018-2024 Folke Will
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <string>
#include <iostream>
#include <sstream>
#include <lunasvg.h>


using namespace lunasvg;

bool parseArgs(int argc, char** argv,
               std::string& filename, std::uint32_t& width, std::uint32_t& height, std::uint32_t& bgColor);

int main(int argc, char** argv)
{
    // TODO - to be useful as a tool we need proper options, including output file
    
    std::string filename = "sample.svg";
    std::uint32_t width = 48, height = 48;
    std::uint32_t bgColor = 0x00000000; // transparent
    if (argc > 1) {
        if(!parseArgs(argc, argv, filename, width, height, bgColor)) exit(1);
    }

    auto document = Document::loadFromFile(filename);
    if(!document) exit(2);

    auto bitmap = document->renderToBitmap(width, height, bgColor);
    if(!bitmap.valid()) exit(3);
    bitmap.convertToRGBA();

    auto bw = bitmap.width();
    auto bh = bitmap.height();
    auto stride = bitmap.stride();
    auto rowData = bitmap.data();

    for(std::uint32_t y = 0; y < bh; y++) {
        auto data = rowData;
        for(std::uint32_t x = 0; x < bw; x++) {
            
            // TODO - write each sample here
            
            data += 4;
        }
        rowData += stride;
    }

    // do something useful with the bitmap here.

    return 0;
}

bool parseArgs(int argc, char** argv,
               std::string& filename, std::uint32_t& width, std::uint32_t& height, std::uint32_t& bgColor)
{
    if(argc > 1) filename.assign(argv[1]);
    if(argc > 2) {
        std::stringstream ss;

        ss << argv[2];
        ss >> width;

        if(ss.fail() || ss.get() != 'x')
            return false;

        ss >> height;
    }

    if(argc > 3) {
        std::stringstream ss;

        ss << std::hex << argv[3];
        ss >> std::hex >> bgColor;
    }

    return argc > 1;
}
