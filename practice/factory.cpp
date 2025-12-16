// 学习设计模式//


#include<iostream>
#include<memory>
#include<string>
using namespace std;
// 简单工厂模式;违背了开闭规则;
// class Fruit
// {
// public:
//     Fruit(){};
//     virtual void name()=0;
// };

// class Apple :public Fruit
// {
// public:
//     Apple(){};
//     void name()
//     {
//         cout<<"苹果"<<endl;
//     }
// };

// class Banana :public Fruit
// {
// public:
//     Banana(){};
//     void name()
//     {
//         cout<<"香蕉"<<endl;
//     }
// };
// class FruitFactory
// {
// public:
//    static std::shared_ptr<Fruit> create(const std::string&name)
//    {
//        if(name=="苹果")
//        {
//          return make_shared<Apple>();
//        }
//        else
//        {
//          return make_shared<Banana>();
//        }
//    }
    
// };
// int main()
// {
//     std::shared_ptr<Fruit> fruit = FruitFactory::create("苹果");
//     fruit->name();
//     fruit = FruitFactory::create("⾹蕉");
//     fruit->name();
//     return 0;
// }

//⼯⼚⽅法模式
// class Fruit
// {
// public:
//     virtual void name()=0;
//     Fruit() {};
// };

// class Apple : public Fruit
// {
// public:
//     void name()
//     {
//         cout<<"我是苹果"<<endl;
//     }
//     Apple(){};
// };

// class Banana : public Fruit
// {
// public:
//     void name()
//     {
//         cout<<"我是香蕉"<<endl;
//     }
//     Banana(){};
// };

// class FruitFactory
// {
// public:
//     virtual shared_ptr<Fruit> create()=0;  
// };

// class AppleFactory : public FruitFactory
// {
//     shared_ptr<Fruit> create()
//     {
//         return make_shared<Apple>();
//     }
// };

// class BananaFactory : public FruitFactory
// {
//     virtual shared_ptr<Fruit> create()
//     {
//         return make_shared<Banana>();
//     }
// };

// int main()
// {
//     std::shared_ptr<FruitFactory> factory(new AppleFactory());
//     std::shared_ptr<Fruit> fruit=factory->create();
//     fruit->name();
//     factory.reset(new BananaFactory());
//     fruit=factory->create();
//     fruit->name();

//     return 0;
// }

//3.抽象工厂模式;
// class Fruit
// {
// public:
//     Fruit(){};
//     virtual void name()=0;
// };

// class Apple :public Fruit
// {
// public:
//     Apple(){};
//     void name()
//     {
//         cout<<"苹果"<<endl;
//     }
// };

// class Banana :public Fruit
// {
// public:
//     Banana(){};
//     void name()
//     {
//         cout<<"香蕉"<<endl;
//     }
// };

// class Animal
// {
// public:
//     virtual void name()=0;
//     Animal(){};
// };

// class Dog : public Animal
// {
// public:
//     void name()
//     {
//         cout<<"我是一只小狗"<<endl;
//     }
//     Dog(){};
// };
// class Cat : public Animal
// {
// public:
//     void name()
//     {
//         cout<<"我是一只小猫"<<endl;
//     }
//     Cat(){};
// };

// class Factory
// {
// public:
//     virtual shared_ptr<Fruit> makeFruit(const string& name)=0;
//     virtual shared_ptr<Animal> makeAnimal(const string& name)=0;
//     Factory(){};
// };

// class  AnimalFactory : public Factory
// {
// public:
//     shared_ptr<Animal> makeAnimal(const string& name)
//     {
//         if(name=="小狗")
//         {
//             return make_shared<Dog>();
//         }
//         else
//         {
//             return make_shared<Cat>();
//         }
//     }
//     shared_ptr<Fruit> makeFruit(const string& name)
//     {
//         return nullptr;
//     }
// };

// class  FruitFactory : public Factory
// {
// public:
//     shared_ptr<Animal> makeAnimal(const string& name)
//     {
//         return nullptr;
//     }
//     shared_ptr<Fruit> makeFruit(const string& name)
//     {   
//         if(name=="苹果")
//         {
//             return make_shared<Apple>();
//         }
//         else
//         {
//             return make_shared<Banana>();
//         }
//     }
// };


// int main()
// {
//     shared_ptr<Factory> factory=make_shared<FruitFactory>();
//     shared_ptr<Fruit> apple=factory->makeFruit("苹果");
//     apple->name();

//     factory.reset();
//     apple.reset();

//     return 0;
// }



#include<memory>
#include<string>
using namespace std;
//代理模式;
class RentHouse
{
public:
    virtual void renthouse()=0;
    RentHouse(){};
};
class Landlord : public RentHouse
{
public:
   Landlord(){};
   void renthouse()
   {
     cout<<"房东将房子租出去"<<endl;
   }
};
class Intermediary : public RentHouse
{
public:
   void renthouse()
   {
     cout<<"带人看房"<<endl;
     cout<<"介绍房子"<<endl;
     _landlordd.renthouse();
   }
private:
   Landlord _landlordd;
};

// int main()
// {
//     Intermediary inter;
//     inter.renthouse();
//     return 0;
// }






class Fruit
{
public:
    Fruit(){};
    virtual void name()=0;
};

class Apple :public Fruit
{
public:
    Apple(){};
    void name()
    {
        cout<<"苹果"<<endl;
    }
};
class Banana :public Fruit
{
public:
    Banana(){};
    void name()
    {
        cout<<"香蕉"<<endl;
    }
};

class FruitFactory
{
public:
    virtual shared_ptr<Fruit> createFruit(const string& name)=0;
};
class AppleFactory :public FruitFactory
{
public:
    shared_ptr<Fruit> createFruit(const string& name)
    {
        return make_shared<Apple>();
    }
};
class BananaFactory :public FruitFactory
{
public:
    shared_ptr<Fruit> createFruit(const string& name)
    {
        return make_shared<Banana>();
    }
};
int main()
{
   shared_ptr<FruitFactory> ptr=make_shared<AppleFactory>();
   shared_ptr<Fruit> fruit=ptr->createFruit("苹果");
   fruit->name();
   ptr.reset();
   fruit.reset();
   return 0;
}