#pragma once

#include "visual_state.h"

#include "ui/label.h"

#include "ui_widgets.h"

class visual_state_intro : public visual_state
{
public:
    visual_state_intro()
    {
        m_intro_text.set_text( "Welcome to FSBB demo." );
        m_intro_text.set_pos( 0, 0 );
        m_intro_text.set_size( 200, 40 );

        add_widget( m_intro_text );
    }

      // visual_state
    virtual void update( visual_state_context& ctx, unsigned int dt ) override
    {
        process( dt );
    }

    virtual void render( visual_state_context& ctx, const char* pass ) override
    {
        draw();
    }

      // layer
    virtual void process_events( nya_ui::event& e )
    {
    
    }

private:
    
    label m_intro_text;
};
