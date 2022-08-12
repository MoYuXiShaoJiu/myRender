// //用于实现反走样
// //MSAA
// //
// //这里使用的是4*4的mass
// //输入一个点，一个在包围盒中的点，输出颜色
// void rst::rasterizer::rasterize_triangle(const Triangle& t) 
// {
//     auto v = t.toVector4();//写成齐次坐标的形式
//     //构造包围盒
//     float bound_t = std::max(v[0].y(), std::max(v[1].y(), v[2].y()));
//     float bound_b = std::min(v[0].y(), std::min(v[1].y(), v[2].y()));
//     float bound_l = std::min(v[0].x(), std::min(v[1].x(), v[2].x()));
//     float bound_r = std::max(v[0].x(), std::max(v[1].x(), v[2].x()));

//     float shiftValue[4][2] = {
//             {0.25, 0.25},
//             {0.25, 0.75},
//             {0.75, 0.25},
//             {0.75, 0.75}
//     };
//     for(int j = floor(bound_b); j <= ceil(bound_t); j++)
//     {
//         for(int i = floor(bound_l); i <= ceil(bound_r); i++)
//         {
//             int current_pixel_index = get_index(i, j);
//             bool changeFlag = false;
//             for(int k = 0; k < 4; k++)
//             {
//                 float x = i + shiftValue[k][0], y = j + shiftValue[k][1];
//                 if (insideTriangle(x, y, t.v))//如果这个点在三角形内
//                 {
//                     auto [alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
//                     float w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
//                     float z_interpolated =
//                             alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();//计算z的插值
//                     z_interpolated *= w_reciprocal;
//                     if (depth_buf_msaa[k][current_pixel_index] > z_interpolated)//zbuffer
//                     {
//                         changeFlag = true;
//                         depth_buf_msaa[k][current_pixel_index] = z_interpolated;
//                         frame_buf_msaa[k][current_pixel_index] = t.getColor() / 4;
//                     }
//                 }
//             }
//             if(changeFlag)
//             {
//                 Eigen::Vector3f color = Eigen::Vector3f{0, 0, 0};
//                 for(int m = 0; m < 4; m++)
//                 {
//                     color += frame_buf_msaa[m][current_pixel_index];//计算出最终的颜色
//                 }
//                 set_pixel({i, j, 0}, color);
//             }
//         }
//     }
// }
