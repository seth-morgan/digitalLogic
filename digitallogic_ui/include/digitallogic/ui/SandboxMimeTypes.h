/**
 * @file SandboxMimeTypes.h
 * @brief Shared drag-and-drop MIME constants for the sandbox canvas.
 * @author Seth Morgan
 * @date 2026-08-26
 */
#pragma once

namespace digitallogic::ui {

// Gate palette drag payload: one byte encoding GateKind.
inline constexpr char kGateMimeType[] = "application/x-digitallogic-gate";

} // namespace digitallogic::ui
