#pragma once

#include <string>
#include <type_traits>
#include "nullable.hpp"
#include <list>
#include <functional>
#include <map>

namespace cu
{

class Result
{
#pragma region _________________________ Constructors __________________________

public:
    Result(bool succeeded = true, std::string message = {})
        : _succeeded{succeeded}
        , _message{std::move(message)}
    {
    }

#pragma endregion

#pragma region ___________________________ Methods _____________________________

public:
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

#pragma endregion

#pragma region ____________________________ Fields _____________________________

private:
    bool        _succeeded;
    std::string _message;

#pragma endregion
};

template<typename TData>
class DataResult : public Result
{
#pragma region _________________________ Constructors __________________________

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

#pragma endregion

#pragma region ___________________________ Methods _____________________________

public:
    std::remove_reference_t<TData>& data()
    {
        if (failed())
            throw std::runtime_error(
                "Can not get the data when result is failure.");

        return _data.get();
    }

#pragma endregion

#pragma region ____________________________ Fields _____________________________

private:
    Nullable<TData> _data{};

#pragma endregion
};

template<typename TStatus, typename TData>
class Response
{
#pragma region _________________________ Constructors __________________________

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

#pragma endregion

#pragma region ___________________________ Methods _____________________________

public:
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

#pragma endregion

#pragma region ____________________________ Fields _____________________________

private:
    TStatus         _status;
    Nullable<TData> _data{};

#pragma endregion
};

template<typename TStatusType>
class Status
{
#pragma region _________________________ Constructors __________________________

public:
    Status(TStatusType status, std::string message = {})
        : _status{std::forward<TStatusType>(status)}
        , _message{std::move(message)}
    {
    }

#pragma endregion

#pragma region ___________________________ Methods _____________________________
    
public:
    const std::remove_cvref_t<TStatusType>& status() const noexcept
    {
        return _status;
    }

    const std::string& message() const noexcept
    {
        return _message;
    }

#pragma endregion

#pragma region ____________________________ Fields _____________________________

private:
    TStatusType _status;
    std::string _message;

#pragma endregion
};

template<typename TStatusType, typename TData>
class DataStatus : public Status<TStatusType>
{
#pragma region _________________________ Constructors __________________________

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

#pragma endregion

#pragma region ___________________________ Methods _____________________________

public:
    std::remove_reference_t<TData>& data()
    {
        if (_data.isNull())
            throw std::runtime_error("The data is null.");

        return _data.get();
    }

#pragma endregion

#pragma region ____________________________ Fields _____________________________

private:
    Nullable<TData> _data{};

#pragma endregion
};

class ResultCollector
{
#pragma region ___________________________ Methods _____________________________

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

#pragma endregion

#pragma region ____________________________ Fields _____________________________

private:
    bool                   _anySucceeded{};
    bool                   _anyFailed{};
    std::list<std::string> _messages{};

#pragma endregion
};

template<typename TStatus>
class StatusActionMapper
{
#pragma region ___________________________ Methods _____________________________

public:
    StatusActionMapper<TStatus>& bind(TStatus                   status,
                                      std::function<void(void)> action)
    {
        _map[std::forward<TStatus>(status)] = std::move(action);

        return *this;
    }

    void execute(const std::remove_cvref_t<TStatus>& status)
    {
        if (_map.contains(status))
            _map[status]();
    }

#pragma endregion

#pragma region ____________________________ Fields _____________________________

private:
    std::map<TStatus, std::function<void(void)>> _map{};

#pragma endregion
};

}