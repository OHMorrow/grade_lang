#include "eval.h"
#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <iomanip>

// Helper: convert a Value* to double for list elements/weights.
static double valueToDouble(Value* v) {
    if (!v) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    switch (v->getType()) {
        case DataType::TYPE_GRADE:
            return static_cast<GradeValue*>(v)->getVal();
        case DataType::TYPE_INTEGER:
            return static_cast<double>(static_cast<IntegerValue*>(v)->getVal());
        case DataType::TYPE_LIST:
            GradeValue* val = static_cast<ListValue*>(v)->toGrade();
            double out = val->getVal();
            delete val;
            return out;
    }
    return std::numeric_limits<double>::quiet_NaN();
}

Context::Context() {
    // Initialize the value cache with constants
    valueCache["pass"] = new GradeValue(1.0);
    valueCache["fail"] = new GradeValue(0.0);
    valueCache["undef"] = &undefinedGrade;
}

// Context implementation
Value* Context::getCategoryValue(const std::string& categoryName) {
    auto it = valueCache.find(categoryName);
    if (it != valueCache.end()) {
        return it->second;
    }
    for (DataProvider* dp : dataProviders) {
        if (!dp) continue;
        Value* val = dp->getCategoryValue(categoryName, this);
        if (val) {
            valueCache[categoryName] = val;
            return val;
        }
    }
    return &undefinedGrade;
}

// Add missing executeOperation implementation.
// It forwards to the first provider that reports it has the operation.
// We copy the argument list because provider interface takes a non-const vector<Value*>&.
Value* Context::executeOperation(const std::string& operationName, const std::vector<Value*>& arguments) {
    for (OperationProvider* op : operationProviders) {
        if (!op) continue;
        if (op->hasOperation(operationName)) {
            std::vector<Value*> argsCopy = arguments;
            return op->executeOperation(operationName, argsCopy);
        }
    }
    throw std::invalid_argument("Operation not found: " + operationName);
}

// Program implementation
Program::~Program() {
    for (auto &p : categories) {
        delete p.second;
    }
    categories.clear();
}

Value* Program::getCategoryValue(const std::string& categoryName, Context* ctx) {
    auto it = categories.find(categoryName);
    if (it == categories.end()) return nullptr;
    Expression* expr = it->second;
    Value* result = nullptr;
    if (expr) {
        result = expr->evaluate(ctx);
    }

    return result;
}

// ConstantExpr
Value* ConstantExpr::evaluate(Context* /*ctx*/) const {
    return value;
}

std::unordered_set<std::string>* ConstantExpr::getDependencies() const {
    return new std::unordered_set<std::string>();
}

// CategoryRefExpr
Value* CategoryRefExpr::evaluate(Context* ctx) const {
    if (!ctx) return nullptr;
    Value* v = ctx->getCategoryValue(categoryName);
    if (!v) return nullptr;
    if (v->getType() == DataType::TYPE_LIST) {
        return static_cast<ListValue*>(v)->copy();
    }
    return v;
}

std::unordered_set<std::string>* CategoryRefExpr::getDependencies() const {
    auto *deps = new std::unordered_set<std::string>();
    deps->insert(categoryName);
    return deps;
}

// ListElement destructor
ListElement::~ListElement() {
    delete valueExpr;
    delete weightExpr;
}

// ListExpr
ListExpr::~ListExpr() {
    for (auto *el : elements) delete el;
    elements.clear();
}

Value* ListExpr::evaluate(Context* ctx) const {
    ListValue* out = new ListValue();
    for (auto *el : elements) {
        Value* valV = el->valueExpr ? el->valueExpr->evaluate(ctx) : &undefinedGrade;
        double val = valueToDouble(valV);
        delete valV;

        double weight = 1.0;
        if (el->weightExpr) {
            Value* wtV = el->weightExpr->evaluate(ctx);
            weight = valueToDouble(wtV);
            delete wtV;
        }
        out->addValue(val, weight);
    }
    return out;
}

std::unordered_set<std::string>* ListExpr::getDependencies() const {
    auto *deps = new std::unordered_set<std::string>();
    for (auto *el : elements) {
        if (el->valueExpr) {
            auto *d = el->valueExpr->getDependencies();
            if (d) { deps->insert(d->begin(), d->end()); delete d; }
        }
        if (el->weightExpr) {
            auto *d = el->weightExpr->getDependencies();
            if (d) { deps->insert(d->begin(), d->end()); delete d; }
        }
    }
    return deps;
}

// OperationExpr
OperationExpr::~OperationExpr() {
    for (auto *arg : arguments) delete arg;
    arguments.clear();
}

