# VTableInspector

**VTableInspector** is a library for probing and printing the virtual functiontables (vtable) and memory layout of objects in C++. 
This library is suitablefor cross-platform debugging, program introspection, security verification,and educational and research scenarios.
## Features

- Inspect and print virtual function tables (vtables) of C++ classes.
- Retrieve and display memory layout information related to vtables.
- Lightweight and header-only library.

## Prerequisites

- C++11 or later.
- For classes containing virtual functions, the vtable pointer contained
    in the class should be at the beginning of the memory layout.
- The storage order of members in the memory layout should be consistent
    with the declaration order in the class.
- Each virtual function table must end with a null pointer (nullptr).
- The size of the long type must be consistent with the size of the void*
    type (in 32-bit or 64-bit).
## Usage

### 1. Add the Header File

Include the `VTableInspector.h` header file in your project. Ensure that it is accessible to your source files.

### 2. Insert ID in Your Class

To enable inspection, you need to insert an `ID` member into all the classes in your class's inheritance diagram and initialize it. Use the following macros:

```cpp
#include "VTableInspector.h"

class Base
{
public:
    virtual void func1() { printf("Base::func1\n"); }
    virtual void func2() { printf("Base::func2\n"); }
    INSERT_ID_MEMBER
    Base() : ID_Init {}
};

class Derived : public Base
{
public:
    void func1() override { printf("Derived::func1\n"); }
    void func3() { printf("Derived::func3\n"); }
    INSERT_ID_MEMBER
    Derived() : Base(), ID_Init {}
};
