#include "VTableInspector.h"

class Base
{
public:
    virtual void func1() { printf("Base::func1\n"); }
    virtual void func2() { printf("Base::func2\n"); }
    //Insert an ID, making sure the id is at the beginning of all members of the class
    INSERT_ID_MEMBER
        //Initialize the ID in the initialization list
        Base() : ID_INIT{}
};

class Derived : public Base
{
public:
    void func1() override { printf("Derived::func1\n"); }
    void func3() { printf("Derived::func3\n"); }
    INSERT_ID_MEMBER
        Derived() : Base(), ID_INIT{}
};

int main()
{
    Derived d;
    PrintClassAll_VFTABLE(&d); //print the VFtable
    return 0;
}
