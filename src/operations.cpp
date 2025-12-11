#include "operations.h"
#include <queue>
#include <cmath>
#include <functional>
#include <iterator>

// modify the list by dropping the lowest n values from the given list,
// ignoring undefined values (NaN) and taking weights into account.
ListValue* drop(unsigned long long n, ListValue* lv) {
    if (n == 0 || lv->size() == 0) {
        return lv;
    }

    // Min-heap to store the lowest n values
    auto cmp = [](const std::pair<double, size_t>& a, const std::pair<double, size_t>& b) {
        return a.first < b.first; // Min-heap based on value
    };
    std::priority_queue<std::pair<double, size_t>, std::vector<std::pair<double, size_t>>, decltype(cmp)> minHeap(cmp);

    ListValueIterator it(lv);
    size_t index = 0;
    while (it.hasNext()) {
        double value = it.getValue();
        if (!std::isnan(value)) {
            minHeap.emplace(value, index);
            if (minHeap.size() > n) {
                minHeap.pop();
            }
        }
        it.advance();
        index++;
    }

    // Collect indices to remove
    std::vector<size_t> indicesToRemove;
    while (!minHeap.empty()) {
        indicesToRemove.push_back(minHeap.top().second);
        minHeap.pop();
    }
    std::sort(indicesToRemove.begin(), indicesToRemove.end(), std::greater<size_t>());

    // Remove elements from the list in reverse order
    for (size_t idx : indicesToRemove) {
        lv->removeAt(idx);
    }

    return lv;
}

// top is like drop, but keeps the highest n values instead of dropping the lowest n.
ListValue* top(unsigned long long n,  ListValue* lv) {
    drop(lv->size() > n ? lv->size() - n : 0, lv);
    return lv;
}

// join modifies a list by extending it with the contents of another list by concatenating the two given listValues.
// Either argument is deleted after joining.
ListValue* join(ListValue* lv1, ListValue* lv2) {
    ListValueIterator it2(lv2);

    while (it2.hasNext()) {
        lv1->addValue(it2.getValue(), it2.getWeight());
        it2.advance();
    }

    delete lv2;
    return lv1;
}

// resolve creates a new listValue where all undefined (NaN) values are replaced with the given default value.
ListValue* resolve(double defaultValue, ListValue* lv) {
    ListValueIterator it(lv);
    while (it.hasNext()) {
        double value = it.getValue();
        if (std::isnan(value)) {
            it.setValue(defaultValue);
        }
        it.advance();
    }
    return lv;
}

// clamp modifies the given list in-place, replacing all values smaller than minValue with minValue,
// and all values larger than maxValue with maxValue.
ListValue* clamp(double minValue, double maxValue, ListValue* lv) {
    ListValueIterator it(lv);
    while (it.hasNext()) {
        double value = it.getValue();
        if (!std::isnan(value)) {
            if (value < minValue) {
                it.setValue(minValue);
            } else if (value > maxValue) {
                it.setValue(maxValue);
            }
        }
        it.advance();
    }
    return lv;
}

// maxOf replaces all values in the list smaller than the given threshold with the threshold value.
ListValue* maxOf(double threshold, ListValue* lv) {
    ListValueIterator it(lv);
    while (it.hasNext()) {
        double value = it.getValue();
        if (!std::isnan(value) && value < threshold) {
            value = threshold;
            it.setValue(value);
        }
        it.advance();
    }
    return lv;
}

// minOf replaces all values in the list larger than the given threshold with the threshold value.
ListValue* minOf(double threshold, ListValue* lv) {
    ListValueIterator it(lv);
    while (it.hasNext()) {
        double value = it.getValue();
        if (!std::isnan(value) && value > threshold) {
            value = threshold;
            it.setValue(value);
        }
        it.advance();
    }
    return lv;
}

// map linearly maps all values from the source range [srcStart, srcEnd]
// to the destination range [dstStart, dstEnd]. Values outside the source
// ranges are not clamped but extrapolated.
ListValue* map(double srcStart, double srcEnd, double dstStart, double dstEnd, ListValue* lv) {
    double srcRange = srcEnd - srcStart;
    double dstRange = dstEnd - dstStart;

    if (srcRange == 0.0) {
        // If source range is zero, set all defined values to the midpoint of the destination range
        double midDst = dstStart + dstRange / 2.0;
        ListValueIterator it(lv);
        while (it.hasNext()) {
            double value = it.getValue();
            if (!std::isnan(value)) {
                it.setValue(midDst);
            }
            it.advance();
        }
    } else {
        ListValueIterator it(lv);
        while (it.hasNext()) {
            double value = it.getValue();
            if (!std::isnan(value)) {
                double mappedValue = dstStart + ((value - srcStart) / srcRange) * dstRange;
                it.setValue(mappedValue);
            }
            it.advance();
        }
    }
    return lv;
}

double require(double value, double threshold, double resultBelow, double resultAbove) {
    return (value < threshold) ? resultBelow : resultAbove;
}

double require(double value, double threshold, double reslutAbove) {
    return require(value, threshold, 0.0, reslutAbove);
}

double require(double value, double threshold) {
    return require(value, threshold, 0.0, 1.0);
}

BasicOperationProvider* createProvider() {
    BasicOperationProvider* provider = new BasicOperationProvider();

    provider->registerOperation("drop", drop);
    provider->registerOperation("top", top);
    provider->registerOperation("join", join);
    provider->registerOperation("resolve", resolve);
    provider->registerOperation("clamp", clamp);
    provider->registerOperation("maxOf", maxOf);
    provider->registerOperation("minOf", minOf);
    provider->registerOperation("map", map);
    provider->registerOperation<double, double, double, double, double>("require", require);
    provider->registerOperation<double, double, double, double>("require", require);
    provider->registerOperation<double, double, double>("require", require);
    provider->registerOperation("len", std::function<unsigned long long(ListValue*)>([](ListValue* lv) -> unsigned long long {
        return static_cast<unsigned long long>(lv->size());
    }));

    return provider;
}