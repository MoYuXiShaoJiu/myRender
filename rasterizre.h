#pragma once
#include<eigen3/Eigen/Dense>
#include"draw.h"
using namespace Eigen;
//用来求覆盖的rasterizer
//实际上这里求的是覆盖以及插值
//重心插值的结果可以存起来，这样就可以给后面的阶段去使用，而不用把两个阶段混合起来
class rasterizer
{
    private:
    int left,right,top,bottom,Image_width;
    double u,v;
    Vector3d p,wd_bc;
    public:
    //需要实现的功能是包围盒和插值
    //再加上深度检查
    //也要把纹理坐标之类的搞定
    //对于包围盒中的每一个像素，计算它的深度值，计算它的纹理坐标，存储到堆中
    rasterizer(TGAImage & image)
    {
        this->Image_width=image.get_width();
    }

    void do_rasterize(TGAImage& image,Vector3d world_coordinates[],Vector3d screen_coordinates[],double *zBuffer,vector<Vector2d>&uvs,pair<double,double>* texture_buffer)
    {
        //求包围盒
        left=max(0.0,min(screen_coordinates[0].x(),min(screen_coordinates[1].x(),screen_coordinates[2].x())));
        right=min((double)image.get_width(),max(screen_coordinates[0].x(),max(screen_coordinates[1].x(),screen_coordinates[2].x())));
        top=min((double)image.get_height(),max(screen_coordinates[0].y(),max(screen_coordinates[1].y(),screen_coordinates[2].y())));
        bottom=max(0.0,min(screen_coordinates[0].y(),min(screen_coordinates[1].y(),screen_coordinates[2].y())));
        //在包围盒中，对每一个像素
        for(int i=left;i<=right;i++)
        {
            for(int j=bottom;j<=top;j++)
            {
                p=barycentric(screen_coordinates[0],screen_coordinates[1],screen_coordinates[2],i,j);
                wd_bc=Vector3d(p.x()/world_coordinates[0][2],p.y()/world_coordinates[1][2],p.z()/world_coordinates[2].z());
                wd_bc=wd_bc/(wd_bc.x()+wd_bc.y()+wd_bc.z());//求得原来的重心坐标
                if(p.x()<1&&p.x()>=0&&p.y()<=1&&p.y()>=0&&p.z()<=1&&p.z()>=0)//如果在三角形内
                {
                    double tempz=wd_bc.x()*world_coordinates[0].z()+wd_bc.y()*world_coordinates[1].z()+wd_bc.z()*world_coordinates[2].z();
                    if(tempz>zBuffer[i+ j*image.get_width()])//如果满足深度条件
                    {
                        u=0.0;
                        v=0.0;
                        for(int k=0;k<3;k++)
                        {
                            u+=uvs[k][0]*p[k];
                            v+=uvs[k][1]*p[k];
                        }
                        
                        zBuffer[i+j*Image_width]=tempz;
                        texture_buffer[i+j*Image_width].first=u;//将结果记录进纹理buffer
                        texture_buffer[i+j*Image_width].second=v;
                      
                    }
                }

            }
        }
    }




};