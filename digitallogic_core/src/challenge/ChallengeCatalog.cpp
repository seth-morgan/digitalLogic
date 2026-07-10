#include "digitallogic/challenge/ChallengeCatalog.h"
#include "digitallogic/challenge/ChallengeLevel.h"

namespace digitallogic {

namespace {

QVector<ChallengeLevel> makeLevels()
{
  QVector<ChallengeLevel> levels;

  {
    ChallengeLevel level;
    level.index = 0;
    level.title = QStringLiteral("Level 1: AND Gate");
    level.description = QStringLiteral(
        "Wire inputs A and B through one AND gate so the output lights up only when both inputs are 1.");
    level.sources = {
        ChallengeSourceSpec{QStringLiteral("A"), QPointF(80.0, 120.0)},
        ChallengeSourceSpec{QStringLiteral("B"), QPointF(80.0, 240.0)},
    };
    level.targetPosition = QPointF(900.0, 180.0);
    level.gateAllowances = {{GateKind::And, 1}};
    level.testCases = {
        {{{QStringLiteral("A"), SignalValue::False}, {QStringLiteral("B"), SignalValue::False}}, SignalValue::False},
        {{{QStringLiteral("A"), SignalValue::False}, {QStringLiteral("B"), SignalValue::True}}, SignalValue::False},
        {{{QStringLiteral("A"), SignalValue::True}, {QStringLiteral("B"), SignalValue::False}}, SignalValue::False},
        {{{QStringLiteral("A"), SignalValue::True}, {QStringLiteral("B"), SignalValue::True}}, SignalValue::True},
    };
    levels.push_back(level);
  }

  {
    ChallengeLevel level;
    level.index = 1;
    level.title = QStringLiteral("Level 2: OR Gate");
    level.description = QStringLiteral(
        "Wire inputs A and B through one OR gate so the output lights up when either input is 1.");
    level.sources = {
        ChallengeSourceSpec{QStringLiteral("A"), QPointF(80.0, 120.0)},
        ChallengeSourceSpec{QStringLiteral("B"), QPointF(80.0, 240.0)},
    };
    level.targetPosition = QPointF(900.0, 180.0);
    level.gateAllowances = {{GateKind::Or, 1}};
    level.testCases = {
        {{{QStringLiteral("A"), SignalValue::False}, {QStringLiteral("B"), SignalValue::False}}, SignalValue::False},
        {{{QStringLiteral("A"), SignalValue::False}, {QStringLiteral("B"), SignalValue::True}}, SignalValue::True},
        {{{QStringLiteral("A"), SignalValue::True}, {QStringLiteral("B"), SignalValue::False}}, SignalValue::True},
        {{{QStringLiteral("A"), SignalValue::True}, {QStringLiteral("B"), SignalValue::True}}, SignalValue::True},
    };
    levels.push_back(level);
  }

  return levels;
}

} // namespace

const QVector<ChallengeLevel>& ChallengeCatalog::levels()
{
  static const QVector<ChallengeLevel> kLevels = makeLevels();
  return kLevels;
}

int ChallengeCatalog::levelCount() noexcept
{
  return levels().size();
}

const ChallengeLevel* ChallengeCatalog::levelAt(const int index)
{
  const QVector<ChallengeLevel>& all = levels();
  if (index < 0 || index >= all.size()) {
    return nullptr;
  }
  return &all.at(index);
}

} // namespace digitallogic
