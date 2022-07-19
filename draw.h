#pragma once
#include "tgaimage.h"
//#include"tgaimage.cpp"
#include <iostream>
#include"eigen3/Eigen/Dense"
using namespace std;
using namespace Eigen;

#include<math.h>


//bool InTriangle(vec2 v0,vec2 v1,vec2 v2,int x,int y);
Vector3d barycentric(Vector3d v0,Vector3d v1, Vector3d v2,int  x,int  y)
{
    //采用矩阵的解法，只需要解出alpha和beta就行
    //p是用来存放重心座标的
    Vector3d Vectorx=Vector3d(v1.x()-v0.x(),v2.x()-v0.x(),v0.x()-x);
    Vector3d Vectory=Vector3d(v1.y()-v0.y(),v2.y()-v0.y(),v0.y()-y);
    Vector3d Vectorf=Vectorx.cross(Vectory);
    Vector3d p=Vector3d(1-Vectorf.x()/Vectorf.z()-Vectorf.y()/Vectorf.z(),Vectorf.x()/Vectorf.z(),Vectorf.y()/Vectorf.z());
    if(abs(p.z())>=0)
    return p;
    return Vector3d(-1,1,1);

}

void triangle3D(Vector3d v0,Vector3d v1,Vector3d v2,TGAImage& image,vector<Vector2d> uvs,double intensity,double* zBuffer,int width,TGAImage diffuse)
{
    //構造包圍盒
    int  left=min(v0.x(),min(v1.x(),v2.x()));
    int  right=max(v0.x(),max(v1.x(),v2.x()));
    int  top=max(v0.y(),max(v1.y(),v2.y()));
    int  bottom=min(v0.y(),min(v1.y(),v2.y()));
    //试着采用重心坐标对uv进行插值
    double u,v;
   
    //先給zbuffer賦值
    //事實上已經先全部賦值爲0了
    //這裏是z的值越大越靠前
    //先判斷是否在平面上
    //如果在平面上再判斷是否在三角形上
    // for(int i=0;i<3;i++)
    // {
    //     cout<<"v "<<i<<" r "<<color[i].r<<" g "<<color[i].g<<" b "<<color[i].b<<endl;
        
    // }
    
    Vector3d p;
        for(int i=left;i<=right;i++)
        {
            for(int j=bottom;j<=top;j++)
            {
                p=barycentric(v0,v1,v2,i,j);
                if(p.x()<1&&p.x()>=0&&p.y()<=1&&p.y()>=0&&p.z()<=1&&p.z()>=0)//如果在三角形内
                {
                    double temp=p.x()*v0.z()+p.y()*v1.z()+p.z()*v2.z();//通过重心座标计算z的值
                    if(temp>zBuffer[i+j*width])//如果满足zBuffer 条件
                    {
                        u=0.0;
                        v=0.0;
                        for(int k=0;k<3;k++)
                        {
                            u+=uvs[k][0]*p[k];
                            v+=uvs[k][1]*p[k];//Eigen的向量是可以这么调用的
                        }    
                        TGAColor finalColor(diffuse.get((int)u*diffuse.get_width(),(int)v*diffuse.get_height()));
                        finalColor=finalColor*intensity;
                        //cout<<temp<<endl;
                        zBuffer[i+j*width]=temp;//更换zbuffer中记录的值
                        //填充颜色
                        image.set(i,j,finalColor);
                        // cout<<" r-draw "<<int(((color[0].r*p.x()+color[1].r*p.y()+color[2].r*p.z())))
                        // <<" g-draw "<<int(((color[0].g*p.x()+color[1].g*p.y()+color[2].g*p.z())))
                        // <<" b-draw "<<int(((color[0].b*p.x()+color[1].b*p.y()+color[2].b*p.z())))<<endl;
                    }
                }
            }
        }
     
}







