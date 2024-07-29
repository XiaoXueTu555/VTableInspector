#pragma once
/*
VTableInspector.h

VTableInspector is a library for probing and printing the virtual function
tables (vtable) and memory layout of objects in C++. This library is suitable
for cross-platform debugging, program introspection, security verification,
and educational and research scenarios.

Usage:
1. Add the INSERT_ID_MEMBER macro at the beginning of the class to insert an ID
   object that records the size of the class.
2. Use the PrintClassAll_VFTABLE function to print the object's virtual
   function tables and memory layout.

Example:

#include "VTableInspector.h"

class Base
{
public:
    virtual void func1() { printf("Base::func1\n"); }
    virtual void func2() { printf("Base::func2\n"); }
    INSERT_ID_MEMBER
    Base() : ID_INIT {}
};

class Derived : public Base
{
public:
    void func1() override { printf("Derived::func1\n"); }
    void func3() { printf("Derived::func3\n"); }
    INSERT_ID_MEMBER
    Derived() : Base(), ID_INIT {}
};

int main()
{
    Derived d;
    PrintClassAll_VFTABLE(&d);
    return 0;
}

/*
* Main Functions:
*
* VFPTRS& get_VFTABLE(T* ptr)
*   - Retrieves the virtual function tables of the object pointed to by ptr.
*   - Returns a VFPTRS object containing the virtual function tables.
*   - Note: The function assumes that the object has been properly initialized
*           and that the virtual table pointers are valid.
*
* void PrintVFTable(const VFPTR& vftable)
*   - Prints the contents of a virtual function table.
*   - Takes a virtual function table pointer as an argument.
*   - Note: This function relies on the stdio printf function. If the
*           environment does not support printf or if the PRINT_RUN macro is
*           manually undefined in the header file, this function will not
*           print any information.
*
* void PrintClassAll_VFTABLE(T* p)
*   - Prints all virtual function tables of an object.
*   - Takes a pointer to the object as an argument.
*   - Note: Ensure the object is fully constructed before passing it to this
*           function. This function also relies on the stdio printf function,
*           and will not print any information if printf is not supported or
*           if the PRINT_RUN macro is manually undefined.
*/

/*
* This library typically does not have general cross-platform capabilities and
* requires a specific compiler implementation, generally requiring the
* compiler to meet the following assumptions:
* 1. For classes containing virtual functions, the vtable pointer contained
*    in the class should be at the beginning of the memory layout.
* 2. The storage order of members in the memory layout should be consistent
*    with the declaration order in the class.
* 3. Each virtual function table must end with a null pointer (nullptr).
* 4. The size of the long type must be consistent with the size of the void*
*    type (in 32-bit or 64-bit).
*/

/* Control whether to print; do not reference the stdio library when generating */
#define PRINT_RUN

#ifdef PRINT_RUN
#include <stdio.h>
#else
void printf(const char* a, ...) {}
#endif // PRINT_RUN

/* Check if the compiler supports nullptr */
#if defined(__has_feature)
#if __has_feature(cxx_nullptr)
// Compiler supports nullptr, do nothing.
#else
// Compiler does not support nullptr.
#define nullptr NULL
#endif
#elif defined(__cplusplus) && __cplusplus >= 201103L
// Compiler supports C++11 and nullptr, do nothing.
#else
// Compiler does not support C++11 or nullptr.
#define nullptr NULL
#endif

// Insert ID member in the class
#define INSERT_ID_MEMBER public: ID id;
// Call the macro in the class constructor's initialization list
#define ID_INIT id(sizeof(*this))

/*
 * The ID class is used to store metadata about an object, specifically its
 * size. This information is used by VTableInspector to help locate and
 * interpret virtual function tables (vtables) within an object's memory layout.
 */
