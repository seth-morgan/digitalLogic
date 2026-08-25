/**
 * @file SignalValue.cpp
 * @brief Implements string conversion helpers for SignalValue.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/model/SignalValue.h"

namespace digitallogic {

const char* toString(const SignalValue value)
{
    // Map enum values to short labels shown in the UI.
    switch (value) {
    case SignalValue::False:
        return "0";
    case SignalValue::True:
        return "1";
    case SignalValue::Unknown:
    default:
        return "?";
    }
}

} // namespace digitallogic
