//这是重构代码之后的shader
//输入是模型，以及目标图片
//输出是图片
//一个类
#include"model.h"
#include"tgaimage.h"
#include"eigen3/Eigen/Dense"
#include"draw.h"
#include"vertexShader.h"
using namespace Eigen;
//对输出的图元进行处理输出fragment
class shader
{
   private:
    Matrix4d  trans_Matrix;//总的变换矩阵
    Vector3d  world_coordinates[3];//仅有一个物体的条件下,用来存放一个三角形
    Vector3d  screen_coordinates[3];
    bool MSAA_bool=false;//默认不开启msaa
    

    public:
    shader(Vector3d world_cd[],Matrix4d trans)//其实本质是rasterizer
    {
        this->trans_Matrix=trans;
        for(int i=0;i<3;i++)
        {
            world_coordinates[i]=world_cd[i];
            screen_coordinates[i]=HomogeneousTo(trans*PointToHomogeneous(world_cd[i]));
        }
    }
     shader(Vector3d world_cd[],Matrix4d trans,bool msaa)//其实本质是rasterizer
    {
        this->trans_Matrix=trans;
        for(int i=0;i<3;i++)
        {
            world_coordinates[i]=world_cd[i];
            screen_coordinates[i]=HomogeneousTo(trans*PointToHomogeneous(world_cd[i]));
        }
        MSAA_bool=msaa;
    }
    shader(Vector3d world_cd[],vertexShader&  vertex_shader,bool msaa)
    {
        this->trans_Matrix=vertex_shader.getTransMatrix();
         for(int i=0;i<3;i++)
        {
            world_coordinates[i]=world_cd[i];
            screen_coordinates[i]=vertex_shader.TransVertex(screen_coordinates[i]);
        }
        MSAA_bool=msaa;
    }
    
    void rasterize(TGAImage &image,TGAImage& diffuseMap,vector<Vector2d> &uvs,double intensity,double* zBuffer)//进行渲染
    {
        //求覆盖的像素
        //求插值
        int left,right,top,bottom,I_width;//构造包围盒
        left=max(0.0,min(screen_coordinates[0].x(),min(screen_coordinates[1].x(),screen_coordinates[2].x())));
        right=min((double)image.get_width(),max(screen_coordinates[0].x(),max(screen_coordinates[1].x(),screen_coordinates[2].x())));
        top=min((double)image.get_height(),max(screen_coordinates[0].y(),max(screen_coordinates[1].y(),screen_coordinates[2].y())));
        bottom=max(0.0,min(screen_coordinates[0].y(),min(screen_coordinates[1].y(),screen_coordinates[2].y())));
        double u,v;
        Vector3d p,wd_bc;
        TGAColor finalColor;
        I_width=image.get_width();
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
                        finalColor=fragment(u,v,diffuseMap,intensity);
                        zBuffer[i+j*I_width]=tempz;
                        image.set(i,j,finalColor);
                    }
                }

            }
            
        }

    }
    void rasterize(TGAImage &image,TGAImage& diffuseMap,vector<Vector2d> &uvs,double intensity,double* zBuffer,double* MSAA_Buffer,bool*pixel)//进行渲染,加入MSAA
    {
        //求覆盖的像素
        //求插值
        int left,right,top,bottom,I_width;//构造包围盒
        left=max(0.0,min(screen_coordinates[0].x(),min(screen_coordinates[1].x(),screen_coordinates[2].x())));
        right=min((double)image.get_width(),max(screen_coordinates[0].x(),max(screen_coordinates[1].x(),screen_coordinates[2].x())));
        top=min((double)image.get_height(),max(screen_coordinates[0].y(),max(screen_coordinates[1].y(),screen_coordinates[2].y())));
        bottom=max(0.0,min(screen_coordinates[0].y(),min(screen_coordinates[1].y(),screen_coordinates[2].y())));
        double u,v;
        Vector3d p,wd_bc;
        TGAColor finalColor;
        I_width=image.get_width();
        for(int i=left;i<=right;i++)
        {
            for(int j=bottom;j<=top;j++)
            {
                if(MSAA_bool)
                {
                    MSAA(i,j,MSAA_Buffer,pixel,I_width,image.get_height());
                }
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
                        finalColor=fragment(u,v,diffuseMap,intensity);
                        zBuffer[i+j*I_width]=tempz;
                        image.set(i,j,finalColor);
                    }
                }

            }
            
        }

    }

    void setVertex(Vector3d world_cd[])//更改当前shader中输出的图元
    {
        for(int i=0;i<3;i++)
        {
            world_coordinates[i]=world_cd[i];
            screen_coordinates[i]=HomogeneousTo(this->trans_Matrix*PointToHomogeneous(world_cd[i]));
        }

    }

    TGAColor fragment(double u,double v,TGAImage diffuse,double intensity)//给输入的图元返回颜色
    {
        return diffuse.get(u*diffuse.get_width(),v*diffuse.get_height())*intensity;
    }

 void MSAA(int i,int j,double* MSAA_Buffer,bool* pixel,int I_width,int I_heigh)//MSAA_Buffer用来计算子像素点的深度置，pixel用来记录是子像素点是否在三角形内
    {
       Vector2d location[4];
       location[0]=Vector2d(0.25,0.25);
       location[1]=Vector2d(0.25,0.75);
       location[2]=Vector2d(0.75,0.25);
       location[3]=Vector2d(0.75,0.75);
       double id,jd,tempz;
       id=(double)i;
       jd=(double)j;
       Vector3d temp,wd_bc;
       for(int k=0;k<4;k++)
       {
        temp=barycentric(screen_coordinates[0],screen_coordinates[1],screen_coordinates[2],id+location[k].x(),jd+location[k].y());
        if(temp.x()<1&&temp.x()>0&&temp.y()>0&&temp.y()<1&&temp.z()>0&&temp.z()<1)//在三角形内
        {
            //测试深度条件
            wd_bc=Vector3d(temp.x()/world_coordinates[0].z(),temp.y()/world_coordinates[1].z(),temp.z()/world_coordinates[2].z());
            wd_bc=wd_bc/(wd_bc.x()+wd_bc.y()+wd_bc.z());
            tempz=wd_bc.x()*world_coordinates[0].z()+wd_bc.y()*world_coordinates[1].z()+wd_bc.z()*world_coordinates[2].z();
            if(tempz>MSAA_Buffer[i*I_heigh*4+j*4+k])//如果满足深度条件
            {
                MSAA_Buffer[i*I_heigh*4+j*4+k]=tempz;//更新深度值
                pixel[i*I_heigh*4+j*4+k]=true;//更新是否在三角形中的值

            }
        }
       }

    }

void doMSAA(TGAImage &image,bool* pixel)//对image进行处理
{
    int tempWidth=image.get_width();
    int tempHeigh=image.get_height();
    double valid_point_num;
    TGAColor tempColor;
    for(int i=0;i<tempWidth;i++)
    {
        for(int j=0;j<tempHeigh;j++)
        {
            valid_point_num=0;
            tempColor=image.get(i,j);
            for(int k=0;k<4;k++)
            {
                valid_point_num+=pixel[i*tempHeigh*4+j*4+k]==true?1.0:0.0;
            }
            valid_point_num/=4.0;
            tempColor=tempColor*valid_point_num;
            image.set(i,j,tempColor);
            
        }
    }
}





};
