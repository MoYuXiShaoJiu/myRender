// #pragma once
// #include"shader.h"
// #include<eigen3/Eigen/Dense>
// using namespace Eigen;
// //用来解决shadow mapping的事
// //这里负责第一趟
// //还有后面的处理问题
// //可以重载fragment函数，达到效果
// //重写一个shadow的类来重写其中的fragment函数达到完成阴影的效果
// class ShadowRender:public shader
// {
//     private :
//     //相机所在位置
//     Vector3d Camera_Position;
//     int Image_Width,Image_Heigh;
//     double* shadowBuffer;
// //******************************* 构建shadow
//     void creatShadowBuffer()
//     {

//     }
// //*************************************************


//     public:

    
//     ShadowRender(Vector3d camera_position,int image_width,int image_heigh)
//     {
//         this->Camera_Position=camera_position;// 将传进来位置存储起来
//         this->Image_Heigh=image_heigh;
//         this->Image_Width=image_width;
//         creatShadowBuffer();//构造一个shaodowBuffer

//     }   
// };







