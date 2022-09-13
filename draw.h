#pragma once
#include "tgaimage.h"
//#include"tgaimage.cpp"
#include <iostream>
#include"eigen3/Eigen/Dense"
using namespace std;
using namespace Eigen;

#include<math.h>


//bool InTriangle(vec2 v0,vec2 v1,vec2 v2,int x,int y);
Vector3d barycentric(Vector3d v0,Vector3d v1, Vector3d v2,double  x,double  y)
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

void triangle3D(Vector3d v0,Vector3d v1,Vector3d v2,Vector3d world_coordinate[3],TGAImage& image,vector<Vector2d> &uvs,double intensity,double* zBuffer,int width,TGAImage diffuse)
{
       
    //構造包圍盒
    int  left=max(0.0,min(v0.x(),min(v1.x(),v2.x())));
    int  right=min(799.9,max(v0.x(),max(v1.x(),v2.x())));
    int  top=min(799.9,max(v0.y(),max(v1.y(),v2.y())));
    int  bottom=max(0.0,min(v0.y(),min(v1.y(),v2.y())));
    //试着采用重心坐标对uv进行插值
    double u,v;
    Vector3d p;
    Vector3d wd_bc;//world coordinates barycentric coordinates
        for(int i=left;i<=right;i++)
        {
            for(int j=bottom;j<=top;j++)
            {
                //这里给triangle函数输入的是screen coordinates，由于空间变换的原因，这里已经不是原来的那个三角形的重心坐标了
                p=barycentric(v0,v1,v2,i,j);//p用来存储重心坐标
                wd_bc=Vector3d(p.x()/world_coordinate[0][2],p.y()/world_coordinate[1][2],p.z()/world_coordinate[2].z());
                wd_bc=wd_bc/(wd_bc.x()+wd_bc.y()+wd_bc.z());//求得原来的重心坐标
                if(p.x()<1&&p.x()>=0&&p.y()<=1&&p.y()>=0&&p.z()<=1&&p.z()>=0)//如果在三角形内
                {
                    double tempz=wd_bc.x()*world_coordinate[0].z()+wd_bc.y()*world_coordinate[1].z()+world_coordinate[2].z(); //通过重心座标计算z的值

                    //tempz=-tempz;
                    if(tempz>zBuffer[i+j*width])//如果满足zBuffer 条件
                    {
                        //cout<<tempz<<endl;
                        u=0.0;
                        v=0.0;
                        for(int k=0;k<3;k++)
                        {
                            u+=uvs[k][0]*p[k];
                            v+=uvs[k][1]*p[k];//Eigen的向量是可以这么调用的
                        }    
                       // cout<<" u"<<u*1024<<" v "<<v*1024<<endl;
                        TGAColor finalColor(diffuse.get(u*diffuse.get_width(),v*diffuse.get_height()));
                        finalColor=finalColor*intensity;
                        zBuffer[i+j*width]=tempz;//更换zbuffer中记录的值
                        //填充颜色
                        image.set(i,j,finalColor);
                      
                    }
                }
            }
        }
     
}

Vector4d PointToHomogeneous(Vector3d temp)//将点转换为齐次坐标
{
    return Vector4d(temp.x(),temp.y(),temp.z(),1.0);
}

Vector4d vectorToHomogeneous(Vector3d temp)//将向量转换为齐次坐标
{
    return Vector4d(temp.x(),temp.y(),temp.z(),0.0);
}

Vector3d HomogeneousTo(Vector4d temp)//将输入的齐次坐标转换为正常的坐标

{
    return Vector3d(temp[0]/temp[3],temp[1]/temp[3],temp[2]/temp[3]);
}

Matrix4d perspective(double zNear,double zFar)//透视投影，
{
    Matrix4d temp;
    temp<< zNear,0,0,0,
           0,zNear,0,0,
           0,0,zNear+zFar,-(zNear*zFar),
           0,0,1,0;
    return temp;
}

Matrix4d Mvp(int nx,int ny)//Viewport矩阵，将单位图片投影到要求的图片坐标
{
    Matrix4d temp,oth;
    temp<< nx/2,0,0,(nx-1)/2,
           0,ny/2,0,(ny-1)/2,
           0,0,1,0,
           0,0,0,1;
  
    return oth;
}

Matrix4d cameraM(Vector3d eye_position,Vector3d gaze,Vector3d t)//其中eye_position是决定相机位置的，gase是决定朝向的，t是分野的
{
    Vector3d w=(-gaze).normalized();
    Vector3d u=t.cross(w).normalized();
    Vector3d v=w.cross(u);
    Matrix4d temp,result;
    temp<<u.x(),v.x(),w.x(),eye_position.x(),
          u.y(),v.y(),w.y(),eye_position.y(),
          u.z(),v.z(),w.z(),eye_position.z(),
          0,0,0,1;
    result=temp.inverse();
    return temp;
    

}

Vector3d color_To_Vec3(TGAColor& color)
{
    Vector3d temp;
    temp[0]=color.r;
    temp[1]=color.g;
    temp[2]=color.b;
    return temp;
    
}
