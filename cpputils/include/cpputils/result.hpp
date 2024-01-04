#pragma once

#include "export.hpp"
#include <string>
#include <type_traits>
#include <vector>
#include <span>
#include "concepts.hpp"

namespace cu
{

using Message     = std::string;
using MessageList = std::vector<Message>;

template<DefaultConstructable TCode>
class CU_EXPORT Status
{
public:
    Status()                   = default;
    virtual ~Status() noexcept = default;

    Status(TCode code, Message message)
        : _code{std::forward<TCode>(code)}
        , _message{std::move(message)}
    {
    }

    const std::remove_reference_t<TCode>& code() const noexcept
    {
        return _code;
    }

    const Message& message() const noexcept
    {
        return _message;
    }

private:
    const TCode   _code;
    const Message _message;
};

template<DefaultConstructable TCode>
class CU_EXPORT StatusMessageList
{
public:
    StatusMessageList()                   = default;
    virtual ~StatusMessageList() noexcept = default;

    StatusMessageList(TCode code, MessageList messages)
        : _code{std::forward<TCode>(code)}
        , _messages{std::move(messages)}
    {
    }

    void addMessage(Message message)
    {
        _messages.push_back(std::move(message));
    }

    const std::remove_reference_t<TCode>& code() const noexcept
    {
        return _code;
    }

    std::span<const Message> messages() const noexcept
    {
        return _messages;
    }

private:
    const TCode _code;
    MessageList _messages;
};

template<DefaultConstructable TStatus>
class CU_EXPORT ResultInfo
{
public:
    ResultInfo()
        : _succeeded{true}
    {
    }

    ResultInfo(TStatus error)
        : _error{std::forward<TStatus>(error)}
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

    const std::remove_reference_t<TStatus>& error() const noexcept
    {
        return _error;
    }

    static ResultInfo success()
    {
        return {};
    }

    static ResultInfo failure(TStatus error)
    {
        return {std::forward<TStatus>(error)};
    }

private:
    const bool    _succeeded{};
    const TStatus _error{};
};

using Result = ResultInfo<Message>;

template<typename TValue, DefaultConstructable TStatus>
class CU_EXPORT ResultObject
{
public:
    ResultObject(TValue value)
        : _value{std::forward<TValue>(value)}
    {
    }

    ResultObject(TStatus error, TValue defaultValue = {})
        : _value{std::forward<TValue>(defaultValue)}
        , _succeeded{false}
        , _error{std::forward<TStatus>(error)}
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

    static ResultObject<TValue, TStatus> success(TValue value)
    {
        return {std::forward<TValue>(value)};
    }

    static ResultObject<TValue, TStatus> failure(TStatus error,
                                                 TValue  defaultValue = {})
    {
        return {std::forward<TStatus>(error),
                std::forward<TValue>(defaultValue)};
    }

    const typename std::remove_reference_t<TStatus>& error() const noexcept
    {
        return _error;
    }

private:
    TValue        _value;
    const bool    _succeeded{true};
    const TStatus _error{};
};

template<typename TValue>
using ErrorOr = ResultObject<TValue, Message>;
}