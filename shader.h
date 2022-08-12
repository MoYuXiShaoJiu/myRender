//这是重构代码之后的shader
//输入是模型，以及目标图片
//输出是图片
//一个类
#include"model.h"
#include"tgaimage.h"
#include"eigen3/Eigen/Dense"
#include"draw.h"
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

 void MSAA(TGAImage &image,TGAImage& diffuseMap,vector<Vector2d> &uvs,double intensity,double* MSAA_buffer)//使用一个bool值来控制是否使用MSAA，坐标就直接使用类内部的数据，这样就节约了时间
    {
        int left,right,top,bottom,valid_point_num,sample_point_num,I_width,I_heigh;//构造包围盒
        left=max(0.0,min(screen_coordinates[0].x(),min(screen_coordinates[1].x(),screen_coordinates[2].x())));
        right=min((double)image.get_width(),max(screen_coordinates[0].x(),max(screen_coordinates[1].x(),screen_coordinates[2].x())));
        top=min((double)image.get_height(),max(screen_coordinates[0].y(),max(screen_coordinates[1].y(),screen_coordinates[2].y())));
        bottom=max(0.0,min(screen_coordinates[0].y(),min(screen_coordinates[1].y(),screen_coordinates[2].y())));
        sample_point_num=4;//总的子采样点数
        double u,v,id,jd,tempz,coefficient;
        Vector3d wd_bc,temp,p;
        TGAColor finalColor;
        Vector2d add_num[4];
        I_heigh=image.get_height();
        I_width=image.get_width();
        add_num[0]=Vector2d(0.25,0.25);
        add_num[1]=Vector2d(0.25,0.75);
        add_num[2]=Vector2d(0.75,0.25);
        add_num[3]=Vector2d(0.75,0.75);
        for(int i=left;i<right;i++)
        {
            for(int j=bottom;j<top;j++)//挑选一个点
            {
                id=(double)i;
                jd=(double)j;
                valid_point_num=0;
                //先计算出有几个子采样点在三角形中
                //如果一个都没有就不用计算颜色乐
                for(int k=0;k<4;k++)//对四个采样点判断
                {
                    temp=barycentric(screen_coordinates[0],screen_coordinates[1],screen_coordinates[2],i+add_num[k].x(),j+add_num[k].y());
                    if(temp.x()<1&&temp.x()>0&&temp.y()<1&&temp.y()>0&&temp.z()>0&&temp.z()<1)//如果该点在三角形内
                    {
                        wd_bc=Vector3d(temp.x()/world_coordinates[0][2],temp.y()/world_coordinates[1][2],temp.z()/world_coordinates[2][2]);
                        wd_bc=wd_bc/(wd_bc.x()+wd_bc.y()+wd_bc.z());//取得投影前的重心坐标
                        tempz=wd_bc.x()*world_coordinates[0].z()+wd_bc.y()*world_coordinates[1].z()+wd_bc.z()*world_coordinates[2].z();//矫正后的z坐标
                        if(tempz>MSAA_buffer[i*I_heigh*4+j*4+k])//如果满足zbuffer
                        {
                            MSAA_buffer[i*I_heigh*4+j*4+k]=tempz;
                            valid_point_num++;//有效子采样点增加
                        }
                    }
                }
                if(valid_point_num==0)
                {
                    continue;//如果这个点的四个子测试点都不在三角形内
                }
                else//如果有点在三角形内
                {
                    u=0.0;
                    v=0.0;
                    p=barycentric(screen_coordinates[0],screen_coordinates[1],screen_coordinates[2],id,jd);
                    for(int k=0;k<3;k++)
                    {
                        u+=uvs[k][0]*p[k];
                        v+=uvs[k][1]*p[k];
                    }
                    coefficient=(double)valid_point_num/(double)sample_point_num;
                    finalColor=fragment(u,v,diffuseMap,intensity)*coefficient;
                    image.set(i,j,finalColor);

                }


            }   
        }

    }







};
