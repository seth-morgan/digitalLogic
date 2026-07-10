#include "digitallogic/model/WireValidation.h"

namespace digitallogic {

const char* wireValidationMessage(const WireValidationResult result)
{
    switch (result) {
    case WireValidationResult::Ok:
        return "OK";
    case WireValidationResult::SameComponent:
        return "Cannot connect a component to itself.";
    case WireValidationResult::InvalidDestination:
        return "Wires must end on a gate or output input pin.";
    case WireValidationResult::InvalidSource:
        return "Wires must start from a source or gate output pin.";
    case WireValidationResult::InputAlreadyConnected:
        return "That gate input already has a wire.";
    default:
        return "Invalid wire connection.";
    }
}

} // namespace digitallogic
