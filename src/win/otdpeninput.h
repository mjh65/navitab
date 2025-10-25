/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <memory>

namespace navitab {

class OtdPenInput
{
public:
    static std::shared_ptr<OtdPenInput> Factory();

    virtual ~OtdPenInput() = default;

    // returns true if pen is in proximity. x/y/pressure range from 0.0 .. 1.0
    virtual bool GetPenState(float &x, float &y, float &pressure) = 0;
};

}
