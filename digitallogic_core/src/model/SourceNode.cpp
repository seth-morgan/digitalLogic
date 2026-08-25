/**
 * @file SourceNode.cpp
 * @brief Implements construction and toggle behavior for input sources.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/model/SourceNode.h"

namespace digitallogic {

SourceNode::SourceNode(const ComponentId id, const SignalValue initialValue)
    : m_id(id)
    , m_value(initialValue)
{
}

void SourceNode::toggle() noexcept
{
    // Flip between the two known logic levels (Unknown is not used for sources).
    m_value = (m_value == SignalValue::True) ? SignalValue::False : SignalValue::True;
}

} // namespace digitallogic
