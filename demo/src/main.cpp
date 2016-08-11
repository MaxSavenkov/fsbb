#include "system/app.h"
#include "system/system.h"
#include "scene/mesh.h"
#include "scene/postprocess.h"
#include "resources/file_resources_provider.h"
#include "log/log.h"
#include "app_context.h"
#include "game.h"
#include <sstream>
#include <deque>
#include <vector>

class app: public nya_system::app, private nya_scene::postprocess, public app_context
{
private:
    std::unique_ptr<game> m_game;

private:
    bool on_splash() override
    {
        auto frp = new nya_resources::file_resources_provider;
        std::string path_dev = std::string(nya_system::get_app_path()) + "../../res/";
        std::string path_release = std::string(nya_system::get_app_path()) + "./res/";
        frp->set_folder(path_dev.c_str());
        if ( !frp->access("font.tga") )
            frp->set_folder(path_release.c_str());

        nya_resources::set_resources_provider(frp);
        nya_scene::mesh::set_resources_prefix("objects/");
        nya_scene::animation::set_resources_prefix("objects/");
        nya_scene::shader::set_resources_prefix("shaders/");
        nya_scene::material::set_resources_prefix("materials/");
        nya_scene::texture::set_resources_prefix("textures_pc/");

        nya_scene::mesh::register_load_function(nya_scene::mesh::load_nms);

        return true;
    }

    void on_init() override
    {
        nya_render::set_clear_depth(1.0f);
        nya_render::depth_test::enable(nya_render::depth_test::less);

        load("postprocess.txt");
        //set_condition("grunge", true);

        m_game.reset(new game());
        m_game->on_resize(get_width(), get_height());
        m_game->init();
    }

    virtual void draw_scene(const char *pass,const nya_scene::tags &t) override
    {
        //m_game->draw(pass, t);
    }

    void on_frame(unsigned int dt)
    {
        m_game->update(dt, *this);
        nya_render::set_clear_color(0.2f,0.4f,0.5f,0.0f);
        nya_render::clear( true, true );
        m_game->draw("", {});
        //draw(dt);
        //nya_render::state s;s.depth_test=false;nya_render::set_state(s);
        //m_game->draw("ui");

        static unsigned int fps_counter=0,fps_update_timer=0;
        ++fps_counter;
        fps_update_timer+=dt;
        if(fps_update_timer>1000)
        {
            std::ostringstream os;
            os<<"FSBB demo "<<fps_counter<<" fps";
            std::string str=os.str();
            set_title(str.c_str());

            fps_update_timer%=1000;
            fps_counter=0;
        }
    }

    void on_mouse_move(int x,int y)
    {
        m_game->on_mouse_move(x,y);
    }

    void on_mouse_button(nya_system::mouse_button button,bool pressed)
    {
        m_game->on_mouse_button(button, pressed);
    }

    void on_mouse_scroll(int dx, int dy)
    {
        m_game->on_mouse_scroll(dx, dy);
    }

    void on_keyboard(unsigned int key,bool pressed)
    {
        m_game->on_keyboard(key, pressed);
    }

    void on_resize(unsigned int w,unsigned int h)
    {
        if(!w || !h)
            return;

        if ( m_game )
            m_game->on_resize(w,h);

        resize(w, h);
    }

    void on_free() { m_game.reset(0); unload(); }

    // app_context
private:
    nya_system::app & nya_app()
    {
        return *this;
    }
};

#if WIN32
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>
    void RedirectIOToConsole()
    {
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
        setvbuf(stdout, NULL, _IONBF, 0);
        setvbuf(stderr, NULL, _IONBF, 0);
        std::ios::sync_with_stdio();
    }
    
    int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
    int main(int argc, char **argv)
#endif
{
#ifdef WIN32
    RedirectIOToConsole();
#endif

    static ::app app;
    app.set_title("Loading...");
    app.start_windowed(100, 100, 1024, 640, 0);
}
