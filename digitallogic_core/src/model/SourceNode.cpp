#include "digitallogic/model/SourceNode.h"

namespace digitallogic {

SourceNode::SourceNode(const ComponentId id, const SignalValue initialValue)
    : m_id(id)
    , m_value(initialValue)
{
}

void SourceNode::toggle() noexcept
{
    m_value = (m_value == SignalValue::True) ? SignalValue::False : SignalValue::True;
}

} // namespace digitallogic
