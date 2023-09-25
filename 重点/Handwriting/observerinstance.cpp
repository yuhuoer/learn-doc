#include<iostream>
#include<vector>
#include<string>

using namespace std;

class Interview;
class Student{
public:
    Student(string name, Interview* inter):m_name(name),pintervirw(inter){};
    ~Student(){};
    void Update(string action)
    {
        cout << action << endl;
    }

private:
    Interview* pintervirw;
    string m_name;
};

class Interview{
public:
    Interview(){};
    ~Interview(){};
    void AddObserver(Student* stu)
    {
        observer.push_back(stu);
    }
    void SetState(string result)
    {
        state = result;
        Notify();
    }
    void Notify()
    {
        for(auto iter : observer)
        {
            iter->Update(state);
        }
        
    }
private:
    vector<Student*> observer;
    string state;

};




int main(int argc, char const *argv[])
{
    Interview* inter = new Interview();
    Student* tyx = new Student("tyx",inter);
    inter->AddObserver(tyx);
    inter->SetState("Sucess!");

    return 0;
}

