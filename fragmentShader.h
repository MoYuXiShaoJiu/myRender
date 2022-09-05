#pragma once
#include <eigen3/Eigen/Dense>
#include"tgaimage.h"
#include<iostream>
using namespace std;
using namespace Eigen;
// 继续分离大计
//这里是利用上一个reastrizer分离出来的数据和纹理进行着色
//取得最终的颜色并进行着色
class fragmentShader
{
    private:
    pair<double,double>* tex_buffer_pointer=nullptr;
    double * zbuffer_pointer=nullptr;
    public:
    fragmentShader(pair<double,double>* tex_buffer,double* zbuffer)//将指针传递
    {
        this->tex_buffer_pointer=tex_buffer;
        this->zbuffer_pointer=zbuffer;
        if(tex_buffer_pointer==nullptr)
        {
            cout<<"get tex buffer failed"<<endl;
        }
        if(zbuffer_pointer==nullptr)
        {
            cout<<"get zbuffer failed"<<endl;
        }
        
    }

    //处理函数
    void using_fragment_shader(TGAImage &image,TGAImage& diffuseMap,double* intensityA)
    {
        if(this->tex_buffer_pointer==nullptr)
        {
            cout<<"please initinalize firstly"<<endl;
        }
        if(zbuffer_pointer==nullptr)
        {
            cout<<"please initinalize firstly"<<endl;
        }
        //如果对应的数据都没有问题
        TGAColor finalcolor;
        int image_heigh,image_width;
        image_heigh=image.get_height();
        image_width=image.get_width();
        //实际上这里的时间复杂度几乎上升了一倍，
        //但是因为opengl是用在gpu上的，
        //所以在gpu和内存之间的通信会占用更多的时间
        //相对来说这样把大量的数据一次性处理掉会比较好
        //这里有一个问题是zbuffer的前后关系没有用到
        for(int i=0;i<image_width;i++)
        {
            for(int j=0;j<image_heigh;j++)
            {
                finalcolor=diffuseMap.get(tex_buffer_pointer[i+j*image_width].first*diffuseMap.get_width(),tex_buffer_pointer[i+j*image_width].second*diffuseMap.get_height());
                finalcolor=finalcolor*intensityA[i+j*image_width];
                image.set(i,j,finalcolor);
            }
        }


    }




};
