/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <memory>
#include "../app.h"

namespace navitab {

class AppServices;

class AboutApp : public App
{
public:
    AboutApp(std::shared_ptr<AppServices> core);

    void ToolClick(ClickableTool t) override;
    void MouseEvent(int x, int y, bool l) override;

protected:
    void Assemble() override;
    void Demolish() override;

private:
};


} // namespace navitab
