#pragma once
// 或者用 include guard

//namespace math {

class World  {
public:
    void update();

private:
    void setPerspective(float fov, float aspect, float zNear, float zFar);
    void setupProjection(int w, int h);
    void drawCube();

};

//} 
