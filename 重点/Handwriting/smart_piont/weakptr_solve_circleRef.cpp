#include<iostream>
#include<memory>
#include<vector>

using namespace std;


class parent;
class children;


class parent{
public:
    ~parent(){cout << "destorying parent!" << endl;}
public:
    //shared_ptr<children> children;
    weak_ptr<children> children;
};

class children{
public:
    ~children(){cout << "destorying children!" << endl;}

public:
    shared_ptr<parent> parent;
    //weak_ptr<parent> parent;//循环引用中一方使用弱引用即可解决弱引用问题。
};

void test()
{
    shared_ptr<parent> father(new parent);
    shared_ptr<children> son(new children);
    father->children = son;
    cout << son.use_count() << " ";
    son->parent = father;
    cout << father.use_count() << endl;
}

int main(int argc, char const *argv[])
{
    test();
    return 0;
}
