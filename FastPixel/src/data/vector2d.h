#ifndef VECTOR2D_H
#define VECTOR2D_H

class Vector2D
{
public:
    Vector2D();
    Vector2D(double xpos, double ypos);

    double Length() const;
    void Normalize();

    inline void SetX(double x);
    inline void SetY(double y);
    inline double X() const;
    inline double Y() const;

private:
    double v_[2];
};

inline void Vector2D::SetX(double x)
{
    v_[0] = x;
}

inline void Vector2D::SetY(double y)
{
    v_[1] = y;
}

inline double Vector2D::X() const
{
    return v_[0];
}

inline double Vector2D::Y() const
{
    return v_[1];
}

#endif // VECTOR2D_H
