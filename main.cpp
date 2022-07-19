#include<iostream>
#include"eigen3/Eigen/Dense"
#include"tgaimage.h"
#include"draw.h"
#include"model.h"

using namespace std;
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green=TGAColor(0,255,0,255);
const int height = 800;
const int width = 800;

Vector3d world2screen(Vector3d v) 
{
    return Vector3d(int((v.x()+1.)*width/2.+.5), int((v.y()+1.)*height/2.+.5), v.z());
}


model *myModel = NULL;

int  main()
{
    /*
    关于使用的中坐标系说明:
    这里使用的是屏幕平面为xy平面，x正方向向右，y正方向向上，
    z轴垂直屏幕平面，无限远处为0，z轴正方向垂直平面向外，即x.corss(y)
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
    double zBuffer[width][height] = {0};//其实这里应该是无限远的值
    double* zBufferP=zBuffer[0];//zbuffer的指针
    TGAImage image(width, height, TGAImage::RGB);//创建图
    vector<int> temp;//用来暂时存储点的次序
    Vector3d tempVertex[3];//用来暂时存储三个点
    Vector3d original[3];
    vector<TGAColor> vertexColor;//三角形三个顶点的颜色
    for(int i=0;i<myModel->faceNumber();i++)//对每个三角形渲染
    {
        temp=myModel->getTriangle(i);//取得序列
        vertexColor=myModel->getcolor(i);//所以问题出现在这里
        for(int j=0;j<3;j++)
        {
            tempVertex[j]=world2screen(myModel->getVertex(temp[j])) ;
            original[j]=myModel->getVertex(temp[j]);
            //cout<<"color "<<i<<" r "<<(vertexColor[i].r) <<" g "<<(vertexColor[i].g) <<" b "<<(vertexColor[i].b) <<endl;//输出的值确实是满足要求的
        }
     
        Vector3d normal=((original[0]-original[1]).cross(original[0]-original[2])).normalized();
        double intensity=normal.dot(light_dir);
       
        if(intensity>0)
        {
            triangle3D(tempVertex[0],tempVertex[1],tempVertex[2],image, vertexColor,intensity,zBufferP,width);
        }
    }

    cout<<"r "<<(int)myModel->diffuseMap.get(489,966).r<<" g "<<(int)myModel->diffuseMap.get(489,966).g<<" b "<<(int)myModel->diffuseMap.get(489,966).b<<endl;
    


    image.flip_vertically();  
    image.write_tga_file("output.tga");
    delete myModel;
    return 0;

}