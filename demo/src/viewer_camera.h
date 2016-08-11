//https://code.google.com/p/nya-engine/

#pragma once

#include "math/vector.h"

class viewer_camera
{
public:
    void add_rot(float dx,float dy);
    void add_pos(float dx,float dy,float dz);

    void set_pos(float x,float y,float z);

    void set_aspect(float aspect);
    void set_height(float value);
    void set_dist(float value);

private:
    void update();

private:
    float m_rot_x = 0.0f;
    float m_rot_y = 50.0f;
    float m_height = 1.0;
    float m_dist = 5.0;

    nya_math::vec3 m_pos;
};
