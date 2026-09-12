#ifndef QAPPLICATIONBUTTON_H
#define QAPPLICATIONBUTTON_H

/*
 * QApplicationButton
 * ------------------------------------------------------------
 * Ribbon top-left application menu button (Office-style "File" button).
 *
 * Typically displays "File" or localized text, with width automatically
 * calculated based on the text content.
 */

#include "RibbonLibGlobal.h"

#include <QToolButton>
#include <QMenu>

/*
 * QApplicationButton
 * Application button at the top-left of the Ribbon, encapsulating menu setup and styling.
 */
class RIBBONLIB_EXPORT QApplicationButton : public QToolButton
{
    Q_OBJECT
public:
    explicit QApplicationButton(QWidget *parent = nullptr);

    void setApplicationMenu(QMenu *menu);
    QMenu *applicationMenu() const { return m_menu; }

public:
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QMenu *m_menu { nullptr };
};

#endif // QAPPLICATIONBUTTON_H
