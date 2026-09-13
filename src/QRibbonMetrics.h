#ifndef QRIBBONMETRICS_H
#define QRIBBONMETRICS_H

#include <QSize>

namespace QRibbonMetrics
{
    constexpr int TopBarHeight = 29;
    constexpr int ContentHeight = 96;
    constexpr int RibbonHeight = TopBarHeight + ContentHeight;

    constexpr int GroupContentHeight = 72;
    constexpr int GroupTitleHeight = 18;
    constexpr int GroupSeparatorHeight = 68;
    constexpr int GroupHorizontalPadding = 4;
    constexpr int GroupContentSpacing = 2;
    constexpr int TabGroupSpacing = 6;
    constexpr int SmallRowHeight = 22;
    constexpr int SmallRowCount = 3;
    constexpr int SmallRowSpacing = 1;

    constexpr int LargeIconSize = 32;
    constexpr int SmallIconSize = 16;
    constexpr int AccessIconSize = 16;

    constexpr int LargeButtonHeight = 72;
    constexpr int SmallButtonHeight = 22;
    constexpr int AccessButtonHeight = 24;
    constexpr int LargeButtonMinWidth = 44;
    constexpr int SmallButtonMinWidth = 28;
    constexpr int SplitButtonMinWidth = 40;
    constexpr int LargeButtonMinHeight = 68;
    constexpr int SmallButtonMinHeight = 22;

    constexpr int LargeButtonHPadding = 10;
    constexpr int SmallButtonHPadding = 8;
    constexpr int ButtonTextSpacing = 6;
    constexpr int ButtonSidePadding = 6;
    constexpr int LargeIconTop = 6;
    constexpr int LargeTextTop = 43;
    constexpr int SplitArrowWidth = 16;

    inline QSize largeIconSize() { return QSize(LargeIconSize, LargeIconSize); }
    inline QSize smallIconSize() { return QSize(SmallIconSize, SmallIconSize); }
    inline QSize accessIconSize() { return QSize(AccessIconSize, AccessIconSize); }
}

#endif // QRIBBONMETRICS_H
