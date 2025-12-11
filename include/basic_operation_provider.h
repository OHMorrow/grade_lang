#pragma once
#include "eval.h"
#include <functional>
#include <string>
#include <vector>
#include <stdexcept>


// OperationSignature and matching logic
class OperationSignature {
public:
    std::string name;
    std::vector<DataType> argumentTypes;
    
    OperationSignature(const std::string& opName, const std::vector<DataType>& argTypes)
        : name(opName), argumentTypes(argTypes) {}
    bool matches(const std::string& opName, const std::vector<DataType>& argTypes) const {
        if (name != opName || argumentTypes.size() != argTypes.size()) {
            return false;
        }
        for (size_t i = 0; i < argumentTypes.size(); ++i) {
            if (!canCast(argumentTypes[i], argTypes[i])) {
                return false;
            }
        }
        return true;
    }
};

template<typename T>
struct TypeToDataType;

template<>
struct TypeToDataType<GradeValue*> {
    static constexpr DataType value = DataType::TYPE_GRADE;
};

template<>
struct TypeToDataType<IntegerValue*> {
    static constexpr DataType value = DataType::TYPE_INTEGER;
};

template<>
struct TypeToDataType<ListValue*> {
    static constexpr DataType value = DataType::TYPE_LIST;
};

template<>
struct TypeToDataType<double> {
    static constexpr DataType value = DataType::TYPE_GRADE;
};

template<>
struct TypeToDataType<unsigned long long> {
    static constexpr DataType value = DataType::TYPE_INTEGER;
};

template<typename T>
T castArgument(Value* v);

// Make explicit specializations inline to avoid multiple-definition errors
template<>
inline GradeValue* castArgument<GradeValue*>(Value* v) {
    GradeValue* out = static_cast<GradeValue*>(castValue(v, DataType::TYPE_GRADE));
    if (!out) {
        throw std::invalid_argument("Failed to cast argument to GradeValue.");
    }
    return out;
}

template<>
inline IntegerValue* castArgument<IntegerValue*>(Value* v) {
    IntegerValue* out = static_cast<IntegerValue*>(castValue(v, DataType::TYPE_INTEGER));
    if (!out) {
        throw std::invalid_argument("Failed to cast argument to IntegerValue.");
    }
    return out;
}

template<>
inline ListValue* castArgument<ListValue*>(Value* v) {
    ListValue* out = static_cast<ListValue*>(castValue(v, DataType::TYPE_LIST));
    if (!out) {
        throw std::invalid_argument("Failed to cast argument to ListValue.");
    }
    return out;
}

template<>
inline double castArgument<double>(Value* v) {
    GradeValue* gv = static_cast<GradeValue*>(castValue(v, DataType::TYPE_GRADE));
    if (!gv) {
        throw std::invalid_argument("Failed to cast argument to double.");
    }
    delete v;
    return gv->getVal();
}

template<>
inline unsigned long long castArgument<unsigned long long>(Value* v) {
    IntegerValue* iv = static_cast<IntegerValue*>(castValue(v, DataType::TYPE_INTEGER));
    if (!iv) {
        throw std::invalid_argument("Failed to cast argument to unsigned long long.");
    }
    delete v;
    return iv->getVal();
}

template<typename T>
Value* castReturnValue(T retVal);

// Mark explicit return-value specializations inline as well
template<>
inline Value* castReturnValue<GradeValue*>(GradeValue* retVal) {
    return retVal;
}

template<>
inline Value* castReturnValue<IntegerValue*>(IntegerValue* retVal) {
    return retVal;
}

template<>
inline Value* castReturnValue<ListValue*>(ListValue* retVal) {
    return retVal;
}

template<>
inline Value* castReturnValue<double>(double retVal) {
    return new GradeValue(retVal);
}

template<>
inline Value* castReturnValue<unsigned long long>(unsigned long long retVal) {
    return new IntegerValue(retVal);
}

template<typename R, typename S, typename ...T>
std::function<R(T...)> _bind_front(std::function<R(S, T...)> func, S arg) {
    return [func, arg](T... rest) -> R {
        return func(arg, rest...);
    };
}

template<typename R, typename S, typename... T>
std::function<Value*(const std::vector<Value*>::iterator&)> _wrapOperation(std::function<R(S, T...)> func) {
    return [func](const std::vector<Value*>::iterator& argsIt) -> Value* {
        S firstArg = castArgument<S>(*(argsIt));
        if constexpr (sizeof...(T) == 0) {
            return castReturnValue<R>(func(firstArg));
        } else {
            auto restFunc = _wrapOperation<R, T...>(_bind_front(func, firstArg));
            return restFunc(argsIt + 1);
        }
    };
}

template<typename R>
std::function<Value*(const std::vector<Value*>&)> _wrapOperation(std::function<R()> func) {
    return [func](const std::vector<Value*>& /*args*/) -> Value* {
        return castReturnValue<R>(func());
    };
}

template<typename... T>
OperationSignature _makeSignature(const std::string& name) {
    return OperationSignature(name, { TypeToDataType<T>::value... });
}

class BasicOperationProvider : public OperationProvider {
private:
    std::vector<std::pair<OperationSignature, std::function<Value*(std::vector<Value*>&)>>> operations;
public:
    // NON-TEMPLATE member function declarations (implemented in .cpp)
    bool hasOperation(const std::string& operationName) const override;
    Value* executeOperation(const std::string& operationName, std::vector<Value*>& arguments) const override;
    void registerOperation(OperationSignature sig, std::function<Value*(std::vector<Value*>&)> func);

    // member-template overloads remain inline so they can be instantiated
    template<typename S, typename... T>
    void registerOperation(const std::string& name, std::function<S(T...)> func) {
        OperationSignature sig = _makeSignature<T...>(name);
        std::function<Value*(std::vector<Value*>&)> wrappedFunc = [func](std::vector<Value*>& args) -> Value* {
            if (args.size() != sizeof...(T)) {
                throw std::invalid_argument("Incorrect number of arguments for operation.");
            }
            auto argsIt = args.begin();
            return _wrapOperation<S, T...>(func)(argsIt);
        };
        registerOperation(sig, wrappedFunc);
    }

    template<typename S, typename... T>
    void registerOperation(const std::string& name, S(*func)(T...)) {
        std::function<S(T...)> f = func;
        registerOperation<S, T...>(name, f);
    }
};

