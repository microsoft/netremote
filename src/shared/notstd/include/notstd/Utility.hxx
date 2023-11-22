
#ifndef NOTSTD_UTILITY_HXX
#define NOTSTD_UTILITY_HXX

#include <concepts>
#include <type_traits>

namespace notstd
{
/**
 * @brief Converts a enumeration value to its underlying type.
 * 
 * The function prototype matches the c++23 function so can be used in its
 * place.
 * 
 * @tparam Enum The enumeration class type to convert.
 * @param e The enumeration class value.
 * @return constexpr std::underlying_type_t<Enum> 
 */
template <typename EnumT>
requires std::is_enum_v<EnumT>
constexpr std::underlying_type_t<EnumT>
to_underlying(EnumT e) noexcept
{
    return static_cast<std::underlying_type_t<EnumT>>(e);
}

} // namespace notstd

#endif // NOTSTD_UTILITY_HXX
