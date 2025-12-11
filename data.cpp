#include "data.h"
#include <cmath>
#include <iterator>

// GradeValue implementations
GradeValue::GradeValue(double g) : grade(g) {}

DataType GradeValue::getType() const { return DataType::TYPE_GRADE; }
double GradeValue::getVal() const { return grade; }
void GradeValue::setVal(double g) { grade = g; }
bool GradeValue::isUndefined() const { return std::isnan(grade); }

// define the extern undefinedGrade
GradeValue undefinedGrade(std::numeric_limits<double>::quiet_NaN());

// IntegerValue implementations
IntegerValue::IntegerValue(unsigned long long val) : intValue(val) {}
DataType IntegerValue::getType() const { return DataType::TYPE_INTEGER; }
unsigned long long IntegerValue::getVal() const { return intValue; }
void IntegerValue::setVal(unsigned long long val) { intValue = val; }

// ListValue implementations
ListValue::ListValue() : listValues() {}
DataType ListValue::getType() const { return DataType::TYPE_LIST; }
double ListValue::getValueAt(size_t index) const { return listValues[index].first; }
double ListValue::getWeightAt(size_t index) const { return listValues[index].second; }
size_t ListValue::size() const { return listValues.size(); }
void ListValue::addValue(double value, double weight) { listValues.emplace_back(value, weight); }
void ListValue::clear() { listValues.clear(); }
void ListValue::setValueAt(size_t index, double value) { listValues[index].first = value; }
void ListValue::setWeightAt(size_t index, double weight) { listValues[index].second = weight; }
void ListValue::removeAt(size_t index) { listValues.erase(listValues.begin() + index); }
void ListValue::insertAt(size_t index, double value, double weight) { listValues.insert(listValues.begin() + index, std::make_pair(value, weight)); }

ListValue* ListValue::copy() const {
    ListValue* newList = new ListValue();
    newList->listValues = listValues;
    return newList;
}

GradeValue* ListValue::toGrade() const {
    double totalWeightedValue = 0.0;
    double totalWeight = 0.0;

    for (const auto& pair : listValues) {
        if (!std::isnan(pair.first)) {
            totalWeightedValue += pair.first * pair.second;
            totalWeight += pair.second;
        }
    }
    if (totalWeight == 0.0) {
        return &undefinedGrade;
    } else {
        return new GradeValue(totalWeightedValue / totalWeight);
    }
}

// ListValueIterator implementations
ListValueIterator::ListValueIterator(ListValue* lv) : listValue(lv), currentIt(lv->listValues.begin()) {}

bool ListValueIterator::hasNext() const {
    return currentIt != listValue->listValues.end();
}

void ListValueIterator::advance() {
    if (hasNext()) {
        ++currentIt;
    } else {
        throw std::out_of_range("Iterator has reached the end of the list.");
    }
}

double ListValueIterator::getValue() const {
    if (hasNext()) {
        return currentIt->first;
    } else {
        throw std::out_of_range("Iterator has reached the end of the list.");
    }
}

double ListValueIterator::getWeight() const {
    if (hasNext()) {
        return currentIt->second;
    } else {
        throw std::out_of_range("Iterator has reached the end of the list.");
    }
}

void ListValueIterator::setValue(double value) {
    if (hasNext()) {
        currentIt->first = value;
    } else {
        throw std::out_of_range("Iterator has reached the end of the list.");
    }
}

void ListValueIterator::setWeight(double weight) {
    if (hasNext()) {
        currentIt->second = weight;
    } else {
        throw std::out_of_range("Iterator has reached the end of the list.");
    }
}

void ListValueIterator::discard() {
    if (hasNext()) {
        currentIt = listValue->listValues.erase(currentIt);
    } else {
        throw std::out_of_range("Iterator has reached the end of the list.");
    }
}

void ListValueIterator::insertBefore(double value, double weight) {
    currentIt = listValue->listValues.insert(currentIt, std::make_pair(value, weight));
}

void ListValueIterator::insertAfter(double value, double weight) {
    if (hasNext()) {
        auto nextIt = std::next(currentIt);
        listValue->listValues.insert(nextIt, std::make_pair(value, weight));
    } else {
        listValue->listValues.emplace_back(value, weight);
    }
}
