#include<iostream>
#include"eigen3/Eigen/Dense"
#include"tgaimage.h"
#include"draw.h"
#include"model.h"
#include<limits.h>

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


//    TGAImage image(width,height,TGAImage::RGB);
//    //相邻且z相同的两个三角形
//     Vector3d v0(400,100,100);
//     Vector3d v1(400,500,100);
//     Vector3d v2(100,250,200);
//     Vector3d v3(600,300,100);

//    int zBuffer[width][height] = {0};//其实这里应该是无限远的值
//    int* zBufferP=&zBuffer[0][0];//zbuffer的指针
//    triangle3D(v0,v1,v2,image,red,zBufferP,width);
//    triangle3D(v1,v0,v3,image,white,zBufferP,width);
//    //image.flip_vertically();
//    image.write_tga_file("output.tga");
//    return 0;


    Vector3d light_dir(0,0,1);
    myModel = new model("obj/african_head.obj");//打开文件
    double zBuffer[Mywidth][Myheight] ;//= {0};//其实这里应该是无限远的值
    for(int i=0;i<Myheight;i++)
    {
        for(int j=0;j<Mywidth;j++)
        {
            zBuffer[i][j]=-numeric_limits<double>::max();
        }
    }
    double* zBufferP=zBuffer[0];//zbuffer的指针
    TGAImage image(Mywidth, Myheight, TGAImage::RGB);//创建图
    vector<int> temp;//用来暂时存储点的次序
    Vector3d tempVertex[3];//用来暂时存储三个点
    Vector3d perVertex[3];
    Vector3d original[3];
    vector<Vector2d> uvs;

    Vector3d camera(0,0,3);//相机位置
    Matrix4d projectiveM,CameraMatrix,MViewP,M;
    Vector3d eye_position(0,0,3);//相机位置
    Vector3d gaze(1,1,-1);//朝向
    Vector3d t(0,1,0);//分野
    CameraMatrix=cameraM(eye_position,gaze,t);
    projectiveM=perspective(1,-1);//投影矩阵
    MViewP=Mvp(800,800);//Viewport
    M=MViewP*projectiveM*CameraMatrix;


    Matrix4d lesson_viewport,lesson_porjective,lesson_M;
    lesson_porjective<<1,0,0,0,
                       0,1,0,0,
                       0,0,1,0,
                       0,0,-1/3,1;
    lesson_viewport<<Mywidth/8*3,0,0,Mywidth/2,
                     0,Myheight/8*3,0,Myheight/2,
                     0,0,255/2,255/2,
                     0,0,0,1;
    lesson_M=lesson_viewport*lesson_porjective;
    //vector<TGAColor> vertexColor;//三角形三个顶点的颜色
    for(int i=0;i<myModel->faceNumber();i++)//对每个三角形渲染
    {
        
        uvs.clear();
        temp=myModel->getTriangle(i);//取得序列
        //vertexColor=myModel->getcolor(i);//所以问题出现在这里
        uvs=myModel->getUV(i);
        //cout<<uvs[0][0]<<" "<<uvs[0][1]<<" "<<"\n"<<endl;
        for(int j=0;j<3;j++)
        {
            tempVertex[j]=world2screen(myModel->getVertex(temp[j])) ;
            original[j]=myModel->getVertex(temp[j]);
            perVertex[j]= HomogeneousTo(lesson_viewport*lesson_porjective*CameraMatrix*PointToHomogeneous(original[j]));
            //cout<<"color "<<i<<" r "<<(vertexColor[i].r) <<" g "<<(vertexColor[i].g) <<" b "<<(vertexColor[i].b) <<endl;//输出的值确实是满足要求的
        }
     
        Vector3d normal=((original[0]-original[1]).cross(original[0]-original[2])).normalized();
        double intensity=normal.dot(light_dir);
       
        if(intensity>0)
        {
        
            triangle3D(perVertex[0],perVertex[1],perVertex[2],image, uvs,intensity,zBufferP,Mywidth,myModel->diffuseMap);
            //triangle3D(tempVertex[0],tempVertex[1],tempVertex[2],image, uvs,intensity,zBufferP,Mywidth,myModel->diffuseMap);
        }
    }

    //cout<<"r "<<(int)myModel->diffuseMap.get(489,966).r<<" g "<<(int)myModel->diffuseMap.get(489,966).g<<" b "<<(int)myModel->diffuseMap.get(489,966).b<<endl;
    


    image.flip_vertically();  
    image.write_tga_file("lesson4.tga");
   // image.write_tga_file("lesson3.tga");
    delete myModel;
    return 0;

}