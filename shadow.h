//就暂且不管rasterizer的复用了，直接写shadow map
#pragma once
#include"eigen3/Eigen/Dense"
using namespace Eigen;

class shadow
{
    private:
    int width,height;//用来存储图像的长宽
    Vector3d lightSource;//用来存储光源的朝向
    double * shadowBuffer=nullptr;//用来存储malloc的大小,但是还是放在外面比较好

    public:
    shadow(int width,int height,Vector3d light_souce,double * shadowBuffer)//构造函数
    {
        this->width=width;
        this->height=height;
        this->lightSource=light_souce;
        this->shadowBuffer=shadowBuffer;
    }

    void do_shadow_mapping()//进行shadow mapping，产出shadow map
    {
        
    }
    


};