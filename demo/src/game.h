#pragma once

#include <memory>
#include <random>

#include "system/system.h"
#include "system/app.h"
#include "scene/camera.h"
#include "scene/postprocess.h"
#include "render/debug_draw.h"

#include "fsbb_prefabs.hpp"

#include "app_context.h"
#include "viewer_camera.h"

#include "game_state_context.h"
#include "game_state.h"

#include "visual_state_context.h"
#include "visual_state.h"

class game : public game_state_context, public visual_state_context
{
public:
    game();
    ~game();

    void init();
    void update( unsigned int dt, app_context &ctx );
    void draw(const char *pass,const nya_scene::tags &t);

    void on_resize(unsigned int w, unsigned int h);
    void on_suspend();
    void on_resume();

    void on_mouse_move(int x, int y);
    void on_mouse_button(nya_system::mouse_button button, bool pressed);
    void on_mouse_scroll(int dx, int dy);
    void on_keyboard(unsigned int key,bool pressed);

    // game_state_context
private:
    void push_state( game_state_id new_state ) override
    {
        m_game_fsm.queue_push_state( new_state );
    }

    void remove_state( game_state_id state ) override
    {
        m_game_fsm.queue_remove_state_and_all_above( state );
    }

    void change_visual_state( visual_state_id new_state ) override
    {
        change_state( new_state );
    }

        // visual_state_context
private:
    void change_state( visual_state_id new_state ) override
    {
        m_visual_fsm.queue_change_state( new_state );
    }

private:
    viewer_camera m_camera;
    float m_camera_distance;
    
private:
    std::shared_ptr<std::mt19937> m_random;

private:
    nya_render::debug_draw m_debug_draw;

    int m_hold_mouse_x = 0, m_hold_mouse_y = 0;
    bool m_lbtn_pressed = false, m_rbtn_pressed = false;
    int m_mouse_x = 0, m_mouse_y = 0;
    bool mouse_init = false;

    bool m_key_move_left = false;
    bool m_key_move_right = false;
    bool m_key_move_forward = false;
    bool m_key_move_backward = false;

    fsbb::fsm_stacked_queued_enter_exit<game_state_id, game_state_ptr, game_state_context&> m_game_fsm;
    fsbb::fsm_single_queued_enter_exit<visual_state_id, visual_state_ptr, visual_state_context&> m_visual_fsm;

    //game_ui m_ui;
};
