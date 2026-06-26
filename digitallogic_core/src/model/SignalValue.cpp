#include "digitallogic/model/SignalValue.h"

namespace digitallogic {

const char* toString(const SignalValue value)
{
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
