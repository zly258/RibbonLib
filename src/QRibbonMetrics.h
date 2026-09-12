#ifndef QRIBBONMETRICS_H
#define QRIBBONMETRICS_H

/*
 * QRibbonMetrics
 * ------------------------------------------------------------
 * Ribbon layout dimension and metric constants.
 *
 * This header defines structural dimensions only. Colors, borders, hover,
 * pressed, and checked states are strictly managed via QSS.
 */

#include <QSize>

namespace QRibbonMetrics
{
    constexpr int LargeIconSize = 32;
    constexpr int SmallIconSize = 16;
    constexpr int AccessIconSize = 16;

    constexpr int LargeButtonMinWidth = 44;
    constexpr int SmallButtonMinWidth = 28;
    constexpr int SplitButtonMinWidth = 34;

    constexpr int LargeButtonMinHeight = 68;
    constexpr int SmallButtonMinHeight = 24;
    constexpr int AccessButtonSize = 22;

    constexpr int LargeButtonHPadding = 10;
    constexpr int SmallButtonHPadding = 8;
    constexpr int ButtonTextSpacing = 6;
    constexpr int SplitArrowWidth = 14;

    inline QSize largeIconSize() { return QSize(LargeIconSize, LargeIconSize); }
    inline QSize smallIconSize() { return QSize(SmallIconSize, SmallIconSize); }
    inline QSize accessIconSize() { return QSize(AccessIconSize, AccessIconSize); }
}

#endif // QRIBBONMETRICS_H