Value* OperationExpr::evaluate(Context* ctx) const {
    return ctx->executeOperation(operationName, 
        [this, ctx]() {
            std::vector<Value*> args;
            for (auto *argExpr : arguments) {
                args.push_back(argExpr->evaluate(ctx));
            }
            return args;
        }());
}

std::unordered_set<std::string>* OperationExpr::getDependencies() const {
    auto *deps = new std::unordered_set<std::string>();
    for (auto *arg : arguments) {
        auto *d = arg->getDependencies();
        if (d) { deps->insert(d->begin(), d->end()); delete d; }
    }
    return deps;
}


bool canCast(DataType fromType, DataType toType) {
    switch (fromType) {
        case DataType::TYPE_GRADE:
            return toType == DataType::TYPE_GRADE;
        case DataType::TYPE_INTEGER:
            return toType == DataType::TYPE_INTEGER || toType == DataType::TYPE_GRADE;
        case DataType::TYPE_LIST:
            return toType == DataType::TYPE_LIST || toType == DataType::TYPE_GRADE;
    }
    return false;
}


Value* castValue(Value* value, DataType targetType) {
    if (!value) return nullptr;
    DataType fromType = value->getType();
    if (!canCast(fromType, targetType)) {
        return nullptr;
    }
    if (fromType == targetType) {
        return value;
    }
    switch (fromType) {
        case DataType::TYPE_INTEGER:
            if (targetType == DataType::TYPE_GRADE) {
                double gradeVal = static_cast<double>(static_cast<IntegerValue*>(value)->getVal());
                return new GradeValue(gradeVal);
            }
            break;
        case DataType::TYPE_LIST:
            if (targetType == DataType::TYPE_GRADE) {
                return static_cast<ListValue*>(value)->toGrade();
            }
            break;
        default:
            break;
    }
    return nullptr;
}

// Helper to print indentation
static void printIndent(std::ostream& os, int indent) {
    for (int i = 0; i < indent; ++i) os.put(' ');
}

// Helper to print a Value inline for AST printing (non-owning, don't delete)
static void printValueInline(std::ostream& os, Value* v) {
    if (!v) {
        os << "<null>";
        return;
    }
    switch (v->getType()) {
        case DataType::TYPE_GRADE: {
            double g = static_cast<GradeValue*>(v)->getVal();
            if (std::isnan(g)) os << "undef";
            else os << g;
            break;
        }
        case DataType::TYPE_INTEGER: {
            os << static_cast<IntegerValue*>(v)->getVal();
            break;
        }
        case DataType::TYPE_LIST: {
            ListValue* lv = static_cast<ListValue*>(v);
            os << "{";
            for (size_t i = 0; i < lv->size(); ++i) {
                if (i) os << ", ";
                double val = lv->getValueAt(i);
                double wt  = lv->getWeightAt(i);
                if (std::isnan(val)) os << "undef";
                else os << val;
                if (wt != 1.0) os << ":" << wt;
            }
            os << "}";
            break;
        }
        default:
            os << "<unknown>";
    }
}

// ConstantExpr::printAST
void ConstantExpr::printAST(std::ostream& os, int indent) const {
    printIndent(os, indent);
    os << "Constant: ";
    // we can safely use evaluate with nullptr because ConstantExpr doesn't use ctx
    Value* v = value;
    printValueInline(os, v);
    os << "\n";
}

// CategoryRefExpr::printAST
void CategoryRefExpr::printAST(std::ostream& os, int indent) const {
    printIndent(os, indent);
    os << "CategoryRef: " << categoryName << "\n";
}

// ListExpr::printAST
void ListExpr::printAST(std::ostream& os, int indent) const {
    printIndent(os, indent);
    os << "List:\n";
    for (auto *el : elements) {
        printIndent(os, indent + 2);
        os << "- Element:\n";
        if (el->valueExpr) {
            el->valueExpr->printAST(os, indent + 4);
        } else {
            printIndent(os, indent + 4);
            os << "Value: <null>\n";
        }
        if (el->weightExpr) {
            printIndent(os, indent + 4);
            os << "Weight:\n";
            el->weightExpr->printAST(os, indent + 6);
        }
    }
}

// OperationExpr::printAST
void OperationExpr::printAST(std::ostream& os, int indent) const {
    printIndent(os, indent);
    os << "Operation: " << operationName << "\n";
    for (size_t i = 0; i < arguments.size(); ++i) {
        printIndent(os, indent + 2);
        os << "Arg " << i << ":\n";
        if (arguments[i]) arguments[i]->printAST(os, indent + 4);
        else {
            printIndent(os, indent + 4);
            os << "<null>\n";
        }
    }
}