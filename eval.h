#pragma once
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include "data.h"


class DataProvider;
class OperationProvider;

class Context {
private:
    std::unordered_map<std::string, Value*> valueCache;
public:
    std::vector<DataProvider*> dataProviders;
    std::vector<OperationProvider*> operationProviders;
    // This function should ensure that no circular dependencies occur and that all
    // dependencies are cached before finding categoryName.
    Context(); // updates the value cache with the constants pass, fail, and undef.
    Value* getCategoryValue(const std::string& categoryName);
    Value* executeOperation(const std::string& operationName, const std::vector<Value*>& arguments);
};


class OperationProvider {
public:
    virtual ~OperationProvider() = default;
    virtual bool hasOperation(const std::string& operationName) const = 0;
    virtual Value* executeOperation(const std::string& operationName, std::vector<Value*>& arguments) const = 0;
};


// interfaces for external databases should use this interface
class DataProvider {
public:
    virtual ~DataProvider() = default;
    virtual Value* getCategoryValue(const std::string& categoryName, Context* ctx) = 0;
};

class Expression;

class Program : public DataProvider {
public:
    std::unordered_map<std::string, Expression*> categories;
    ~Program();
    Value* getCategoryValue(const std::string& categoryName, Context* ctx) override;
};


class Expression {
public:
    virtual ~Expression() = default;
    virtual Value* evaluate(Context* ctx) const = 0;
    virtual std::unordered_set<std::string>* getDependencies() const = 0;
};


class ConstantExpr : public Expression {
private:
    Value* value;

public:
    ConstantExpr(Value* val) : value(val) {}
    Value* evaluate(Context* ctx) const override;
    std::unordered_set<std::string>* getDependencies() const override;
};

// Represents a reference to another category by name.
// If the category is of the List type, a copy of the ListValue is returned.
class CategoryRefExpr : public Expression {
private:
    std::string categoryName;
public:
    CategoryRefExpr(const std::string& name) : categoryName(name) {}
    Value* evaluate(Context* ctx) const override;
    std::unordered_set<std::string>* getDependencies() const override;
};

class ListElement {
public:
    Expression* valueExpr;
    Expression* weightExpr; // can be nullptr
    ListElement(Expression* valExpr, Expression* wtExpr = nullptr) : valueExpr(valExpr), weightExpr(wtExpr) {}
    ~ListElement();
};

class ListExpr : public Expression {
private:
    std::vector<ListElement*> elements;
public:
    ListExpr(const std::vector<ListElement*>& elems) : elements(elems) {}
    ~ListExpr();
    Value* evaluate(Context* ctx) const override;
    std::unordered_set<std::string>* getDependencies() const override;
};

class OperationExpr : public Expression {
private:
    std::string operationName;
    std::vector<Expression*> arguments;
public:
    OperationExpr(const std::string& opName, const std::vector<Expression*>& args)
        : operationName(opName), arguments(args) {};
    ~OperationExpr();
    Value* evaluate(Context* ctx) const override;
    std::unordered_set<std::string>* getDependencies() const override;
};

bool canCast(DataType fromType, DataType toType);

Value* castValue(Value* value, DataType targetType);

