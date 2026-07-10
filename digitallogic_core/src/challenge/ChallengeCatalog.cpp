#include "digitallogic/challenge/ChallengeCatalog.h"
#include "digitallogic/challenge/ChallengeLevel.h"

namespace digitallogic {

namespace {

QVector<ChallengeTestCase> twoInputAndCases()
{
    return {
        {{{QStringLiteral("A"), SignalValue::False}, {QStringLiteral("B"), SignalValue::False}}, SignalValue::False},
        {{{QStringLiteral("A"), SignalValue::False}, {QStringLiteral("B"), SignalValue::True}}, SignalValue::False},
        {{{QStringLiteral("A"), SignalValue::True}, {QStringLiteral("B"), SignalValue::False}}, SignalValue::False},
        {{{QStringLiteral("A"), SignalValue::True}, {QStringLiteral("B"), SignalValue::True}}, SignalValue::True},
    };
}

QVector<ChallengeTestCase> twoInputOrCases()
{
    return {
        {{{QStringLiteral("A"), SignalValue::False}, {QStringLiteral("B"), SignalValue::False}}, SignalValue::False},
        {{{QStringLiteral("A"), SignalValue::False}, {QStringLiteral("B"), SignalValue::True}}, SignalValue::True},
        {{{QStringLiteral("A"), SignalValue::True}, {QStringLiteral("B"), SignalValue::False}}, SignalValue::True},
        {{{QStringLiteral("A"), SignalValue::True}, {QStringLiteral("B"), SignalValue::True}}, SignalValue::True},
    };
}

QVector<ChallengeTestCase> twoInputXorCases()
{
    return {
        {{{QStringLiteral("A"), SignalValue::False}, {QStringLiteral("B"), SignalValue::False}}, SignalValue::False},
        {{{QStringLiteral("A"), SignalValue::False}, {QStringLiteral("B"), SignalValue::True}}, SignalValue::True},
        {{{QStringLiteral("A"), SignalValue::True}, {QStringLiteral("B"), SignalValue::False}}, SignalValue::True},
        {{{QStringLiteral("A"), SignalValue::True}, {QStringLiteral("B"), SignalValue::True}}, SignalValue::False},
    };
}

QVector<ChallengeTestCase> singleInputNotCases()
{
    return {
        {{{QStringLiteral("A"), SignalValue::False}}, SignalValue::True},
        {{{QStringLiteral("A"), SignalValue::True}}, SignalValue::False},
    };
}

QVector<ChallengeSourceSpec> twoSources()
{
    return {
        ChallengeSourceSpec{QStringLiteral("A"), QPointF(80.0, 120.0)},
        ChallengeSourceSpec{QStringLiteral("B"), QPointF(80.0, 240.0)},
    };
}

QVector<ChallengeLevel> makeLevels()
{
    QVector<ChallengeLevel> levels;

    {
        ChallengeLevel level;
        level.index = 0;
        level.title = QStringLiteral("Level 1: AND Gate");
        level.description = QStringLiteral(
            "Wire inputs A and B through one AND gate so the output lights up only when both inputs are 1.");
        level.sources = twoSources();
        level.targetPosition = QPointF(900.0, 180.0);
        level.gateAllowances = {{GateKind::And, 1}};
        level.testCases = twoInputAndCases();
        levels.push_back(level);
    }

    {
        ChallengeLevel level;
        level.index = 1;
        level.title = QStringLiteral("Level 2: OR Gate");
        level.description = QStringLiteral(
            "Wire inputs A and B through one OR gate so the output lights up when either input is 1.");
        level.sources = twoSources();
        level.targetPosition = QPointF(900.0, 180.0);
        level.gateAllowances = {{GateKind::Or, 1}};
        level.testCases = twoInputOrCases();
        levels.push_back(level);
    }

    {
        ChallengeLevel level;
        level.index = 2;
        level.title = QStringLiteral("Level 3: XOR Gate");
        level.description = QStringLiteral(
            "Wire inputs A and B through one XOR gate so the output lights up when the inputs differ.");
        level.sources = twoSources();
        level.targetPosition = QPointF(900.0, 180.0);
        level.gateAllowances = {{GateKind::Xor, 1}};
        level.testCases = twoInputXorCases();
        levels.push_back(level);
    }

    {
        ChallengeLevel level;
        level.index = 3;
        level.title = QStringLiteral("Level 4: NOT from NAND");
        level.description = QStringLiteral(
            "Build a NOT gate using one NAND. Connect input A to both NAND inputs; the output is NOT A.");
        level.sources = {ChallengeSourceSpec{QStringLiteral("A"), QPointF(80.0, 180.0)}};
        level.targetPosition = QPointF(900.0, 180.0);
        level.gateAllowances = {{GateKind::Nand, 1}};
        level.testCases = singleInputNotCases();
        levels.push_back(level);
    }

    {
        ChallengeLevel level;
        level.index = 4;
        level.title = QStringLiteral("Level 5: AND from NAND");
        level.description = QStringLiteral(
            "Build A AND B using only NAND gates. Hint: NAND followed by NOT (another NAND) gives AND.");
        level.sources = twoSources();
        level.targetPosition = QPointF(900.0, 180.0);
        level.gateAllowances = {{GateKind::Nand, 2}};
        level.testCases = twoInputAndCases();
        levels.push_back(level);
    }

    {
        ChallengeLevel level;
        level.index = 5;
        level.title = QStringLiteral("Level 6: OR from NAND");
        level.description = QStringLiteral(
            "Build A OR B using only NAND gates. Hint: De Morgan's law — invert each input, then NAND.");
        level.sources = twoSources();
        level.targetPosition = QPointF(900.0, 180.0);
        level.gateAllowances = {{GateKind::Nand, 3}};
        level.testCases = twoInputOrCases();
        levels.push_back(level);
    }

    {
        ChallengeLevel level;
        level.index = 6;
        level.title = QStringLiteral("Level 7: XOR from NAND");
        level.description = QStringLiteral(
            "Build A XOR B using only NAND gates. You have exactly four NAND gates to work with.");
        level.sources = twoSources();
        level.targetPosition = QPointF(900.0, 180.0);
        level.gateAllowances = {{GateKind::Nand, 4}};
        level.testCases = twoInputXorCases();
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
