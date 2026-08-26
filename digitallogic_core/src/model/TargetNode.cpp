/**
 * @file TargetNode.cpp
 * @brief Implements the challenge output target node.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/model/TargetNode.h"

namespace digitallogic {

/**
 * @brief Creates a target node with the given component identifier.
 */
TargetNode::TargetNode(const ComponentId id)
    : m_id(id)
{
}

} // namespace digitallogic
