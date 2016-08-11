#pragma once

#include "ui/ui.h"

#include <memory>

class visual_state_context;

class visual_state : public nya_ui::layer
{
public:
    virtual ~visual_state() {}

    void on_resize( int w, int h ) { resize( w, h );  }

    virtual void on_enter( visual_state_context& ctx ) {}
    virtual void on_exit( visual_state_context& ctx ) {}

    virtual void update( visual_state_context& ctx, unsigned int dt ) {}
    virtual void render( visual_state_context& ctx, const char* pass ) {}

    virtual void on_keyboard( int key ) {}
    virtual void on_mouse_button( int button ) {}
    virtual void on_mouse_move( int x, int y) {}
};

using visual_state_ptr = std::shared_ptr<visual_state>;
