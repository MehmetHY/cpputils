#pragma once

#include "export.hpp"
#include <string>
#include <type_traits>
#include <vector>
#include <span>
#include "concepts.hpp"

namespace cu
{

using ErrorMessage     = std::string;
using ErrorMessageList = std::vector<ErrorMessage>;

template<DefaultConstructable TCode>
class CU_EXPORT Error
{
public:
    Error()                   = default;
    virtual ~Error() noexcept = default;

    Error(TCode code, ErrorMessage message)
        : _code{std::forward<TCode>(code)}
        , _message{std::move(message)}
    {
    }

    const std::remove_reference_t<TCode>& code() const noexcept
    {
        return _code;
    }

    const ErrorMessage& message() const noexcept
    {
        return _message;
    }

private:
    const TCode        _code;
    const ErrorMessage _message;
};

template<DefaultConstructable TCode>
class CU_EXPORT Errors
{
public:
    Errors()                   = default;
    virtual ~Errors() noexcept = default;

    Errors(TCode code, ErrorMessageList messages)
        : _code{std::forward<TCode>(code)}
        , _messages{std::move(messages)}
    {
    }

    void addErrorMessage(ErrorMessage message)
    {
        _messages.push_back(std::move(message));
    }

    const std::remove_reference_t<TCode>& code() const noexcept
    {
        return _code;
    }

    std::span<const ErrorMessage> messages() const noexcept
    {
        return _messages;
    }

private:
    const TCode            _code;
    ErrorMessageList _messages;
};

template<DefaultConstructable TError>
class CU_EXPORT ResultInfo
{
public:
    ResultInfo()
        : _succeeded{true}
    {
    }

    ResultInfo(TError error)
        : _error{std::forward<TError>(error)}
    {
    }

    virtual ~ResultInfo() noexcept = default;

    bool succeeded() const noexcept
    {
        return _succeeded;
    }

    bool failed() const noexcept
    {
        return !_succeeded;
    }

    const std::remove_reference_t<TError>& error() const noexcept
    {
        return _error;
    }

    static ResultInfo success()
    {
        return {};
    }

    static ResultInfo failure(TError error)
    {
        return {std::forward<TError>(error)};
    }

private:
    const bool   _succeeded{};
    const TError _error{};
};

using Result = ResultInfo<ErrorMessage>;

template<typename TValue, DefaultConstructable TError>
class CU_EXPORT ResultObject
{
public:
    ResultObject(TValue value)
        : _value{std::forward<TValue>(value)}
    {
    }

    ResultObject(TError error, TValue defaultValue = {})
        : _value{std::forward<TValue>(defaultValue)}
        , _succeeded{false}
        , _error{std::forward<TError>(error)}
    {
    }

    virtual ~ResultObject() noexcept = default;

    typename std::remove_reference_t<TValue>& value() noexcept
    {
        return _value;
    }

    bool succeeded() const noexcept
    {
        return _succeeded;
    }

    bool failed() const noexcept
    {
        return !_succeeded;
    }

    static ResultObject<TValue, TError> success(TValue value)
    {
        return {std::forward<TValue>(value)};
    }

    static ResultObject<TValue, TError> failure(TError error,
                                                TValue defaultValue = {})
    {
        return {std::forward<TError>(error),
                std::forward<TValue>(defaultValue)};
    }

    const typename std::remove_reference_t<TError>& error() const noexcept
    {
        return _error;
    }

private:
    TValue       _value;
    const bool   _succeeded{true};
    const TError _error{};
};

template<typename TValue>
using ErrorOr = ResultObject<TValue, ErrorMessage>;
}