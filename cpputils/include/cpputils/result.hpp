#pragma once

#include <string>
#include <type_traits>
#include "nullable.hpp"
#include <list>

namespace cu
{

class Result
{
    bool        _succeeded;
    std::string _message;

public:
    Result(bool succeeded = true, std::string message = {})
        : _succeeded{succeeded}
        , _message{std::move(message)}
    {
    }

    bool succeeded() const noexcept
    {
        return _succeeded;
    }

    bool failed() const noexcept
    {
        return !_succeeded;
    }

    const std::string& message() const noexcept
    {
        return _message;
    }
};

template<typename TData>
class DataResult : public Result
{
    Nullable<TData> _data{};

public:
    DataResult(TData data, std::string message = {})
        : Result(true, std::move(message))
        , _data{std::forward<TData>(data)}
    {
    }

    DataResult(std::string message = {})
        : Result(false, std::move(message))
    {
    }

    std::remove_reference_t<TData>& data()
    {
        if (failed())
            throw std::runtime_error(
                "Can not get the data when result is failure.");

        return _data.get();
    }
};

template<typename TStatus, typename TData>
class Response
{
    TStatus         _status;
    Nullable<TData> _data{};

public:
    Response(TStatus status)
        : _status{std::forward<TStatus>(status)}
    {
    }

    Response(TStatus status, TData data)
        : _status{std::forward<TStatus>(status)}
        , _data{std::forward<TData>(data)}
    {
    }

    const std::remove_cvref_t<TStatus>& status() const noexcept
    {
        return _status;
    }

    std::remove_reference_t<TData>& data()
    {
        if (_data.isNull())
            throw std::runtime_error("The data is null.");

        return _data.get();
    }
};

template<typename TStatusType>
class Status
{
    TStatusType _status;
    std::string _message;

public:
    Status(TStatusType status, std::string message = {})
        : _status{std::forward<TStatusType>(status)}
        , _message{std::move(message)}
    {
    }

    const std::remove_cvref_t<TStatusType>& status() const noexcept
    {
        return _status;
    }

    const std::string& message() const noexcept
    {
        return _message;
    }
};

template<typename TStatusType, typename TData>
class DataStatus : public Status<TStatusType>
{
    Nullable<TData> _data{};

public:
    DataStatus(TStatusType status, std::string message = {})
        : Status<TStatusType>(std::forward<TStatusType>(status),
                              std::move(message))
    {
    }

    DataStatus(TStatusType status, TData data, std::string message = {})
        : Status<TStatusType>(std::forward<TStatusType>(status),
                              std::move(message))
        , _data{std::forward<TData>(data)}
    {
    }

    std::remove_reference_t<TData>& data()
    {
        if (_data.isNull())
            throw std::runtime_error("The data is null.");

        return _data.get();
    }
};

class ResultCollector
{
    bool                   _anySucceeded{};
    bool                   _anyFailed{};
    std::list<std::string> _messages{};

public:
    void addResult(const Result& result)
    {
        if (result.failed())
            _anyFailed = true;
        else
            _anySucceeded = true;

        if (!result.message().empty())
            _messages.push_back(result.message());
    }

    bool anyFailed() const noexcept
    {
        return _anyFailed;
    }

    bool anySucceeded() const noexcept
    {
        return _anySucceeded;
    }

    const std::list<std::string>& messages() const noexcept
    {
        return _messages;
    }
};

}