class ID
{
public:
    // Constructor initializes the ID with a placeholder and the object's size.
    ID(long sizeofclass)
        : id(placeholder_id), sizeofclass(sizeofclass) {}
public:
    const long id;           // Placeholder member used for identification.
    const long sizeofclass;  // Size of the object.
public:
    // Placeholder ID value.
    static const long placeholder_id = 0;
};

// Virtual function pointer
typedef void(*VF)();
// Virtual function table
typedef VF* VFPTR;

// Array of multiple virtual function tables
class VFPTRS
{
public:
    VFPTRS(int arr_size = 10) { Init(arr_size); }
    ~VFPTRS() { delete[] this->vftables; }
    inline void Init(int arr_size)
    {
        if (arr_size <= 0)
            arr_size = 10;
        delete[] this->vftables;
        this->arraySize = arr_size;
        this->vftables = new VFPTR[this->arraySize]();
        this->_size = 0;
    }
    bool AddVFPTR(VFPTR vftable)
    {
        if (this->_size >= this->arraySize)
            return false;
        this->vftables[_size++] = vftable;
        return true;
    }
    void Print() const
    {
        for (int i = 0; i < this->_size; i++)
        {
            printf("%d: Address: %p\n", i, this->vftables[i]);
        }
    }
    int size() const { return this->_size; }
    VFPTR& get_VFTable(int i) const { return this->vftables[i]; }
private:
    VFPTR* vftables;
    int _size;
    int arraySize;
};

// Reinterpret PTR as long
#define reinterpret_PTR_AS_LL(OFFSET) *reinterpret_cast<long*>(OFFSET + reinterpret_cast<char*>(ptr))

// Reinterpret PTR as VFPTR
#define reinterpret_PTR_AS_VFPTR(OFFSET) *reinterpret_cast<VFPTR*>(OFFSET + reinterpret_cast<char*>(ptr))

// Return null pointer if not found
template<class T>
VFPTRS& get_VFTABLE(T* ptr)
{
    const int MAXSIZE = 5;
    // Array of virtual function tables
    static VFPTRS vftables(MAXSIZE);
    // Base address (indicates the starting position of reading a directly 
    // inherited class)
    long base_address = 0;
    // Offset (relative to the base address, representing the read position)
    long offset = 0;

    // Initialization
    vftables.Init(MAXSIZE);

    while (1)
    {
        // Read 8 bytes of data from this segment
        long data = reinterpret_PTR_AS_LL(offset);

        // Check if it is placeholder data
        if (data != ID::placeholder_id)
        {
            // Read vtable
            VFPTR temp = reinterpret_PTR_AS_VFPTR(offset);
            vftables.AddVFPTR(temp);
            offset += sizeof(VFPTR);
            continue;
        }

        // This segment of data is ID
        data = reinterpret_PTR_AS_LL(offset + sizeof(ID::placeholder_id));

        // Mark that all vtables have been read
        if (data == ptr->id.sizeofclass) break;

        // Mark that the data of a certain class has been read
        if (data == offset + sizeof(ID) - base_address)
        {
            base_address += data;
            offset = base_address;
        }
        else
        {
            base_address = data;
            offset = base_address;
        }
    }
    return vftables;
}

// Parameter can also be written as: VFPTR vftable
void PrintVFTable(const VFPTR& vftable) {
    printf("|-VTable Address: %p\n", vftable);
    for (int i = 0; vftable[i] != nullptr; ++i) {
        printf("   |.Index: %d Virtual Function Address: 0X%x,->", i, *reinterpret_cast<unsigned int*>(vftable[i]));
        vftable[i]();
        printf("\n");
    }
    printf("\n");
}

template<class T>
void PrintClassAll_VFTABLE(T* p)
{
    VFPTRS& vftables = get_VFTABLE(p);

    printf("|-Object Address: %p Contains %d VTables\n", p, vftables.size());
    for (int i = 0; i < vftables.size(); i++)
    {
        PrintVFTable(vftables.get_VFTable(i));
    }
}
