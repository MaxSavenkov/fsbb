#pragma once

#include "game_state.h"

class game_state_gameover : public game_state
{
public:
    virtual void on_enter( game_state_context& ctx ) override {}
    virtual void on_exit( game_state_context& ctx ) override {}

    virtual void update( game_state_context& ctx, unsigned int dt ) override {}
};
