#pragma once
#include "../common/stdinc.h"

class class_pointer
{
public:
    class_pointer(std::uintptr_t address = 0)
        :_address(address)
    {

    }

    class_pointer& operator=(std::uintptr_t address)
    {
        _address = address;
        return *this;
    }

    void set(std::uintptr_t address)
    {
        _address = address;
    }

    std::uintptr_t get() const
    {
        return _address;
    }

    template <typename T>
    T* get() const
    {
        return reinterpret_cast<T*>(_address);
    }

    template <typename T>
    T* get_member_pointer(std::uintptr_t offset) const
    {
        return reinterpret_cast<T*>(_address + offset);
    }

    template <typename T>
    const T* get_const_member_pointer(std::uintptr_t offset) const
    {
        return get_member_pointer<T>(offset);
    }

    template <typename T>
    T get_member(std::uintptr_t offset) const
    {
        return *get_const_member_pointer<T>(offset);
    }

    //TODO: call_member_function
    template <typename Ret, typename ...Arg>
    Ret call_member_function(std::uintptr_t func_addr, Arg ...args)
    {
        return reinterpret_cast<Ret(__thiscall*)(void*, Arg...)>(func_addr)(reinterpret_cast<void*>(_address), std::forward<Arg>(args)...);
    }

    //TODO: call_virtual_function
    template <typename Ret, typename ...Arg>
    Ret call_virtual_function(std::size_t index, Arg ...args)
    {
        auto func_addr = get_member<std::uintptr_t*>(0)[index];

        return call_member_function(func_addr, std::forward<Arg>(args)...);
    }

private:
    std::uintptr_t _address;
};
