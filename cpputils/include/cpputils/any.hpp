#pragma once

#include "nullable.hpp"
#include <typeindex>
#include <memory>
#include <type_traits>
#include <stdexcept>

namespace cu
{

class Any
{
#pragma region ____________________________ Types ______________________________

private:
    struct Wrapper
    {
        std::type_index type;

        Wrapper()
            : type{typeid(void)}
        {
        }

        Wrapper(std::type_index type)
            : type{std::move(type)}
        {
        }

        virtual constexpr bool isValid() const noexcept
        {
            return true;
        }

        template<typename T>
        bool isSameType() const noexcept
        {
            return isValid() && type == std::type_index{typeid(T)};
        }

        virtual std::unique_ptr<Wrapper> clone() const = 0;
    };

    struct InvalidWrapper : public Wrapper
    {
        constexpr bool isValid() const noexcept override
        {
            return false;
        }

        std::unique_ptr<Wrapper> clone() const override
        {
            return std::make_unique<InvalidWrapper>();
        }
    };

    template<typename T>
    struct ValidWrapper : public Wrapper
    {
        T data;

        ValidWrapper(T data)
            : Wrapper(typeid(T))
            , data{std::forward<T>(data)}
        {
        }

        std::unique_ptr<Wrapper> clone() const override
        {
            if constexpr (std::is_copy_constructible_v<T>)
                return std::make_unique<ValidWrapper<T>>(data);
            else
                throw std::runtime_error("Type is not copyable.");
        }
    };

#pragma endregion

#pragma region _________________________ Constructors __________________________

private:
    Any(std::unique_ptr<Wrapper> data)
        : _data{std::move(data)}
    {
    }

public:
    Any() = default;

#pragma endregion

#pragma region ________________________ Move Semantics _________________________

public:
    Any(Any&& other)
    {
        if (!other.isValid())
        {
            reset();

            return;
        }

        _data = std::move(other._data);
        other.reset();
    }

    Any& operator=(Any&& other)
    {
        if (!other.isValid())
        {
            reset();

            return *this;
        }

        _data = std::move(other._data);
        other.reset();

        return *this;
    }

#pragma endregion

#pragma region ________________________ Copy Semantics _________________________

    Any(const Any& other)
    {
        _data = other._data->clone();
    }

    Any& operator=(const Any& other)
    {
        _data = other._data->clone();

        return *this;
    }

#pragma endregion

#pragma region ___________________________ Operators ___________________________

public:
    template<typename T>
    operator T&() &
    {
        return get<T>();
    }

    template<typename T>
    operator T() &&
    {
        T data{std::forward<T>(get<T>())};
        reset();

        return data;
    }

    template<typename T>
    Any& operator=(T t)
    {
        _data = std::make_unique<ValidWrapper<T>>(std::forward<T>(t));

        return *this;
    }

#pragma endregion

#pragma region ____________________________ Static _____________________________

public:
    template<typename T>
    static Any create(T data);

#pragma endregion

#pragma region ___________________________ Methods _____________________________

public:
    bool isValid() const noexcept
    {
        return _data->isValid();
    }

    template<typename T>
    bool isSameType() const noexcept
    {
        return _data->isSameType<T>();
    }

    void reset()
    {
        _data = std::make_unique<InvalidWrapper>();
    }

    template<typename T>
    std::remove_reference_t<T>& get()
    {
        if (!isValid())
            throw std::runtime_error("Data is not valid.");

        if (!isSameType<T>())
            throw std::runtime_error("Not the same type.");

        auto wrapper = reinterpret_cast<ValidWrapper<T>*>(_data.get());

        return reinterpret_cast<T&>(wrapper->data);
    }

    template<typename T>
    void emplace(T data)
    {
        _data = std::make_unique<ValidWrapper<T>>(std::forward<T>(data));
    }

#pragma endregion

#pragma region ____________________________ Fields _____________________________

private:
    std::unique_ptr<Wrapper> _data{std::make_unique<InvalidWrapper>()};

#pragma endregion
};

template<typename T>
Any
Any::create(T data)
{
    return Any{std::make_unique<ValidWrapper<T>>(std::forward<T>(data))};
}

}