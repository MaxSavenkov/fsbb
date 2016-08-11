#pragma once

#include "game_state.h"

class game_state_intro : public game_state
{
public:
    virtual void on_enter( game_state_context& ctx ) override
    {
        ctx.change_visual_state( visual_state_id::intro );
    }

    virtual void on_exit( game_state_context& ctx ) override {}

    virtual void update( game_state_context& ctx, unsigned int dt ) override {}
};
