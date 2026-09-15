#ifndef QRIBBONMETRICS_H
#define QRIBBONMETRICS_H

#include <QSize>

namespace QRibbonMetrics
{
    constexpr int TopBarHeight = 29;
    constexpr int ContentHeight = 96;
    constexpr int RibbonHeight = TopBarHeight + ContentHeight;

    constexpr int SmallRowHeight = 22;
    constexpr int SmallRowCount = 3;
    constexpr int SmallRowSpacing = 2;
    constexpr int SmallColumnVerticalPadding = 2;
    constexpr int SmallColumnHeight =
        SmallColumnVerticalPadding * 2
        + SmallRowHeight * SmallRowCount
        + SmallRowSpacing * (SmallRowCount - 1);

    constexpr int GroupContentHeight = SmallColumnHeight;
    constexpr int GroupTitleHeight = 18;
    constexpr int GroupTopPadding = 2;
    constexpr int GroupBottomPadding =
        ContentHeight - GroupTopPadding - GroupContentHeight - GroupTitleHeight;
    constexpr int GroupSeparatorHeight = 68;
    constexpr int GroupHorizontalPadding = 4;
    constexpr int GroupContentSpacing = 1;
    constexpr int TabGroupSpacing = 6;

    constexpr int LargeIconSize = 32;
    constexpr int SmallIconSize = 16;
    constexpr int AccessIconSize = 16;

    constexpr int LargeButtonVerticalPadding = 2;
    constexpr int LargeButtonHeight = GroupContentHeight - LargeButtonVerticalPadding * 2;
    constexpr int SmallButtonHeight = SmallRowHeight;
    constexpr int AccessButtonHeight = 24;
    constexpr int LargeButtonMinWidth = 44;
    constexpr int SmallButtonMinWidth = 28;
    constexpr int SplitButtonMinWidth = 40;
    constexpr int LargeButtonMinHeight = 68;
    constexpr int SmallButtonMinHeight = SmallRowHeight;

    constexpr int LargeButtonHPadding = 8;
    constexpr int SmallButtonHPadding = 8;
    constexpr int ButtonTextSpacing = 6;
    constexpr int ButtonSidePadding = 6;
    constexpr int LargeIconTop = 6;
    constexpr int LargeTextTop = 43;
    constexpr int SplitArrowWidth = 16;

    static_assert(GroupBottomPadding >= 0,
                  "Ribbon group metrics exceed the configured content height");
    static_assert(GroupTopPadding + GroupContentHeight + GroupTitleHeight + GroupBottomPadding
                      == ContentHeight,
                  "Ribbon group vertical metrics must exactly fill the content height");
    static_assert(LargeButtonVerticalPadding * 2 + LargeButtonHeight == GroupContentHeight,
                  "Large buttons and their vertical clearance must exactly fill the group content area");
    static_assert(LargeButtonHeight >= LargeButtonMinHeight,
                  "Large button height must satisfy the configured minimum height");
    static_assert(SmallColumnVerticalPadding * 2
                      + SmallButtonHeight * SmallRowCount
                      + SmallRowSpacing * (SmallRowCount - 1)
                      == GroupContentHeight,
                  "Small rows and their vertical padding must exactly fill the group content area");

    inline QSize largeIconSize() { return QSize(LargeIconSize, LargeIconSize); }
    inline QSize smallIconSize() { return QSize(SmallIconSize, SmallIconSize); }
    inline QSize accessIconSize() { return QSize(AccessIconSize, AccessIconSize); }
}

#endif // QRIBBONMETRICS_H
