#ifndef MTSTLALLOCATOR_H
#define MTSTLALLOCATOR_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <cstddef>
#include <memory>
#include <limits>
#include "MTMemoryPrivate.h"


// STL ALLocator
// Base STL allocator class.
template<typename T>
struct STLAllocatorBase
{   // base class for generic allocators
    typedef T value_type;
};

// Base STL allocator class. (const T version).
template<typename T>
struct STLAllocatorBase<const T>
{   // base class for generic allocators for const T
    typedef T value_type;
};
/**
 * @description of STLAllocator
 *
 * @details definition and allocator for STLAllocator
 */
template<typename T>
class STLAllocator : public STLAllocatorBase<T>
{
public :
    /// define our types, as per ISO C++
    typedef STLAllocatorBase<T>         _Base;
    typedef typename _Base::value_type  value_type;
    typedef value_type*                 pointer;
    typedef const value_type*           const_pointer;
    typedef value_type&                 reference;
    typedef const value_type&           const_reference;
    typedef ::std::size_t               size_type;
    typedef ::std::ptrdiff_t            difference_type;


    /// the standard rebind mechanism
    template<typename U>
    struct rebind
    {
        typedef STLAllocator<U> other;
    };

    /// ctor
    inline explicit STLAllocator()
    {
    }

    /// dtor
    virtual ~STLAllocator()
    {
    }

    /// copy ctor - done component wise
    inline STLAllocator(STLAllocator const&)
    {
        // for cppcheck, must have this function ,or it will cause link error

    }

    /// cast
    template <typename U>
    inline STLAllocator(STLAllocator<U> const&)
    {
    }


    /// memory allocation (elements, used by STL)
    inline pointer allocate(size_type count,
        typename std::allocator<void>::const_pointer ptr = 0)
    {
        static_cast<void>(ptr);
        return static_cast<pointer>(MEM_malloc_private(count * sizeof(T)));
    }

    /// memory deallocation (elements, used by STL)
    inline void deallocate(pointer ptr, size_type)
    {
        MEM_free_private(ptr);
    }

    pointer address(reference x) const
    {
        return &x;
    }

    const_pointer address(const_reference x) const
    {
        return &x;
    }

    size_type max_size() const throw()
    {
        // maximum size this can handle, delegate
        return std::numeric_limits<size_t>::max();
    }

    void construct(pointer p)
    {
        // call placement new
        new(static_cast<void*>(p)) T();
    }

    void construct(pointer p, const T& val)
    {
        // call placement new
        new(static_cast<void*>(p)) T(val);
    }

    void destroy(pointer p)
    {
        // do we have to protect against non-classes here?
        // some articles suggest yes, some no
        p->~T();
    }

    STLAllocator<T>& operator=(STLAllocator<T> const&)
    {
        return *this;
    }
};

/// determine equality, can memory from another allocator
/// be released by this allocator, (ISO C++)
template<typename T, typename T2>
inline bool operator==(STLAllocator<T> const&,
    STLAllocator<T2> const&)
{
    // same alloc policy (P), memory can be freed
    return true;
}

/// determine equality, can memory from another allocator
/// be released by this allocator, (ISO C++)
template<typename T, typename OtherAllocator>
inline bool operator==(STLAllocator<T> const&,
    OtherAllocator const&)
{
    return false;
}
/// determine equality, can memory from another allocator
/// be released by this allocator, (ISO C++)
template<typename T, typename T2>
inline bool operator!=(STLAllocator<T> const&,
    STLAllocator<T2> const&)
{
    // same alloc policy (P), memory can be freed
    return false;
}

/// determine equality, can memory from another allocator
/// be released by this allocator, (ISO C++)
template<typename T, typename OtherAllocator>
inline bool operator!=(STLAllocator<T> const&,
    OtherAllocator const&)
{
    return true;
}
/// end define STL Allocator


#endif //MTSTLALLOCATOR_H