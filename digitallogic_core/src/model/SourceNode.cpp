/**
 * @file SourceNode.cpp
 * @brief Implements construction and toggle behavior for input sources.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/model/SourceNode.h"

namespace digitallogic {

/**
 * @brief Creates a source with the given identifier and initial value.
 */
SourceNode::SourceNode(const ComponentId id, const SignalValue initialValue)
    : m_id(id)
    , m_value(initialValue)
{
}

/**
 * @brief Toggles between false and true.
 */
void SourceNode::toggle() noexcept
{
    // Flip between the two known logic levels (Unknown is not used for sources).
    m_value = (m_value == SignalValue::True) ? SignalValue::False : SignalValue::True;
}

} // namespace digitallogic
