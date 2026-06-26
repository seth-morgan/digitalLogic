#include "digitallogic/model/Gate.h"

namespace digitallogic {

Gate::Gate(const ComponentId id, const GateKind kind, const int inputCount)
    : m_id(id)
    , m_kind(kind)
    , m_inputCount(inputCount)
{
}

} // namespace digitallogic
