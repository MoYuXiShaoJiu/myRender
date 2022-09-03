#pragma once
#include<eigen3/Eigen/Dense>
#include"draw.h"
using namespace Eigen;
//处理点的shader
class vertexShader
{
    private:
    Matrix4d viewPort,projective,finalM,cameraMatrix;
    int image_width,image_heigh;
    public:
    //构造vertexShader
    vertexShader(int width,int heigh,Vector3d cameraPosition,Vector3d eye_position,Vector3d gaze,Vector3d t)
    {
        this->image_heigh=heigh;
        this->image_width=width;
        projective<<1,0,0,0,
                0,1,0,0,
                0,0,1,0,
                0,0,-1.0/3.0,1;
        viewPort<<width/8*3,0,0,width/2,
                     0,heigh/8*3,0,heigh/2,
                     0,0,255/2,255/2,
                     0,0,0,1;
        cameraMatrix=cameraM(eye_position,gaze,t);
        finalM=viewPort*projective;
    }
    
    //变换
    Vector3d TransVertex(Vector3d inputVertex)
    {
        return HomogeneousTo(this->finalM*PointToHomogeneous(inputVertex));
    }
    //取得变换矩阵
    Matrix4d getTransMatrix()
    {
        return finalM;
    }



};
