#ifndef HOSTED_TEST /* The compiler takes care of this for hosted systems. */

#include <stddef.h>
#include <stdint.h>
#include <unwind.h>

#include "../include/cxxabi"
#include "../include/array"
#include "../include/cstdio"
#include "../include/cstdlib"
#include "../include/new"
#include "../include/typeinfo"
#include "../include/vector"

/******************************************************************************
 ******************************************************************************/

namespace __cxxabiv1 {

/******************************************************************************
 ******************************************************************************/

extern "C"
void __cxa_pure_virtual()
{
    // Send a message to wherever printf goes, probably the system log.
    NMSP::printf("__cxa_pure_virtual called\n");
    NMSP::abort();
}

/******************************************************************************
 ******************************************************************************/

int __cxa_guard_acquire(__guard* g)
{
    // This is not thread safe. Since we're only operating with single threads
    // at the moment, that doesn't matter. Simply return whether the varibale
    // is already initialised, don't bother trying to lock it.
    return !(*reinterpret_cast<char*>(g));
}

/******************************************************************************
 ******************************************************************************/

void __cxa_guard_release(__guard* g)
{
    // This is not thread safe. Since we're only operating with single threads
    // at the moment, that doesn't matter. Simply set the variable to
    // initialised, don't bother trying to unlock it.
    *reinterpret_cast<char*>(g) = 1;
}

/******************************************************************************
 ******************************************************************************/

void __cxa_guard_abort(__guard*)
{
    // This is not thread safe. Since we're only operating with single threads
    // at the moment, that doesn't matter. We would need to unlock, but not set
    // the variable to initialised. Without the unlock, we don't need to do
    // anything.
}

/******************************************************************************
 ******************************************************************************/

// Global pointer to the list of destructors. It might be nice to make this a
// local static variable, but that would be a weird cyclic dependency.
NMSP::vector<AtExitFuncEntry>* atexit_func_list = nullptr;

/******************************************************************************
 ******************************************************************************/

int __cxa_atexit(void (*destructor_func)(void*), void* obj_ptr,
    void* dso_handle)
{
    // If this is the first call, the list is unitialised. Let's hope we don't
    // call any functions with local static variables before the heap exists...
    if (atexit_func_list == nullptr)
        atexit_func_list = new NMSP::vector<AtExitFuncEntry>{};

    // Fail here if we didn't manage to create the list.
    if (atexit_func_list == nullptr)
        return -1;

    // Create the new entry.
    atexit_func_list->emplace_back(destructor_func, obj_ptr, dso_handle);

    return 0;
}

/******************************************************************************
 ******************************************************************************/

void __cxa_finalize(void* destructor_func)
{
    // Exit if nothing was ever registered with atexit.
    if (atexit_func_list == nullptr)
        return;

    if (destructor_func != nullptr)
    {
        // Search for the specific destructor.
        for (auto p = atexit_func_list->begin(); p != atexit_func_list->end();
            ++p)
        {
            if (p->destructor_func == destructor_func)
            {
                // Call the destructor.
                p->destructor_func(p->obj_ptr);
                // Delete the entry.
                atexit_func_list->erase(p);
                break;
            }
        }
    }
    else
    {
        // Delete all entries.
        for (const AtExitFuncEntry& p : *atexit_func_list)
        {
            p.destructor_func(p.obj_ptr);
        }
        // Clear the vector.
        delete atexit_func_list;
        atexit_func_list = nullptr;
    }
}

/******************************************************************************
 ******************************************************************************/

// I'm really not sure why I need this, but the linker complains if it doesn't
// exist.
void* __dso_handle;

/******************************************************************************
 ******************************************************************************/

__fundamental_type_info::~__fundamental_type_info() {}

/******************************************************************************
 ******************************************************************************/

__class_type_info::~__class_type_info() {}

/******************************************************************************/

__class_type_info::sub_kind __class_type_info::find_public_src(
        ptrdiff_t src2dest, const void* obj_ptr,
        const __class_type_info* src_type, const void* src_ptr) const
{
    if (src2dest >= 0)
        return (helper::adjust_pointer<void>(obj_ptr, src2dest) == src_ptr ?
            contained_public : not_contained);

    if (src2dest == -2)
        return not_contained;

    return do_find_public_src(src2dest, obj_ptr, src_type, src_ptr);
}

/******************************************************************************/

__class_type_info::sub_kind __class_type_info::do_find_public_src(ptrdiff_t,
    const void* obj_ptr, const __class_type_info*, const void* src_ptr) const
{

    if (src_ptr == obj_ptr)
        // Pointers match, so our type is definitely derived from the source
        // type.
        return contained_public;

    return not_contained;
}

/******************************************************************************/

bool __class_type_info::do_dyncast(ptrdiff_t, sub_kind access_path,
        const __class_type_info* dest_type, const void* obj_ptr,
        const __class_type_info* src_type, const void* src_ptr,
        dyncast_result& result) const
{
    if (obj_ptr == src_ptr && *this == *src_type)
    {
        // Same object of same type.
        result.whole2src = access_path;
        return false;
    }

    if (*this == *dest_type)
    {
        result.dest_ptr = obj_ptr;
        result.whole2dest = access_path;
        result.dest2src = not_contained;
        return false;
    }

    return false;
}

/******************************************************************************/

bool __class_type_info::__do_upcast(const __class_type_info* dest_type,
    void** obj_ptr) const
{
    upcast_result result {__vmi_class_type_info::flags_unknown_mask};

    __do_upcast(dest_type, *obj_ptr, result);

    if (!helper::contained_public_p(result.part2dest))
        return false;

    *obj_ptr = const_cast<void*>(result.dest_ptr);
    return true;
}

bool __class_type_info::__do_upcast(const __class_type_info* dest_type,
        const void* obj, upcast_result& result) const
{
    if (*this == *dest_type)
    {
        result.dest_ptr = obj;
        result.base_type = helper::nonvirtual_base_type;
        result.part2dest = contained_public;
        return true;
    }

    return false;
}

/******************************************************************************/

bool __class_type_info::__do_catch(const std::type_info* throw_type,
        void** throw_obj, size_t outer) const
{
    if (*this == *throw_type)
        return true;

    if (outer >= 4)
        return false;

    return throw_type->__do_upcast(this, throw_obj);
}

/******************************************************************************
 ******************************************************************************/

__si_class_type_info::~__si_class_type_info() {}

/******************************************************************************/

bool __si_class_type_info::__do_upcast(const __class_type_info* dest_type,
    const void* obj, upcast_result& result) const
{
    // True if we already match the type.
    if (__class_type_info::__do_upcast(dest_type, obj, result))
        return true;

    // Otherwise check the base type.
    return base_type->__do_upcast(dest_type, obj, result);
}

/******************************************************************************/

__class_type_info::sub_kind __si_class_type_info::do_find_public_src(
        ptrdiff_t src2dest, const void* obj_ptr,
        const __class_type_info* src_type, const void* src_ptr) const
{
    // Test if we already match the type.
    if (src_ptr == obj_ptr && *this == *src_type)
        return contained_public;

    // Otherwise chck the base type.
    return base_type->do_find_public_src(src2dest, obj_ptr, src_type, src_ptr);
}

/******************************************************************************/

bool __si_class_type_info::do_dyncast(ptrdiff_t src2dest, sub_kind access_path,
        const __class_type_info* dest_type, const void* obj_ptr,
        const __class_type_info* src_type, const void* src_ptr,
        dyncast_result& result) const
{
    if (*this == *dest_type)
    {
        result.dest_ptr = obj_ptr;
        result.whole2dest = access_path;
        if (src2dest >= 0)
            result.dest2src = (helper::adjust_pointer<void>(obj_ptr, src2dest)
                == src_ptr ? contained_public : not_contained);
        else if (src2dest == -2)
            result.dest2src = not_contained;
        return false;
    }

    if (obj_ptr == src_ptr && *this == *src_type)
    {
        result.whole2src = access_path;
        return false;
    }

    return base_type->do_dyncast(src2dest, access_path, dest_type, obj_ptr,
        src_type, src_ptr, result);
}

/******************************************************************************
 ******************************************************************************/

__vmi_class_type_info::~__vmi_class_type_info() {}

/******************************************************************************/

bool __vmi_class_type_info::__do_upcast(const __class_type_info* dest_type,
        const void* obj, upcast_result& result) const
{
    // I'm not even going to pretend I know what's happening here. Just copying
    // from the GCC 7.2.0 source.

    if (__class_type_info::__do_upcast(dest_type, obj, result))
        return true;

    int src_details = result.src_details;
    if (src_details & flags_unknown_mask)
        src_details = flags;

    for (size_t i = base_count; i--; )
    {
        upcast_result result2 {src_details};
        const void* base = obj;
        ptrdiff_t offset = base_info[i].offset();
        bool is_virtual = base_info[i].is_virtual();
        bool is_public = base_info[i].is_public();

        if (!is_public && !(src_details & non_diamond_repeat_mask))
            continue;

        if (base)
            base = helper::convert_to_base(base, is_virtual, offset);

        if (base_info[i].base_type->__do_upcast(dest_type, base, result2))
        {
            if (result2.base_type == helper::nonvirtual_base_type && is_virtual)
                result2.base_type = base_info[i].base_type;

            if (helper::contained_p(result.part2dest) && !is_public)
                result2.part2dest = static_cast<sub_kind>(
                result2.part2dest & ~contained_public_mask);

            if (!result.base_type)
            {
                result = result2;
                if (!helper::contained_p(result.part2dest))
                    return true;

                if (result.part2dest & contained_public_mask)
                {
                    if (!(flags & non_diamond_repeat_mask))
                        return true;
                }
                else
                {
                    if (!helper::virtual_p(result.part2dest))
                        return true;
                    if (!(flags & diamond_shape_mask))
                        return true;
                }
            }
            else if (result.dest_ptr != result2.dest_ptr)
            {
                result.dest_ptr = nullptr;
                result.part2dest = contained_ambig;
                return true;
            }
            else if (result.dest_ptr)
                result.part2dest =
                    static_cast<sub_kind>(result.part2dest | result2.part2dest);
            else
            {
                if (result2.base_type == helper::nonvirtual_base_type ||
                    result.base_type == helper::nonvirtual_base_type ||
                    !(*result2.base_type == *result.base_type))
                {
                    result.part2dest = contained_ambig;
                    return true;
                }
                result.part2dest =
                    static_cast<sub_kind>(result.part2dest | result2.part2dest);
            }
        }
    }

    return (result.part2dest != unknown);
}

/******************************************************************************/

__class_type_info::sub_kind __vmi_class_type_info::do_find_public_src(
        ptrdiff_t src2dest, const void* obj_ptr,
        const __class_type_info* src_type, const void* src_ptr) const
{
    // I'm not even going to pretend I know what's happening here. Just copying
    // from the GCC 7.2.0 source.

    if (obj_ptr == src_ptr && *this == *src_type)
        return contained_public;

    for (size_t i = base_count; i--; )
    {
        if (!base_info[i].is_public())
            // Not a public base.
            continue;

        const void* base = obj_ptr;
        ptrdiff_t offset = base_info[i].offset();
        bool is_virtual = base_info[i].is_virtual();

        if (is_virtual)
        {
            if (src2dest == -3)
                // Not a virtual base.
                continue;
        }

        base = helper::convert_to_base(base, is_virtual, offset);

        sub_kind base_kind = base_info[i].base_type->
            do_find_public_src(src2dest, base, src_type, src_ptr);
        if (helper::contained_p(base_kind))
        {
            if (is_virtual)
                base_kind = static_cast<sub_kind>(
                    base_kind | contained_virtual_mask);
            return base_kind;
        }
    }

    return not_contained;
}

/******************************************************************************/

bool __vmi_class_type_info::do_dyncast(ptrdiff_t src2dest, sub_kind access_path,
    const __class_type_info* dest_type, const void* obj_ptr,
    const __class_type_info* src_type, const void* src_ptr,
    dyncast_result& result) const
{
    // I'm really not following what's happening here, just copying from the
    // GCC 7.2.0 source. Even that admits this is a rather complicated function.

    if (result.whole_details & flags_unknown_mask)
        result.whole_details = flags;

    if (obj_ptr == src_ptr && *this == *src_type)
    {
        result.whole2src = access_path;
        return false;
    }

    if (*this == *dest_type)
    {
        result.dest_ptr = obj_ptr;
        result.whole2dest = access_path;
        if (src2dest >= 0)
            result.dest2src =
                (helper::adjust_pointer<void>(obj_ptr, src2dest) == src_ptr ?
                contained_public : not_contained);
        else if (src2dest == -2)
            result.dest2src = not_contained;
        return false;
    }

    const void* dest_cand = nullptr;
    if (src2dest >= 0)
        dest_cand = helper::adjust_pointer<void>(src_ptr, -src2dest);
    bool first_pass = true;
    bool skipped = false;
    bool result_ambig = false;

    // GOTO label
    again:

    for (size_t i = base_count; i--; )
    {
        dyncast_result result2 {result.whole_details};
        const void* base = obj_ptr;
        sub_kind base_access = access_path;
        ptrdiff_t offset = base_info[i].offset();
        bool is_virtual = base_info[i].is_virtual();

        if (is_virtual)
            base_access = static_cast<sub_kind>(
                base_access | contained_virtual_mask);
        base = helper::convert_to_base(base, is_virtual, offset);

        if (dest_cand)
        {
            bool skip_on_first_pass = base > dest_cand;
            if (skip_on_first_pass == first_pass)
            {
                skipped = true;
                continue;
            }
        }

        if (!base_info[i].is_public())
        {
            if (src2dest == -2 && !(result.whole_details &
                (non_diamond_repeat_mask | diamond_shape_mask)))
                continue;
            base_access =
                static_cast<sub_kind>(base_access & ~contained_public_mask);
        }

        bool result2_ambig = base_info[i].base_type->do_dyncast(src2dest,
            base_access, dest_type, base, src_type, src_ptr, result2);
        result.whole2src =
            static_cast<sub_kind>( result.whole2src | result2.whole2src);

        if (result2.dest2src == contained_public ||
            result2.dest2src == contained_ambig)
        {
            result.dest_ptr = result2.dest_ptr;
            result.whole2dest = result2.whole2dest;
            result.dest2src = result2.dest2src;
            return result2_ambig;
        }

        if (!result_ambig && !result.dest_ptr)
        {
            result.dest_ptr = result2.dest_ptr;
            result.whole2dest = result2.whole2dest;
            result_ambig = result2_ambig;
            if (result.dest_ptr && result.whole2src != unknown &&
                !(flags & non_diamond_repeat_mask))
            return result_ambig;
        }
        else if (result.dest_ptr && result.dest_ptr == result2.dest_ptr)
            result.whole2dest =
                static_cast<sub_kind>(result.whole2dest & result2.whole2dest);
        else if ((result.dest_ptr != nullptr && result2.dest_ptr != nullptr) ||
            (result.dest_ptr != nullptr && result2_ambig) ||
            (result2.dest_ptr != nullptr && result_ambig))
        {
            sub_kind new_sub_kind = result2.dest2src;
            sub_kind old_sub_kind = result.dest2src;

            if (helper::contained_p(result.whole2src) &&
                (!helper::virtual_p(result.whole2src) ||
                !(result.whole_details & diamond_shape_mask)))
            {
                if (old_sub_kind == unknown)
                    old_sub_kind = not_contained;
                if (new_sub_kind == unknown)
                    new_sub_kind = not_contained;
            }
            else
            {
                if (old_sub_kind >= not_contained) ;
                else if (helper::contained_p(new_sub_kind) &&
                    (!helper::virtual_p(new_sub_kind) ||
                    !(flags & diamond_shape_mask)))
                    old_sub_kind = not_contained;
                else
                    old_sub_kind = dest_type->find_public_src(src2dest,
                        result.dest_ptr, src_type, src_ptr);

                if (new_sub_kind >= not_contained) ;
                else if (helper::contained_p(old_sub_kind) &&
                    (!helper::virtual_p(old_sub_kind) ||
                    !(flags & diamond_shape_mask)))
                    new_sub_kind = not_contained;
                else
                    new_sub_kind = dest_type->find_public_src(src2dest,
                        result2.dest_ptr, src_type, src_ptr);
            }

            if (helper::contained_p(static_cast<sub_kind>(
                new_sub_kind ^ old_sub_kind)))
            {
                if (helper::contained_p(new_sub_kind))
                {
                    result.dest_ptr = result2.dest_ptr;
                    result.whole2dest = result2.whole2dest;
                    result_ambig = false;
                    old_sub_kind = new_sub_kind;
                }

                result.dest2src = old_sub_kind;

                if (helper::public_p(result.dest2src))
                    return false;
                if (!helper::virtual_p(result.dest2src))
                    return false;
            }
            else if (helper::contained_p(
                static_cast<sub_kind>(new_sub_kind & old_sub_kind)))
            {
                result.dest_ptr = nullptr;
                result.dest2src = contained_ambig;
                return true;
            }
            else
            {
                result.dest_ptr = nullptr;
                result.dest2src = not_contained;
                result_ambig = true;
            }
        }

        if (result.whole2src == contained_private)
            return result_ambig;
    }

    if (skipped && first_pass)
    {
        first_pass = false;
        goto again;
    }

    return result_ambig;
}

/******************************************************************************
 ******************************************************************************/

__pbase_type_info::~__pbase_type_info() {}

/******************************************************************************/

bool __pbase_type_info::__do_catch(const std::type_info* throw_type,
    void** throw_obj, size_t outer) const
{
    if (*this == *throw_type)
        // Same type, simple.
        return true;

    // Test for nullptr.
    if (*throw_type == typeid(nullptr))
    {
        // Any pointer type matches nullptr_t.
        if (typeid(*this) == typeid(__pointer_type_info))
        {
            *throw_obj = nullptr;
            return true;
        }
        // Pointer to member rather than simple pointer.
        else if (typeid(*this) == typeid(__pointer_to_member_type_info))
        {
            // Test for pointer to member function.
            if (pointee->__is_function_p())
            {
                using pmf_type = void (__pbase_type_info::*)();
                static const pmf_type pmf = nullptr;
                *throw_obj = const_cast<pmf_type*>(&pmf);
                return true;
            }
            // Pointer to member data.
            else
            {
                using pm_type = int __pbase_type_info::*;
                static const pm_type pm = nullptr;
                *throw_obj = const_cast<pm_type*>(&pm);
                return true;
            }
        }
    }

    if (!(outer & 1))
        // Not the same type. Something about the outer type not being const
        // qualified.
        return false;

    // Convert the thrown type info from generic to pointer base type.
    const __pbase_type_info* t_type = static_cast<const __pbase_type_info*>
        (throw_type);

    // Start checking the qualifiers.
    unsigned int t_flags = t_type->flags;
    const unsigned int qual_mask = transaction_safe_mask | noexcept_mask;
    unsigned int throw_qual = (t_flags & qual_mask);
    unsigned int catch_qual = (flags * qual_mask);

    if (throw_qual & ~catch_qual)
        // CFunction pointer conversion is possible.
        t_flags &= catch_qual;
    if (catch_qual & ~throw_qual)
        // Conversion is not possible.
        return false;

    if (t_flags & ~flags)
        // Conversion not possible.
        return false;

    if (!(t_flags & const_mask))
        // Wtf?
        outer &= ~1;

    // Refer on to the pointed to type.
    return pointer_catch(t_type, throw_obj, outer);
}

/******************************************************************************/

bool __pbase_type_info::pointer_catch(const __pbase_type_info* throw_type,
    void** throw_obj, unsigned int outer) const
{
    return pointee->__do_catch(throw_type->pointee, throw_obj, outer + 2);
}

/******************************************************************************
 ******************************************************************************/

__pointer_type_info::~__pointer_type_info() {}

/******************************************************************************/

bool __pointer_type_info::__is_pointer_p() const
{
    return true;
}

/******************************************************************************/

bool __pointer_type_info::pointer_catch(const __pbase_type_info* throw_type,
    void** throw_obj, unsigned int outer) const
{
    // Check for void.
    if (outer < 2 && *pointee == typeid(void))
        return !throw_type->pointee->__is_function_p();

    // Refer on to the pointed to type.
    return __pbase_type_info::pointer_catch(throw_type, throw_obj, outer);
}

/******************************************************************************
 ******************************************************************************/

__pointer_to_member_type_info::~__pointer_to_member_type_info() {}

/******************************************************************************/

bool __pointer_to_member_type_info::pointer_catch(
    const __pbase_type_info* throw_type, void** throw_obj, unsigned int outer)
    const
{
    // Static cast is valid as the caller has already determined the thrown type
    // is a pointer to member type.
    const __pointer_to_member_type_info* t_type =
        static_cast<const __pointer_to_member_type_info*>(throw_type);

    // Simple if not pointers to the same class.
    if (*context != *t_type->context)
        return false;

    // Pass on to the pointed to type.
    return __pbase_type_info::pointer_catch(t_type, throw_obj, outer);
}

/******************************************************************************
 ******************************************************************************/

extern "C"
void* __dynamic_cast(const void* src_ptr, const __class_type_info* src_type,
    const __class_type_info* dest_type, ptrdiff_t src2dest)
{
    // The pointer to the vtable of the source object. This assumes the start of
    // the object is the vtable pointer.
    const void* vtable = *static_cast<const void* const *>(src_ptr);

    // The vtable prefix comes before the vtable. Adjust backwards by the size
    // of the prefix.
    const helper::vtable_prefix* prefix =
        helper::adjust_pointer<helper::vtable_prefix>(vtable,
        -offsetof(helper::vtable_prefix, origin));

    // The vtable prefix contains the pointer to the most derived type object.
    const void* whole_ptr =
        helper::adjust_pointer<void>(src_ptr, prefix->whole_object);

    // The vtable prefix also has the pointer to the type information for the
    // most derived type of the object.
    const __class_type_info* whole_type = prefix->whole_type;

    // Construct the result.
    __class_type_info::dyncast_result result;

    // Get the vtable for the most derived type.
    const void* whole_vtable = *static_cast<const void* const *>(whole_ptr);

    // Get the vtable prefix for the most derived type.
    const helper::vtable_prefix* whole_prefix =
        helper::adjust_pointer<helper::vtable_prefix>(whole_vtable,
        -offsetof(helper::vtable_prefix, origin));

    // If we're in the middle of constructing the bases, but haven't got to the
    // constructing the most derived type, the most derived types in the
    // different vtables might not match. Fail if this is so.
    if (whole_prefix->whole_type != whole_type)
        return nullptr;

    // Call the dynamic cast function for the whole type.
    whole_type->do_dyncast(src2dest, __class_type_info::contained_public,
        dest_type, whole_ptr, src_type, src_ptr, result);

    // Check for failure.
    if (result.dest_ptr == nullptr)
        return nullptr;

    if (helper::contained_public_p(result.dest2src))
        // src is a public base of dest, so this is a valid downcast.
        return const_cast<void*>(result.dest_ptr);

    if (helper::contained_public_p(static_cast<__class_type_info::sub_kind>
        (result.whole2src & result.whole2dest)))
        // Both src and dest are public bases of the whole, so this is a valid
        // cross cast.
        return const_cast<void*>(result.dest_ptr);

    if (helper::contained_nonvirtual_p(result.whole2src))
        // src is a non-public non-virtual base of whole and not a base of dest.
        // This is an invalid cross case.
        return nullptr;

    if (result.dest2src == __class_type_info::unknown)
        // We haven't found the relation. Use the helper function to check.
        result.dest2src = dest_type->find_public_src(src2dest, result.dest_ptr,
            src_type, src_ptr);

    if (helper::contained_public_p(result.dest2src))
        // src is a public base of dest, so this is a valid downcast.
        return const_cast<void*>(result.dest_ptr);

    // Nothing valid found.
    return nullptr;
}

/******************************************************************************
 ******************************************************************************/

extern "C"
void __cxa_bad_typeid()
{
    throw NMSP::bad_typeid {};
}

/******************************************************************************
 ******************************************************************************/

extern "C"
void __cxa_bad_cast()
{
    throw NMSP::bad_cast {};
}

/******************************************************************************
 ******************************************************************************/

extern "C"
void __cxa_throw_bad_array_new_length()
{
    throw NMSP::bad_array_new_length {};
}

/******************************************************************************
 ******************************************************************************/

namespace helper {

/******************************************************************************
 ******************************************************************************/

const void* convert_to_base(const void* addr, bool is_virtual, ptrdiff_t offset)
{
    if (is_virtual)
    {
        const void* vtable = *static_cast<const void* const*>(addr);
        offset = *adjust_pointer<ptrdiff_t>(vtable, offset);
    }

    return adjust_pointer<void>(addr, offset);
}

/******************************************************************************
 ******************************************************************************/

bool contained_public_p(__class_type_info::sub_kind access_path)
{
    return ((access_path & __class_type_info::contained_public) ==
        __class_type_info::contained_public);
}

/******************************************************************************/

bool public_p(__class_type_info::sub_kind access_path)
{
    return access_path & __class_type_info::contained_public_mask;
}

/******************************************************************************/

bool contained_nonvirtual_p(__class_type_info::sub_kind access_path)
{
    return ((access_path & (__class_type_info::contained_mask |
        __class_type_info::contained_virtual_mask)) ==
        __class_type_info::contained_mask);
}

/******************************************************************************/

bool contained_p(__class_type_info::sub_kind access_path)
{
    return access_path >= __class_type_info::contained_mask;
}

/******************************************************************************/

bool virtual_p(__class_type_info::sub_kind access_path)
{
    return (access_path & __class_type_info::contained_virtual_mask);
}

/******************************************************************************
 ******************************************************************************/

} // end helper namespace

/******************************************************************************
 ******************************************************************************/

static constexpr size_t exception_buf_size = 256;
static NMSP::array<char, exception_buf_size> exception_buf;
static _Unwind_Exception_Class exc_class =
//    static_cast<_Unwind_Exception_Class>('G') |
//    (static_cast<_Unwind_Exception_Class>('N')) << 8 |
//    (static_cast<_Unwind_Exception_Class>('U')) << 8 | 
//    (static_cast<_Unwind_Exception_Class>('C')) << 8 | 
//    (static_cast<_Unwind_Exception_Class>('C')) << 8 | 
//    (static_cast<_Unwind_Exception_Class>('+')) << 8 | 
//    (static_cast<_Unwind_Exception_Class>('+')) << 8 | 
//    (static_cast<_Unwind_Exception_Class>('\0')) << 8;
    static_cast<_Unwind_Exception_Class>('\0') |
    (static_cast<_Unwind_Exception_Class>('+')) << 8 |
    (static_cast<_Unwind_Exception_Class>('+')) << 8 | 
    (static_cast<_Unwind_Exception_Class>('C')) << 8 | 
    (static_cast<_Unwind_Exception_Class>('C')) << 8 | 
    (static_cast<_Unwind_Exception_Class>('U')) << 8 | 
    (static_cast<_Unwind_Exception_Class>('N')) << 8 | 
    (static_cast<_Unwind_Exception_Class>('G')) << 8;

/******************************************************************************
 ******************************************************************************/

namespace helper {

// A set of functions for adjusting the pointers to the different types of
// exception headers and objects.

/******************************************************************************/

/**
    Given a pointer to the exception object, get the pointer to the ABI header.

    @param obj Pointer to exception object.
    @return Pointer to ABI header.
 */
static __cxa_exception* abi_header_from_obj(void* obj)
{
    return static_cast<__cxa_exception*>(obj) - 1;
}

/******************************************************************************/

/**
    Given a pointer to the ABI header, get the pointer to the exception object.

    @param exc Pointer to ABI header.
    @return Pointer to exception object.
 */
static void* obj_from_abi_header(__cxa_exception* exc)
{
    return exc + 1;
}

/******************************************************************************/

/**
    Given a pointer to the internal header, get the pointer to the ABI header.

    @param ue Pointer to internal header.
    @return Pointer to ABI header.
 */
static __cxa_exception* abi_header_from_internal(_Unwind_Exception* ue)
{
    return reinterpret_cast<__cxa_exception*>(ue + 1) - 1;
}

/******************************************************************************/

/**
    Given a pointer to the ABI header, get the pointer to the internal header.

    @param exc Pointer to the ABI header.
    @return Pointer to internal header.
 */
static _Unwind_Exception* internal_from_abi_header(__cxa_exception* exc)
{
    return &exc->unwindHeader;
}

/******************************************************************************/

/**
    Given a pointer to the internal header, get the pointer to the exception
    object.

    @param ue Pointer to internal header.
    @return Pointer to exception object.
 */
static void* obj_from_internal(_Unwind_Exception* ue)
{
    return static_cast<void*>(ue + 1);
}

/******************************************************************************/

/**
    Given a pointer to the exception, get the pointer to the internal header.

    @param obj Pointer to exception object.
    @return Pointer to internal header.
 */
/*static _Unwind_Exception* internal_from_obj(void* obj)
{
    return static_cast<_Unwind_Exception*>(obj) - 1;
}*/

/******************************************************************************
 ******************************************************************************/

/**
    This is the function called to clean up the exception at the end of the
    handling by libgcc. We set it to be so by setting the cleanup field of the
    internal heading point at this function. It must check its been called for
    the right reason, then call the exception destructor and deallocate the
    space.

    @param code Action code from libgcc.
    @param ue Pointer to the internal header.
 */
static void exception_cleanup(_Unwind_Reason_Code code, _Unwind_Exception* ue)
{
//    NMSP::printf("helper::exception_cleanup\n");
    __cxa_exception* exc = helper::abi_header_from_internal(ue);

    // Terminate if this function was called for the wrong reasons.
    if (code != _URC_FOREIGN_EXCEPTION_CAUGHT && code != _URC_NO_REASON)
        exc->terminateHandler();

    // Call the exception destructor, unless it's nullptr.
    if (exc->exceptionDestructor != nullptr)
        exc->exceptionDestructor(helper::obj_from_abi_header(exc));

    // Free the memory. This expects a pointer to the exception object.
    __cxa_free_exception(helper::obj_from_abi_header(exc));
} 

} // end helper namespace

/******************************************************************************
 ******************************************************************************/

extern "C"
void* __cxa_allocate_exception(size_t thrown_size) noexcept
{
//    NMSP::printf("__cxa_allocate_exception\n");
    void* ret = nullptr;

    // Add on space for the ABI and internal header.
    thrown_size += sizeof(__cxa_exception);

    // Try to allocate on the heap.
    char* alloc_space = new char[thrown_size];

    if (alloc_space != nullptr)
        ret = alloc_space;

    // If we couldn't heap allocate, try to use the static buffer.
    else if (thrown_size > exception_buf_size)
        // Big trouble
        std::terminate();
    else
        ret = exception_buf.elem;

    // Construct the exception header.
    __cxa_exception* exc = new (ret) __cxa_exception {};

    // Offset the return value to point at the start of the exception object.
    return helper::obj_from_abi_header(exc);
}

/******************************************************************************
 ******************************************************************************/

extern "C"
void __cxa_free_exception(void* thrown_exception) noexcept
{
//    NMSP::printf("__cxa_free_exception\n");

    // The pointer is to the exception object. Adjust back to include the
    // header.
    char* base =
        reinterpret_cast<char*>(helper::abi_header_from_obj(thrown_exception));

    // Only free if the exception was not allocated in the static buffer. Note
    // that this will not call the exception destructor. That's done in the
    // clean up function that calls this one.
    if (base != exception_buf.elem)
        delete[] base;
}

/******************************************************************************
 ******************************************************************************/

extern "C"
void __cxa_throw(void* thrown_exception, std::type_info* tinfo,
    void (*dest)(void*))
{
//    NMSP::printf("__cxa_throw\n", thrown_exception);

    // Set up the exception header.
    __cxa_exception* exc = helper::abi_header_from_obj(thrown_exception);
    exc->exceptionType = tinfo;
    exc->exceptionDestructor = dest;
    exc->unexpextedHandler = NMSP::get_unexpected();
    exc->terminateHandler = NMSP::get_terminate();

    // Set up the internal header.
    exc->unwindHeader.exception_cleanup = helper::exception_cleanup;
    exc->unwindHeader.exception_class = exc_class;

    // Update the global information.
    __cxa_eh_globals* eh = __cxa_get_globals();
    ++(eh->uncaughtExceptions);

    _Unwind_RaiseException(&exc->unwindHeader);

    // If we get here, no one caught the exception. Terminate.
//    NMSP::printf("__cxa_throw: no catch\n");
    std::terminate();
}

/******************************************************************************
 ******************************************************************************/

extern "C"
void __cxa_rethrow()
{
    // Get pointers to the required objects.
    __cxa_eh_globals* eh = __cxa_get_globals();
    __cxa_exception* exc = eh->caughtExceptions;

    // Treat the currently handled exception as uncaught again.
    ++(eh->uncaughtExceptions);

    // Terminate if there is no active exception to rethrow.
    if (exc == nullptr)
        std::terminate();

    // Indicate this exception is being rethrown by negating its handler count.
    exc->handlerCount *= -1;

    // Resume unwinding.
    _Unwind_Resume_or_Rethrow(helper::internal_from_abi_header(exc));

    // SOme sort of unwinding error if we get here.
    __cxa_begin_catch(helper::internal_from_abi_header(exc));
}

/******************************************************************************
 ******************************************************************************/

extern "C"
void* __cxa_get_exception_ptr(void* uexp)
{
    _Unwind_Exception* ue = static_cast<_Unwind_Exception*>(uexp);
    __cxa_exception* exc = helper::abi_header_from_internal(ue);
    return exc->adjustedPtr;
}

/******************************************************************************
 ******************************************************************************/

extern "C"
void* __cxa_begin_catch(void* uexp)
{
//    NMSP::printf("__cxa_begin_catch\n");

    // Get pointers to all the relevant objects.
    _Unwind_Exception* ue = static_cast<_Unwind_Exception*>(uexp);
    __cxa_exception* exc = helper::abi_header_from_internal(ue);
    __cxa_eh_globals* eh = __cxa_get_globals();
    __cxa_exception* prev = eh->caughtExceptions;

    // Check the count.
    int count = exc->handlerCount;
    if (count < 0)
        // Indicates the exception was rethrown from an immediately enclosing
        // region.
        count = 1 - count;
    else
        ++count;
    exc->handlerCount = count;
    --(eh->uncaughtExceptions);

    // Update the stack of caught exceptions.
    if (exc != prev)
    {
        exc->nextException = prev;
        eh->caughtExceptions = exc;
    }

    // Returns the pointer to the exception object.
    return exc->adjustedPtr;
}

/******************************************************************************
 ******************************************************************************/

extern "C"
void __cxa_end_catch()
{
//    NMSP::printf("__cxa_end_catch\n");

    // Get pointers to the necessary objects.
    __cxa_eh_globals* eh = __cxa_get_globals_fast();
    __cxa_exception* exc = eh->caughtExceptions;

    // Check the count.
    int count = exc->handlerCount;
    if (count < 0)
    {
        // Indicates a rethrown exception. Decrement the count (towards zero)
        // and remove from the stack if zero.
        if (++count == 0)
            eh->caughtExceptions = exc->nextException;
    }
    // Otherwise, decrement the count (towards zero) and destroy if zero.
    else if (--count == 0)
    {
        eh->caughtExceptions = exc->nextException;
        _Unwind_DeleteException(helper::internal_from_abi_header(exc));
        return;
    }

    exc->handlerCount = count;
}

/******************************************************************************
 ******************************************************************************/

namespace helper {

/******************************************************************************
 ******************************************************************************/

int read_SLEB128(const uint8_t* p)
{
    size_t off;
    return read_SLEB128(p, off);
}

int read_SLEB128(const uint8_t* p, size_t& off)
{
    uintptr_t result = 0;
    size_t shift = 0;
    unsigned char byte;
    off = 0;

    do
    {
        byte = *p++;
        ++off;
        result |= (static_cast<uintptr_t>(byte & 0x7F) << shift);
        shift += 7;
    } while (byte & 0x80);

    if ((byte & 0x40) && (shift < (sizeof(result) << 3)))
        result |= static_cast<uintptr_t>(~0) << shift;

    return static_cast<int>(result);
}

/******************************************************************************/

size_t read_ULEB128(const uint8_t* p)
{
    size_t off;
    return read_ULEB128(p, off);
}

size_t read_ULEB128(const uint8_t* p, size_t& off)
{
    size_t result = 0;
    size_t shift = 0;
    unsigned char byte;
    off = 0;

    while (true)
    {
        byte = *p++;
        ++off;
        result |= (static_cast<size_t>(byte & 0x7F) << shift);
        if ((byte & 0x80) == 0)
            break;
        shift += 7;
    }

    return result;
}

/******************************************************************************
 ******************************************************************************/

/**
    Installs a landing pad with the correct register settings.

    @param ue Pointer to the internal exception header. Used for rethrowing.
    @param context Stack frame context.
    @param type_index Entry in the types table for this catch.
    @param lp_addr Address of the landing pad.
    @return Code indicating installation of the context.
 */

static _Unwind_Reason_Code load_landing_pad(_Unwind_Exception* ue,
    _Unwind_Context* context, int type_index, uintptr_t lp_addr)
{
    int r0 = __builtin_eh_return_data_regno(0);
    int r1 = __builtin_eh_return_data_regno(1);

    _Unwind_SetGR(context, r0, reinterpret_cast<uintptr_t>(ue));
    _Unwind_SetGR(context, r1, type_index);
    _Unwind_SetIP(context, lp_addr);

    return _URC_INSTALL_CONTEXT;
}

/******************************************************************************
 ******************************************************************************/

/**
    Tests whether the type info of a catch matches the thrown type, including
    dealing with inheritance and a pointer dereference.

    @param catch_type Type info for the catch specification.
    @param throw_type Type info for the thrown type.
    @param throw_ptr Pointer to pointer to the exception. In the cases of
           multiple inheritance, this will get updated to the location of the
           catch_type exception containing throw_ptr.
    @return True if the types match, false otherwise.
 */
static bool can_handle(const std::type_info* catch_type,
    const std::type_info* throw_type, void** throw_ptr)
{
    void* tmp_throw_ptr = *throw_ptr;

    // If a pointer to an exception was thrown, dereference.
    if (throw_type->__is_pointer_p())
        tmp_throw_ptr = *reinterpret_cast<void**>(throw_ptr);

    // Test the types.
    if (catch_type->__do_catch(throw_type, &tmp_throw_ptr, 1))
    {
        // Update the exception address.
        *throw_ptr = tmp_throw_ptr;
        return true;
    }

    return false;
}

/******************************************************************************
 ******************************************************************************/

} // end helper namespace

/******************************************************************************
 ******************************************************************************/

extern "C"
_Unwind_Reason_Code __gxx_personality_v0(int version, _Unwind_Action actions,
    uint64_t exception_class, _Unwind_Exception* ue, _Unwind_Context* context)
{
    (void)version; (void)exception_class;

//    NMSP::printf("__gxx_personality_v0: searching for handler\n");

    // Get the original exception type information.
    __cxa_exception* exc = helper::abi_header_from_internal(ue);
    const std::type_info* throw_type = exc->exceptionType;

    // Get the pointer to the start of the stack frame.
    uintptr_t func_start = _Unwind_GetRegionStart(context);

    // Get the instruction that threw the exception (minus 1 since %eip
    // points to the next instruction).
    uintptr_t throw_ip = _Unwind_GetIP(context) - 1;
//    NMSP::printf("__gxx_personality_v0: throw_ip = %p\n", throw_ip);

    // Get the pointer to the start of the LSDA.
    const uint8_t* lsda = reinterpret_cast<uint8_t*>(
        _Unwind_GetLanguageSpecificData(context));

    // Create the LSDA header info.
    LsdaHeaderInfo info {lsda, func_start};

    // Loop over entries in the call site table.
    for (LsdaCsEntry& cs = info.next_cs_entry(); cs.valid();
        cs = info.next_cs_entry())
    {
        // The cs entry can't handle anything if it doesn't have a landing pad.
        if (!cs.has_landing_pad())
            continue;

        // Check whether the try block for this cs entry contains the
        // instruction that threw the exception.
        if (!cs.contains_ip(throw_ip))
            continue;

        // Check for no action, which means cleanup.
        if (!cs.has_action() && actions & _UA_CLEANUP_PHASE)
        {
            // Cleanup action. Run it.
//            NMSP::printf("__gxx_personality_v0: running cleanup action\n");
            return helper::load_landing_pad(ue, context, 0, cs.get_lp());
        }

//        NMSP::printf("__gxx_personality_v0: found CS with lp and correct ip\n");

        // Iterate over the action table entries for this cs.
        for (LsdaActionEntry& action = cs.get_next_action();
            action.valid(); action = cs.get_next_action())
        {
//            NMSP::printf("__gxx_personality_v0: considering valid action...\n");
            if (action.get_type_index() == 0)
            {
//                NMSP::printf("__gxx_personality_v0: action is cleanup\n");
                if (actions & _UA_CLEANUP_PHASE)
                {
                    // There is no type index. This is a cleanup block rather
                    // than a catch statement. Run it.
//                    NMSP::printf("__gxx_personality_v0: running cleanup block\n");
                    return helper::load_landing_pad(ue, context, 0,
                        cs.get_lp());
                }
                else
                    // Don't do anything with cleanup code in the search phase.
                    continue;
            }

//            NMSP::printf("__gxx_personality_v0: fetching catch type info\n");
            // Get the type information for this catch statement.
            const std::type_info* catch_type = action.get_type_info();
//            NMSP::printf("__gxx_personality_v0: catch type info at %p\n", catch_type);

            // Test whether the types match. nullptr indicates all types
            // allowed. throw_ptr may get updated.
            void* throw_ptr = helper::obj_from_internal(ue);
            if (catch_type == nullptr || helper::can_handle(catch_type,
                throw_type, &throw_ptr))
            {
                // If search phase, just return saying a handler was found.
                if (actions & _UA_SEARCH_PHASE)
                {
//                    NMSP::printf("__gxx_personality_v0: search phase found handler\n");
                    exc->adjustedPtr = throw_ptr;
                    return _URC_HANDLER_FOUND;
                }

                // If cleanup phase, run the catch block.
//                NMSP::printf("__gxx_personality_v0: running handler, ue = %p, context = %p, type = %X, lp = %p\n", ue, context, action.get_type_index(), cs.get_lp());
                return helper::load_landing_pad(ue, context,
                    action.get_type_index(), cs.get_lp());
            }
        }
    }

    // If we got here, we didn't find a handler.
//    NMSP::printf("__gxx_personality_v0: no handler found\n");
    return _URC_CONTINUE_UNWIND;
}

/******************************************************************************
 ******************************************************************************/

} // end __cxxabiv1 namespace

/******************************************************************************
 ******************************************************************************/

#endif /* HOSTED_TEST */

