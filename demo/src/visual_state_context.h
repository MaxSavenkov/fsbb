#pragma once

#include "common.h"

class visual_state_context
{
public:
    virtual void change_state( visual_state_id new_state ) = 0;
};
