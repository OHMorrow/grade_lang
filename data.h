#pragma once
#include <vector>
#include <limits>
#include <utility>
#include <stdexcept>

enum class DataType {
    TYPE_GRADE,
    TYPE_INTEGER,
    TYPE_LIST,
};

class Value {
public:
    virtual ~Value() = default;
    virtual DataType getType() const = 0;
};


// Grade value is typically between 0.0 and 1.0, but can exceed 1.0 for extra credit.
// An undefined grade is represented by NaN (not a number).
class GradeValue : public Value {
private:
    double grade;
public:
    GradeValue(double g);
    DataType getType() const override;
    double getVal() const;
    void setVal(double g);
    bool isUndefined() const;
};

extern GradeValue undefinedGrade;

// Integer value represents non-negative whole numbers.
class IntegerValue : public Value {
private:
    unsigned long long intValue;
public:
    IntegerValue(unsigned long long val);
    DataType getType() const override;
    unsigned long long getVal() const;
    void setVal(unsigned long long val);
};

// List values contain a sequence of double values, each of which has a double weight associated with it.
class ListValue : public Value {
private:
    std::vector<std::pair<double, double>> listValues; // pair<value, weight>
    friend class ListValueIterator;
public:
    ListValue();
    DataType getType() const override;
    double getValueAt(size_t index) const;
    double getWeightAt(size_t index) const;
    size_t size() const;
    void addValue(double value, double weight = 1);
    void clear();
    void setValueAt(size_t index, double value);
    void setWeightAt(size_t index, double weight);
    void removeAt(size_t index);
    void insertAt(size_t index, double value, double weight = 1);
    ListValue* copy() const;
    GradeValue* toGrade() const;
};

class ListValueIterator {
private:
    ListValue* listValue;
    std::vector<std::pair<double, double>>::iterator currentIt;
public:
    ListValueIterator(ListValue* lv);

    bool hasNext() const;
    void advance();

    double getValue() const;
    double getWeight() const;

    void setValue(double value);
    void setWeight(double weight);

    void discard();

    void insertBefore(double value, double weight = 1);
    void insertAfter(double value, double weight = 1);
};
