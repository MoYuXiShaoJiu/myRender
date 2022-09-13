#include<iostream>
#include"eigen3/Eigen/Dense"
#include"tgaimage.h"
#include"draw.h"
#include"model.h"
#include<limits.h>
#include"shader.h"
#include<malloc.h>
#include"vertexShader.h"
#include"rasterizre.h"
#include"fragmentShader.h"
using namespace std;
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green=TGAColor(0,255,0,255);
const int Myheight = 800;
const int Mywidth = 800;

Vector3d world2screen(Vector3d v) 
{
    return Vector3d(int((v.x()+1.)*Mywidth/2.+.5), int((v.y()+1.)*Myheight/2.+.5), v.z());
}


model *myModel = NULL;

int  main()
{
    /*
    关于使用的中坐标系说明:
    这里使用的是屏幕平面为xy平面，x正方向向右，y正方向向上，
    z轴垂直屏幕平面，无限远处为0，z轴正方向垂直平面向里，即-x.corss(y)
    */

    bool MSAA_bool=false;
    Vector3d light_dir(0.0,0.0,1.0);
    myModel = new model("obj/african_head.obj");//打开文件
    double zBuffer[Mywidth][Myheight] ;//= {0};//其实这里应该是无限远的值
    //double MSAA_Buffer[Mywidth][Myheight][4];
    double* zBufferP=zBuffer[0];//zbuffer的指针
    //不能使用这么大的数组，会溢出
    double* MSAA_Buffer=(double*)malloc(sizeof(double)*Mywidth*Myheight*4);//这里应该用来存储是否在三角形内，
    //如果在三角形内，就将该点的值置为1
    //如果不在三角形内，就将该点置为0
    bool* pixel_in=(bool*)malloc(sizeof(bool)*Myheight*Mywidth);//这个用来存储所有的子采样点是否在三角形内
    pair<double,double> * texture_buffer=(pair<double,double>*)malloc(sizeof(pair<double,double>)*Myheight*Mywidth);//纹理坐标
    double * intensityArray=(double *)malloc(sizeof(double)*Myheight*Mywidth);
    double * shadowBuffer=(double *)malloc(sizeof(double)*Myheight*Mywidth);//shadow map
    if(shadowBuffer==nullptr)
    {
        cout<<"申请空间失败"<<endl;
    }
    for(int i=0;i<Mywidth;i++)
    {
        for(int j=0;j<Myheight;j++)
        {
            zBuffer[i][j]=-numeric_limits<double>::max();
            intensityArray[i*Mywidth+j]=0.0;
            for(int k=0;k<4;k++)
            {
                MSAA_Buffer[i*Myheight*4+j*4+k]=-numeric_limits<double>::max();//msaa深度的初始化
            }
        }
    }
    TGAImage image(Mywidth, Myheight, TGAImage::RGB);//创建图
    vector<int> temp;//用来暂时存储点的次序
    Vector3d tempVertex[3];//用来暂时存储三个点
    Vector3d screen_coordinate[3];
    Vector3d world_coordinate[3];
    Vector3d original[3];
    vector<Vector2d> uvs;
    vector<Vector3d> normals;
    //Vector3d camera(0,0,3);//相机位置
    Matrix4d projectiveM,CameraMatrix,MViewP,M;
    Vector3d eye_position(0,2,3);//相机位置
    Vector3d gaze(1,1,-1);//朝向
    Vector3d t(0,1,1);//分野
    CameraMatrix=cameraM(eye_position,gaze,t);
    projectiveM=perspective(1,-1);//投影矩阵
    MViewP=Mvp(800,800);//Viewport
    M=MViewP*projectiveM;

    //从这里开始以下都用shder来操作
    vertexShader vertex_shader(Mywidth,Myheight,eye_position,gaze,t);
    //vertexShader shadow_vertex_shader(Mywidth,Myheight,)
    //rasterizer my_rasterizer(image);
    //shader myShader(original,lesson_M,MSAA_bool);
    shader myShader(original,vertex_shader,MSAA_bool,image);
    //fragmentShader my_fragment_shader(myShader.get_tex_buffer_pointer(),zBufferP);
    

    //第一遍shadow
    // for(int i=0;i<myModel->faceNumber();i++)
    // {
    //     temp=myModel->getTriangle(i);
    //     for(int j=0;j<3;j++)
    //     {
    //         world_coordinate[j]=myModel->getVertex(temp[j]);
    //         screen_coordinate[j]=vertex_shader.TransVertex(world_coordinate[j]);

    //     }
    // }

    for(int i=0;i<myModel->faceNumber();i++)//对每个三角形渲染
    {
        
        uvs.clear();
        normals.clear();
        temp=myModel->getTriangle(i);//取得序列
        uvs=myModel->getUV(i);
        normals=myModel->get_Normals(i);
        for(int j=0;j<3;j++)
        {
           //tempVertex[j]=world2screen(myModel->getVertex(temp[j])) ;
            world_coordinate[j]=myModel->getVertex(temp[j]);//world coordinates
            //screen_coordinate[j]= HomogeneousTo(lesson_M*PointToHomogeneous(original[j]));//screen coordinates
            screen_coordinate[j]=vertex_shader.TransVertex(world_coordinate[j]);
        }
        myShader.setVertex(world_coordinate);
        //myShader.rasterize(image,myModel->diffuseMap,uvs,normals,light_dir,zBufferP);
        myShader.rasterize(image,myModel,vertex_shader,uvs,light_dir,zBufferP);
        // Vector3d normal=((world_coordinate[0]-world_coordinate[1]).cross(world_coordinate[0]-world_coordinate[2])).normalized();
        // double intensity=normal.dot(light_dir);
        // if(intensity>0)
        // {
        //     //my_rasterizer.do_rasterize(image,world_coordinate,screen_coordinate,zBufferP,uvs,texture_buffer,intensityArray,intensity);

        //     if(MSAA_bool)
        //     {
        //         myShader.rasterize(image,myModel->diffuseMap,uvs,intensity,zBufferP,MSAA_Buffer,pixel_in);

        //         myShader.doMSAA(image,pixel_in);
        //     }
        //     else
        //     {
        //          myShader.rasterize(image,myModel->diffuseMap,uvs,intensity,zBufferP);
        //     }
        // }
    }
    //my_fragment_shader.using_fragment_shader(image,myModel->diffuseMap,intensityArray);
    

    image.flip_vertically();  
    image.write_tga_file("vertex.tga");
   // image.write_tga_file("lesson3.tga");
    delete myModel;
    return 0;

}