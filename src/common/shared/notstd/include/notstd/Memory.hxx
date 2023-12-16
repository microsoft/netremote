
#ifndef NOT_STD_MEMORY_HXX
#define NOT_STD_MEMORY_HXX

#include <utility>

namespace notstd
{
/**
 * @brief Enables use of std::make_unique and std::make_shared for types with
 * protected constructors.
 *
 * This is achieved by sub-classing from the base type (BaseT). Since this type
 * derives from the base, it can access all protected members, including the
 * constructor. Thus, the internals of std::make_<unique|shared> can invoke
 * the protected constructor.
 *
 * This is useful for classes where std::enable_shared_from_this() is used where
 * the constructor must be hidden to prevent creation of non shared_ptr
 * instances.
 *
 * @tparam T The type to allow std::make_unique or std::make_shared access to
 * the protected constructor.
 */
template <typename BaseT>
struct enable_make_protected : public BaseT
{
    template <typename... TArgs>
    explicit enable_make_protected(TArgs&&... args) :
        BaseT(std::forward<TArgs>(args)...)
    {}
};

} // namespace notstd

#endif // NOT_STD_MEMORY_HXX
