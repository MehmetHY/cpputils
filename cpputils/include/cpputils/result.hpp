#pragma once

#include "export.hpp"
#include <string>
#include <vector>
#include <map>
#include "concepts.hpp"
#include <memory>

namespace cu
{

using Error            = std::string;
using ErrorDescription = Error;
using ErrorList        = std::vector<Error>;

template<DefaultConstructable TCode>
class CU_EXPORT ErrorInfo
{
public: // Types ______________________________________________________________

public: // Constructors & Destructor __________________________________________
    ErrorInfo() noexcept = default;

    ErrorInfo(TCode code, ErrorDescription description) noexcept
        : _code{std::move(code)}
        , _description{std::move(description)}
    {
    }

    virtual ~ErrorInfo() noexcept = default;

public: // Move Semantics _____________________________________________________
    ErrorInfo(ErrorInfo<TCode>&&) noexcept                           = default;
    virtual ErrorInfo<TCode>& operator=(ErrorInfo<TCode>&&) noexcept = default;

public: // Copy Semantics _____________________________________________________
    ErrorInfo(const ErrorInfo<TCode>&)                   = default;
    ErrorInfo<TCode>& operator=(const ErrorInfo<TCode>&) = default;

public: // Operators __________________________________________________________

public: // Events _____________________________________________________________

public: // Methods ____________________________________________________________
    const TCode& code() const
    {
        return _code;
    }

    const ErrorDescription& description() const
    {
        return _description;
    }

private: // Fields ____________________________________________________________
    TCode            _code{};
    ErrorDescription _description{};
};

using ErrorDictionary = ErrorInfo<Error>;
using LineNumber      = size_t;
using LineError       = ErrorInfo<LineNumber>;

template<DefaultConstructable TError>
class CU_EXPORT ResultInfo
{
public: // Types ______________________________________________________________

public: // Constructors & Destructor __________________________________________
    ResultInfo() noexcept = default;

    ResultInfo(TError error) noexcept
        : _succeeded{false}
        , _error{std::move(error)}
    {
    }

    virtual ~ResultInfo() noexcept = default;

public: // Move Semantics _____________________________________________________
    ResultInfo(ResultInfo<TError>&&) noexcept = default;
    virtual ResultInfo<TError>& operator=(ResultInfo<TError>&&) noexcept =
        default;

public: // Copy Semantics _____________________________________________________
    ResultInfo(const ResultInfo<TError>&)                    = default;
    ResultInfo<TError>& operator=(const ResultInfo<TError>&) = default;

public: // Operators __________________________________________________________

public: // Events _____________________________________________________________

public: // Methods ____________________________________________________________
    bool succeeded() const
    {
        return _succeeded;
    }

    const TError& error() const
    {
        return _error;
    }

    static ResultInfo<TError> success()
    {
        return ResultInfo<TError>();
    }

    static ResultInfo<TError> failure(TError error)
    {
        return ResultInfo<TError>(std::move(error));
    }

private: // Fields ____________________________________________________________
    bool   _succeeded{true};
    TError _error{};
};

using Result = ResultInfo<Error>;

template<typename TObj, DefaultConstructable TError>
class CU_EXPORT ResultObject : public ResultInfo<TError>
{
public: // Types ______________________________________________________________

public: // Constructors & Destructor __________________________________________
    ResultObject(TError error) noexcept
        : ResultInfo<TError>(std::move(error))
    {
    }

    ResultObject(std::unique_ptr<TObj> obj) noexcept
        : ResultInfo<TError>()
        , _obj{std::move(obj)}
    {
    }

    virtual ~ResultObject() noexcept = default;

public: // Move Semantics _____________________________________________________
    ResultObject(ResultObject<TObj, TError>&&) noexcept = default;
    virtual ResultObject<TObj, TError>& operator=(
        ResultObject<TObj, TError>&&) noexcept = default;

public: // Copy Semantics _____________________________________________________
    ResultObject(const ResultObject<TObj, TError>&) = delete;
    ResultObject<TObj, TError>& operator=(const ResultObject<TObj, TError>&) =
        delete;

public: // Operators __________________________________________________________

public: // Events _____________________________________________________________

public: // Methods ____________________________________________________________
    static ResultObject<TObj, TError> success(std::unique_ptr<TObj> obj)
    {
        return ResultObject<TObj, TError>(std::move(obj));
    }

    static ResultObject<TObj, TError> failure(TError error)
    {
        return ResultObject<TObj, TError>(std::move(error));
    }

    std::unique_ptr<TObj> object()
    {
        return std::move(_obj);
    }

private: // Fields ____________________________________________________________
    std::unique_ptr<TObj> _obj{};
};

template<typename TObj>
using ErrorOr = ResultObject<TObj, Error>;

}