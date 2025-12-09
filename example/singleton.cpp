//单列模式练习;


#include<iostream>
// 饿汉模式：程序启动就实例化; 优点：以空间换时间;
class Singleton
{
public:
   ~Singleton(){};
   static Singleton& getInstance()
   {
     return _leton;
   }
    Singleton(const Singleton&)=delete;
    int getData()
    {
        return _data;
    }
private:
   Singleton()
   {
     std::cout<<"单例模式创建成功，此为饿汉模式"<<std::endl;
   }
   static Singleton _leton;
   int _data=2;
};
//定义;
Singleton Singleton::_leton;

//懒汉模式;
class LanHan
{
public:
  LanHan(const LanHan&)=delete;
  static LanHan& getInstance()
  {
     static LanHan _lanhan;
     return _lanhan;
  }
     int getData()
    {
        return _data;
    }
private:
   LanHan()
   {
     std::cout<<"这是懒汉模式"<<std::endl;
   }
   int _data=1;
};


int main()
{
    // std::cout<<Singleton::getInstance().getData()<<std::endl;
    //std::cout<<LanHan::getInstance().getData()<<std::endl;
    return 0;
}
