#include<iostream>
#include<mutex>

using namespace std;



class LazySingleInstance{
public:
    static LazySingleInstance* getInstance()
    {
        if (_lazySingleInstance==nullptr)
        {
            _lazySingleInstance = new LazySingleInstance();
        }
        return _lazySingleInstance;
        
    };

private:
    LazySingleInstance(){/*构造函数*/};
    ~LazySingleInstance(){/*析构函数*/};
    LazySingleInstance(const LazySingleInstance &single){};
    const LazySingleInstance& operator=(const LazySingleInstance& single){};

private:
    static LazySingleInstance* _lazySingleInstance;
};



 LazySingleInstance* LazySingleInstance::_lazySingleInstance = nullptr;

class HungrySingleInstance{
public:
    static HungrySingleInstance* getinstance()
    {
        return _HungryInstance;
    }

private:
    static  HungrySingleInstance*  _HungryInstance;
    HungrySingleInstance(){};
    ~HungrySingleInstance(){};
    HungrySingleInstance(const HungrySingleInstance& single){};
    const HungrySingleInstance& operator=(const HungrySingleInstance& single){};
};
HungrySingleInstance* HungrySingleInstance::_HungryInstance = new HungrySingleInstance();
