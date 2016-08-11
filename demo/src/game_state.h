#pragma once

#include <memory>

#include "game_state_context.h"

class game_state
{
public:
    virtual ~game_state() {}

    virtual void on_enter( game_state_context& ctx ) {}
    virtual void on_exit( game_state_context& ctx ) {}

    virtual void update( game_state_context& ctx, unsigned int dt ) {}
};

using game_state_ptr = std::shared_ptr<game_state>;