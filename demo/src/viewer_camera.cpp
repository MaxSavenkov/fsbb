//https://code.google.com/p/nya-engine/

#include "viewer_camera.h"
#include "scene/camera.h"
#include "math/scalar.h"

void viewer_camera::add_rot(float dx,float dy)
{
    m_rot_x+=dx;
    m_rot_y+=dy;

//wrap
    const float max_angle=360.0f;

    if(m_rot_x>max_angle)
        m_rot_x-=max_angle;

    if(m_rot_x< -max_angle)
        m_rot_x+=max_angle;

    if(m_rot_y>max_angle)
        m_rot_y-=max_angle;

    if(m_rot_y< -max_angle)
        m_rot_y+=max_angle;

//clamp
    if(m_rot_y< 0)
        m_rot_y=0;
    if(m_rot_y> 80)
        m_rot_y=80;

    update();
}

void viewer_camera::add_pos(float dx,float dy,float dz)
{

    nya_math::quat rot(0.0,-m_rot_x*3.14f/180.0f,0.0f);
    m_pos += rot.rotate(-nya_math::vec3(dx, dy, dz));

    update();
}


void viewer_camera::set_pos(float x,float y,float z)
{
    m_pos.x = x;
    m_pos.y = y;
    m_pos.z = z;
    update();
}

void viewer_camera::set_height(float value)
{
    m_height=value;
    update();
}

void viewer_camera::set_dist(float value)
{
    m_dist=value;
    update();
}

void viewer_camera::set_aspect(float aspect)
{
    nya_scene::get_camera().set_proj(55.0f,aspect,0.1f,2000.0f);
    update();
}

void viewer_camera::update()
{
    nya_scene::get_camera().set_rot(m_rot_x,m_rot_y,0.0);

    nya_math::quat rot(-m_rot_y*3.14f/180.0f,-m_rot_x*3.14f/180.0f,0.0f);
    auto pos=m_pos+rot.rotate(nya_math::vec3(0.0, 0.0, m_dist));

    nya_scene::get_camera().set_pos(pos.x,pos.y+1.0f,pos.z);
}
