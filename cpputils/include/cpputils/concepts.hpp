#pragma once

#include <concepts>
#include <type_traits>

namespace cu
{

template<typename T>
using Bare = std::remove_cvref_t<T>;

template<typename T>
concept BareDefaultConstructable = std::is_default_constructible_v<Bare<T>>;

template<typename T>
concept DefaultConstructable = std::is_default_constructible_v<T>;

}