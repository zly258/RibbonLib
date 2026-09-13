#ifndef QAPPLICATIONBUTTON_H
#define QAPPLICATIONBUTTON_H

#include "RibbonLibGlobal.h"

#include <QToolButton>

class QMenu;

class RIBBONLIB_EXPORT QApplicationButton : public QToolButton
{
    Q_OBJECT

public:
    explicit QApplicationButton(QWidget *parent = nullptr);

    void setApplicationMenu(QMenu *menu);
    QMenu *applicationMenu() const { return m_menu; }

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

private:
    QMenu *m_menu { nullptr };
};

#endif // QAPPLICATIONBUTTON_H
