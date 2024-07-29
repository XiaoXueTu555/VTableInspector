# VTableInspector

**VTableInspector** is a C++ library designed to inspect and print the virtual function tables (vtables) and memory layout of objects. This library is useful for debugging, introspection, security validation, and educational purposes.

## Features

- Inspect and print virtual function tables (vtables) of C++ classes.
- Retrieve and display memory layout information related to vtables.
- Lightweight and header-only library.

## Prerequisites

- C++11 or later.
- A C++ compiler that supports `long` and `void*` types with the same size (32-bit or 64-bit).

## Usage

### 1. Add the Header File

Include the `VTableInspector.h` header file in your project. Ensure that it is accessible to your source files.

### 2. Insert ID in Your Class

To enable inspection, you need to insert an `ID` member in your class and initialize it. Use the following macros:

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
