//用来解析obj文件的
#pragma once
#include"eigen3/Eigen/Dense"
#include<iostream>
#include<vector>
#include<sstream>
#include<istream>
#include<ostream>
#include<fstream>
#include<string>
using namespace Eigen;
using namespace std;

class model
{
    private:
    vector<Vector3d> vertexs;//点的vector
    vector<vector<int>> faces;//点的连接的vector
    vector<Vector3i> textureN;//用来存储纹理序列
    vector<Vector2d> tex;//用来存储纹理矩阵
    
    
    public:
    TGAImage diffuseMap;//用来存储纹理本身
    model(const string fileName):vertexs(),faces(),textureN(),tex()//modle的构造函数，读入obj文件名
    {
        ifstream inputFile ;
        inputFile.open(fileName,ifstream::in);//以读的方式打开
        if(inputFile.fail())
        {
            cout<<"fail to open the file"<<endl;//如果打开文件失败就输出这段话
            return;
        }
        cout<<"成功打开文件"<<endl;
        string line;

        while(!inputFile.eof())//如果没有到结尾
        {
            
            getline(inputFile,line);//从输入流中读取一行
            
            istringstream iss(line.c_str());//istringstresm从istream派生而来，提供读string的功能
            
            char trash;
            string trash1;
            if(!line.compare(0,2,"v "))//如果开头是v的话，也就是点
            {
                
                iss>>trash;//把不需要的代表属性的字母处理掉
                Vector3d v;
                for(int i=0;i<3;i++)
                {
                    iss>>v[i];
                    //cout<<v[i];
                }
                //cout<<","<<endl;
                this->vertexs.push_back(v);  //把这个点存储进类内的vertexs
            }
            else if(!line.compare(0,2,"f "))//如果是f的话，就是存储点的连接顺序的序列
            {
               
                iss>>trash;//同样是先把不需要的字母处理掉
                vector<int> f;
                Vector3i t;
                int itrash,idx,Mytex;
                int k=0;
                while(iss>>idx>>trash>>Mytex>>trash>>itrash)//看一下obj文件中f开头的行就明白了，中间有/要处理，而且形成三角形的三个点也不是连在一起的
                {
                    idx--;
                    Mytex--;
                    f.push_back(idx);
                    t[k]=Mytex;
                    k++;
                   
                }
               
                //将这个三角形加入序列中
                this->faces.push_back(f);
                this->textureN.push_back(t);//将纹理加入tex列表
            }

             else if(!line.compare(0,3,"vt "))//比较是没有问题的
             {
                
                double dTrash,u,v;
                iss>>trash>>trash;
                iss>>u>>v;
                Vector2d temp(u,v);
                tex.push_back(temp);
                
             }

            
        
        }
        
         std::cerr << "# v# " << vertexs.size() << " f# "  << faces.size() << std::endl;
         cerr<<"# vt#" <<tex.size()<<endl;//检查没有问题
         loadDiffuseTex(fileName,"_diffuse.tga",diffuseMap);
         inputFile.close();
        //  for(int i=0;i<tex.size();i++)
        //  {
        //     for(int j=0;j<2;j++)
        //     {
        //         cout<<tex[i][j];
        //         cout<<"   ";
                
        //     }
        //     cout<<"next";
        //     cout<<" "<<endl;
        //  }
    }

    //获取三角形的数量
    int faceNumber()
    {
        return faces.size();
    }
    //获取指定的三角形序列
    vector<int> getTriangle(int x)
    {
        return faces[x];
    }
    //获取指定位置的点
    Vector3d getVertex(int x)
    {
        return vertexs[x];

    }
    //获取点的数量
    int vertexNumber()
    {
        return vertexs.size();
    }
    //获取指定的纹理
    Vector2d getTexture(int n)
    {
        return this->tex[n];
    }
    //获取指定的纹理序列
    Vector3i getTexOrder(int n)
    {
        return this->textureN[n];
    }
    //读入纹理
    void loadDiffuseTex(string filename,string suffix,TGAImage &img)
    {
        size_t dot=filename.find_last_of(".");
        if(dot==string::npos)return;
        string texfile=filename.substr(0,dot)+suffix;
       
        cerr<<"texture file "<<texfile<<" loading "<<(img.read_tga_file(texfile.c_str())?"ok":"failed")<<endl;
        img.flip_vertically();
    }
    vector<TGAColor> getcolor(int faceN)//输入的是f的编号
    {
        Vector3i  texOrder=this->getTexOrder(faceN);//输入是face数量，这里先取得texOrder
        //cout<<"在这里"<<endl;
        vector<Vector2d> tempTex;//用来暂时存储uv的值
        vector<TGAColor> finalResult;//返回的值
        vector<Vector2i> processedUV;
        for(int i=0;i<3;i++)
        {
            //cout<<"get in for"<<endl;
            //这里根据序列取得uv的值
            tempTex.push_back(this->getTexture(texOrder[i])) ;//先取得原来uv的值
            //cout<<"tempTex[i][0]"<<tempTex[i][0]*1024<<"tempTex[i][1]"<<tempTex[i][1]*1024<<endl;
            //接着乘上img的大小,将处理好的uv
            processedUV.push_back(Vector2i(tempTex[i][0]*diffuseMap.get_width(),tempTex[i][1]*diffuseMap.get_height()));

        }
        //注意一下循环使用的情况
        // for(int j=0;j<3;j++)
        // {
        //     //cout<<"processed_uv"<<" u "<<processedUV[j].x()<<" v "<<processedUV[j].y()<<endl;
            
            
           
        // }
        for(int k=0;k<3;k++)
        {
            finalResult.push_back(diffuseMap.get(processedUV[k].x(),processedUV[k].y()));
           // cout<<"color"<<" r "<<int(final[k].r)<<" g "<<int(final[k].g)<<" b "<<int(final[k].b)<<endl;
        }
        //cout<<" "<<endl;
        int r=unsigned(diffuseMap.get(489,966).r) ;
        int g=(int)diffuseMap.get(489,966).g;
        int b=(int)diffuseMap.get(489,966).b;
        //cout<<"test"<<" r "<<r<<" g "<<g<<" b "<<b<<endl;
        
        return finalResult;

    }
    vector<Vector2d> getUV(int i)//取得指定位置的原始uv值
    {
        Vector3i order=getTexOrder(i);//取得这个位置的三角形对应的顺序
        vector<Vector2d> result;
        for(int i=0;i<3;i++)
        {
            result.push_back(this->getTexture(order[i]));
        }
        return result;

    }




};
