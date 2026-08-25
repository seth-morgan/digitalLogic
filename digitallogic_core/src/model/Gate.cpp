/**
 * @file Gate.cpp
 * @brief Implements the Gate base-class constructor shared by all gate types.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/model/Gate.h"

namespace digitallogic {

Gate::Gate(const ComponentId id, const GateKind kind, const int inputCount)
    : m_id(id)
    , m_kind(kind)
    , m_inputCount(inputCount)
{
}

} // namespace digitallogic
