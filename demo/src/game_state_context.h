#pragma once

#include "common.h"

class game_state_context
{
public:
    virtual void push_state( game_state_id new_state ) = 0;
    virtual void remove_state( game_state_id state ) = 0;

    virtual void change_visual_state( visual_state_id new_state ) = 0;
};
