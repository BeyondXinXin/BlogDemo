#include "vector2d.h"

#include <iostream>

Vector2D::Vector2D()
{

    v_[0] = 0;
    v_[1] = 0;
}

Vector2D::Vector2D(double xpos, double ypos)
  : v_ { xpos, ypos }
{
}

double Vector2D::Length() const
{
    double len = double(v_[0]) * double(v_[0]) + double(v_[1]) * double(v_[1]);
    return std::sqrt(len);
}

void Vector2D::Normalize()
{
    // 如果长度很小，则需要一些额外的精度。
    double len = double(v_[0]) * double(v_[0]) + double(v_[1]) * double(v_[1]);
    len = std::sqrt(len);

    v_[0] = v_[0] / len;
    v_[1] = v_[1] / len;
}
