#ifndef QRIBBONMENU_H
#define QRIBBONMENU_H

#include "RibbonLibGlobal.h"

#include <QMenu>

class RIBBONLIB_EXPORT QRibbonMenu : public QMenu
{
    Q_OBJECT

public:
    explicit QRibbonMenu(QWidget *parent = nullptr);
    explicit QRibbonMenu(const QString &title, QWidget *parent = nullptr);
};

#endif // QRIBBONMENU_H
