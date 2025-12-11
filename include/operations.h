#pragma once
#include "data.h"
#include "basic_operation_provider.h"
#include <cstddef>

// create a new list by dropping the lowest n values from the given list,
// ignoring undefined values (NaN) and taking weights into account.
ListValue* drop(size_t n, ListValue* lv);

// top is like drop, but keeps the highest n values instead of dropping the lowest n.
ListValue* top(size_t n, ListValue* lv);

// join creates a new listValue by concatenating the two given listValues.
// Either argument is deleted after joining.
ListValue* join(ListValue* lv1, ListValue* lv2);

// resolve creates a new listValue where all undefined (NaN) values are replaced with the given default value.
ListValue* resolve(double defaultValue, ListValue* lv);

// clamp modifies the given list in-place, replacing all values smaller than minValue with minValue,
// and all values larger than maxValue with maxValue.
ListValue* clamp(double minValue, double maxValue, ListValue* lv);

// maxOf replaces all values in the list smaller than the given threshold with the threshold value.
ListValue* maxOf(double threshold, ListValue* lv);

// minOf replaces all values in the list larger than the given threshold with the threshold value.
ListValue* minOf(double threshold, ListValue* lv);

// map linearly maps all values from the source range [srcStart, srcEnd]
// to the destination range [dstStart, dstEnd]. Values outside the source
// ranges are not clamped but extrapolated.
ListValue* map(double srcStart, double srcEnd, double dstStart, double dstEnd, ListValue* lv);

// require returns resultBelow if value is below threshold, and resultAbove otherwise.
double require(double value, double threshold, double resultBelow, double resultAbove);
// With two arguments, the default result below is 0.0
double require(double value, double threshold, double reslutAbove);
// With one argument, the default result above is 1.0
double require(double value, double threshold);


BasicOperationProvider* createProvider();
