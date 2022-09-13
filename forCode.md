# 关于重构代码
这里重构代码主要是要把shader给拆开来，点的处理交给vertexshader来

## vertex shader
关于覆盖的处理单独的交给rasterizer，颜色交给fragmentshader，这样在写shadowmappig的时候的复用会变得简单一点

## rasterizer
这里和原来不同的一点是可以把插值的结果存起来，之后使用，这样就可以做到分离了

伪代码
```
for each triangle
    find the boundering box
    for each pixel in the triangle
        calculate the barycentric coordinates
        ……
```

重点还是在怎么把rasterizaer和fragmentshader分开与结合

从包围盒开始才是rasterizer的入口

实际上还是有必要把rasterier分开的，否则shadow mapping还是没有办法简化

理想中的shader是这样的：
```
shader(vertexShader,rasterizer,fragment,data)
```
在sahder中创建一个rasterizer的对象，用来处理创建包围盒还有插值

## fragmen shader
其实rasterizer还没有测试，但是我觉得单独使用的话测试也比较麻烦，而且fragment shader应该还蛮好写的

问题在于没有办法利用zbuffer

## normal mapping
light为什么要经过mvp变换？

为什么法线要逆变换?






