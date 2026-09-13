#ifndef QRIBBONSTYLE_H
#define QRIBBONSTYLE_H

#include <QString>

class QMenu;
class QWidget;

class QRibbonStyle
{
public:
    static QString defaultStyleSheet();
    static void applyDefaultStyle(QWidget *ribbonRoot);
    static void applyMenuStyle(QMenu *menu);

private:
    static void ensureResources();
};

#endif // QRIBBONSTYLE_H
