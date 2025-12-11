#include "basic_operation_provider.h"
#include <stdexcept>

// hasOperation implementation
bool BasicOperationProvider::hasOperation(const std::string& operationName) const {
    for (const auto& op : operations) {
        if (op.first.name == operationName) {
            return true;
        }
    }
    return false;
}

// executeOperation implementation
Value* BasicOperationProvider::executeOperation(const std::string& operationName, std::vector<Value*>& arguments) const {
    std::vector<DataType> argTypes;
    argTypes.reserve(arguments.size());
    for (Value* arg : arguments) {
        argTypes.push_back(arg ? arg->getType() : DataType::TYPE_GRADE);
    }
    for (const auto& op : operations) {
        if (op.first.matches(operationName, argTypes)) {
            return op.second(arguments);
        }
    }
    throw std::invalid_argument("Operation not found: " + operationName);
}

// registerOperation(OperationSignature, func) implementation
void BasicOperationProvider::registerOperation(OperationSignature sig, std::function<Value*(std::vector<Value*>&)> func) {
    operations.emplace_back(std::move(sig), std::move(func));
}
