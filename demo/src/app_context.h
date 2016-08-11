#pragma once

#include "system/app.h"

class app_context
{
public:
    virtual nya_system::app & nya_app() = 0;
};
