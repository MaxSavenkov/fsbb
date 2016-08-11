#include "game.h"

#include "game_state_intro.h"
#include "game_state_menu.h"
#include "game_state_combat.h"
#include "game_state_pause.h"
#include "game_state_gameover.h"
#include "game_state_victory.h"

#include "visual_state_intro.h"
#include "visual_state_menu.h"
#include "visual_state_combat.h"
#include "visual_state_pause.h"
#include "visual_state_gameover.h"
#include "visual_state_victory.h"


game::game()
{    
    m_random = std::make_shared<std::mt19937>();

    m_game_fsm.register_state( game_state_id::intro, std::make_shared<game_state_intro>() );
    m_game_fsm.register_state( game_state_id::menu, std::make_shared<game_state_menu>() );
    m_game_fsm.register_state( game_state_id::combat, std::make_shared<game_state_combat>() );
    m_game_fsm.register_state( game_state_id::pause, std::make_shared<game_state_pause>() );
    m_game_fsm.register_state( game_state_id::gameover, std::make_shared<game_state_gameover>() );
    m_game_fsm.register_state( game_state_id::victory, std::make_shared<game_state_victory>() );

    m_visual_fsm.register_state( visual_state_id::intro, std::make_shared<visual_state_intro>() );
    m_visual_fsm.register_state( visual_state_id::menu, std::make_shared<visual_state_menu>() );
    m_visual_fsm.register_state( visual_state_id::combat, std::make_shared<visual_state_combat>() );
    m_visual_fsm.register_state( visual_state_id::pause, std::make_shared<visual_state_pause>() );
    m_visual_fsm.register_state( visual_state_id::gameover, std::make_shared<visual_state_gameover>() );
    m_visual_fsm.register_state( visual_state_id::victory, std::make_shared<visual_state_victory>() );
}

game::~game()
{

}

void game::init()
{
    //m_camera.set_pos(-30.0f, 0.0, 200.0f);
    m_camera.set_pos(10.0f, 0.0, 29.0f);
    m_camera_distance = 10.0f;
    m_camera.set_dist(m_camera_distance);

    m_game_fsm.queue_push_state( game_state_id::intro );
    m_game_fsm.update( *this );
    
    widget_renderer::get().init();
}

void game::update( unsigned int dt, app_context& ctx )
{    
    m_game_fsm.update( *this );
    m_visual_fsm.update( *this );

    game_state_ptr current_game_state = m_game_fsm.get_top_state();
    if ( current_game_state != nullptr )
        current_game_state->update( *this, dt );

    visual_state_ptr current_visual_state = m_visual_fsm.get_current_state();
    if ( current_visual_state != nullptr )
        current_visual_state->update( *this, dt );

//    m_ui.update(dt);

    /*const float d_move = 5 / 2 * dt * 0.008f;
    if (m_key_move_left ) m_camera.add_pos( d_move, 0, 0 );
    if (m_key_move_right ) m_camera.add_pos( -d_move, 0, 0 );
    if (m_key_move_forward) m_camera.add_pos(0, 0, d_move);
    if (m_key_move_backward) m_camera.add_pos(0, 0, -d_move);*/
}

void game::draw(const char *pass, const nya_scene::tags &t)
{
    visual_state_ptr current_visual_state = m_visual_fsm.get_current_state();
    if ( current_visual_state != nullptr )
        current_visual_state->render( *this, pass );
    
    //if (t.has("location"))
    //    m_location->draw(pass);

/*    if (t.has("debug"))
    {
        static nya_render::state s;
        s.depth_test = s.zwrite = false;
        s.set_blend(true, nya_render::blend::src_alpha, nya_render::blend::inv_src_alpha);
        nya_render::set_state(s);
        nya_render::set_modelview_matrix(nya_scene::get_camera().get_view_matrix());
        m_debug_draw.draw();
    }*/
}

//void game::move_camera(const nya_math::vec3& pos)
//{
//    m_camera.set_pos(pos.x, pos.y, pos.z);
//}

void game::on_resize(unsigned int w, unsigned int h)
{
    widget_renderer::get().resize( w, h );

    m_camera.set_aspect(w / (float)h);
    auto & visual_states = m_visual_fsm.get_states();
    for ( auto & vstate : visual_states )
        vstate.state->on_resize( w, h );
    //m_ui.on_resize(w,h);
}

void game::on_suspend()
{

}

void game::on_resume()
{
}

void game::on_mouse_move(int x, int y)
{
    //if (m_ui.on_mouse_move(x, y))
    //    return;

    //if(mouse_init)
    //{
    //    if(m_lbtn_pressed)
    //        m_camera.add_rot((x-m_mouse_x)*0.8f, -(y-m_mouse_y)*0.8f);
    //    if(m_rbtn_pressed)
    //        m_camera.add_pos((x-m_mouse_x)*0.5f, 0 ,-(y-m_mouse_y)*0.5f);
    //}

    //if (m_current_mode)
    //    m_current_mode->on_mouse_move(x, y, *this);

    //m_mouse_x = x, m_mouse_y = y, mouse_init = true;    
}

void game::on_mouse_button(nya_system::mouse_button button, bool pressed)
{
    //if ( m_ui.on_mouse_button(button, pressed, m_mouse_x, m_mouse_y) )
    //    return;
    
/*    if ( pressed )
    {
        if ( !m_lbtn_pressed )
        {
            m_hold_mouse_x = m_mouse_x;
            m_hold_mouse_y = m_mouse_y;
        }
    }
    else
    {
        nya_math::vec2 dist(m_mouse_x - m_hold_mouse_x, m_mouse_y - m_hold_mouse_y);
        if ( dist.length_sq() < 100 )
        {
            if (m_current_mode)
                m_current_mode->on_mouse_button(button, pressed, *this);
        }
    }

    if(button == nya_system::mouse_left)
        m_lbtn_pressed = pressed;
    if (button == nya_system::mouse_right)
        m_rbtn_pressed = pressed;*/
}

void game::on_mouse_scroll(int dx, int dy)
{
/*    if (m_ui.on_mouse_scroll(dx, dy))
        return;

    if (m_current_mode)
        m_current_mode->on_mouse_scroll(dx, dy, *this);

    m_camera_distance -= dy * 0.8f;
    if (m_camera_distance < 5) m_camera_distance = 5;
    if (m_camera_distance > 40) m_camera_distance = 40;
    m_camera.set_dist(m_camera_distance);*/
}

void game::on_keyboard(unsigned int key,bool pressed)
{
/*    if (m_ui.on_keyboard(key, pressed))
        return;

    if (key == 'a') { m_key_move_left = pressed; return; }
    if (key == 'd') { m_key_move_right = pressed; return; }
    if (key == 'w') { m_key_move_forward = pressed; return; }
    if (key == 's') { m_key_move_backward = pressed; return; }
*/
}

/*generator_ptr game::get_random_generator()
{
    return m_random;
}*/
