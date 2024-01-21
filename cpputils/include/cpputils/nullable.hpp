#pragma once

#include <type_traits>
#include <concepts>
#include <stdexcept>
#include <memory>

namespace cu
{

template<typename T>
class Nullable
{
#pragma region ____________________________ Types ______________________________

private:
    struct WrapperBase
    {
        constexpr virtual bool isNull() const noexcept
        {
            return false;
        }
    };

    struct WrapperNull : WrapperBase
    {
        constexpr bool isNull() const noexcept override
        {
            return true;
        }
    };

    struct Wrapper : WrapperBase
    {
        T data;

        Wrapper(T data)
            : data{std::forward<T>(data)}
        {
        }
    };

#pragma endregion

#pragma region _________________________ Constructors __________________________

public:
    Nullable()
        : _data{std::make_unique<WrapperNull>()}
    {
    }

    Nullable(T data) requires std::is_move_constructible_v<T> ||
        std::is_copy_constructible_v<T>
        : _data{std::make_unique<Wrapper>(std::forward<T>(data))}
    {
    }

    ~Nullable() = default;

#pragma endregion

#pragma region ________________________ Move Semantics _________________________

public:
    Nullable(Nullable<T>&& other) requires std::is_move_constructible_v<T>
    {
        if (other.isNull())
        {
            setNull();

            return;
        }

        set(std::forward<T>(other.get()));
        other.setNull();
    }

    Nullable<T>& operator=(
        Nullable<T>&& other) requires std::is_move_assignable_v<T>
    {
        if (other.isNull())
        {
            setNull();

            return *this;
        }

        set(std::forward<T>(other.get()));
        other.setNull();

        return *this;
    }

#pragma endregion

#pragma region ________________________ Copy Semantics _________________________

public:
    Nullable(const Nullable<T>& other) requires std::is_copy_constructible_v<T>
    {
        if (other.isNull())
        {
            setNull();

            return;
        }

        set(const_cast<Nullable<T>&>(other).get());
    }

    Nullable<T>& operator=(
        const Nullable<T>& other) requires std::is_copy_assignable_v<T>
    {
        if (other.isNull())
        {
            setNull();

            return *this;
        }

        set(const_cast<Nullable<T>&>(other).get());

        return *this;
    }

#pragma endregion

#pragma region ___________________________ Operators ___________________________

public:

    Nullable<T>& operator=(T data)
    {
        set(std::forward<T>(data));

        return *this;
    }

    bool operator==(const std::remove_cvref_t<T>& data)
    {
        return !isNull() && get() == data;
    }

    bool operator!=(const std::remove_cvref_t<T>& data)
    {
        return !operator==(data);
    }

    bool operator==(const Nullable<T>& other)
    {
        if (isNull() != other.isNull())
            return false;

        if (other.isNull() && isNull())
            return true;

        return get() == const_cast<Nullable<T>&>(other).get();
    }

    bool operator!=(const Nullable<T>& other)
    {
        return !operator==(other);
    }

#pragma endregion

#pragma region ___________________________ Methods _____________________________

public:
    bool isNull() const noexcept
    {
        return _data->isNull();
    }

    void setNull()
    {
        _data = std::make_unique<WrapperNull>();
    }

    void set(T data)
    {
        _data = std::make_unique<Wrapper>(std::forward<T>(data));
    }

    std::remove_reference_t<T>& get()
    {
        if (isNull())
            throw std::runtime_error("Can not access null value.");

        return reinterpret_cast<Wrapper*>(_data.get())->data;
    }

#pragma endregion

#pragma region ____________________________ Fields _____________________________

private:
    std::unique_ptr<WrapperBase> _data;

#pragma endregion

};

}