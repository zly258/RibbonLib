#ifndef QRIBBONSPLITBUTTON_H
#define QRIBBONSPLITBUTTON_H

/*
 * QRibbonSplitButton
 * ------------------------------------------------------------
 * Ribbon split button.
 *
 * The left area triggers the default action, and the right area
 * triggers the drop-down menu.
 * Width is calculated naturally from icon, text, arrow, and padding.
 */

#include "RibbonLibGlobal.h"

#include <QWidget>
#include <QIcon>
#include <QString>
#include <QSize>
#include <QRect>

#include "QRibbonButtonSize.h"

class QMenu;
class QAction;

// Ribbon split button: main body executes default action; right arrow opens menu
class RIBBONLIB_EXPORT QRibbonSplitButton : public QWidget
{
    Q_OBJECT
public:
    explicit QRibbonSplitButton(QWidget *parent = nullptr);
    QRibbonSplitButton(const QIcon &icon, const QString &text, QRibbonButtonSize size = QRibbonButtonSize::Large, QWidget *parent = nullptr);
    ~QRibbonSplitButton();

    void setButtonSize(QRibbonButtonSize size);
    QRibbonButtonSize buttonSize() const { return m_size; }

    void setIcon(const QIcon &icon);
    void setText(const QString &text);
    void setCheckable(bool checkable);
    void setChecked(bool checked);
    void setMenu(QMenu *menu);
    void setDefaultAction(QAction *action);

    QIcon icon() const { return m_icon; }
    QSize iconSize() const { return m_iconSize; }
    QString text() const { return m_text; }
    QMenu* menu() const { return m_menu; }
    bool isChecked() const { return m_checked; }
    bool isCheckable() const { return m_checkable; }

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void clicked();
    void toggled(bool checked);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void changeEvent(QEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override { return QWidget::eventFilter(obj, event); }

    QRect getIconRect() const;
    QRect getTextRect() const;
    QRect getDropArrowRect() const;
    bool isInArrowArea(const QPoint &pos) const;

private slots:
    void onMenuActionTriggered(QAction *action);

private:
    void updateIconSize();
    QIcon effectiveIcon() const;
    void updateHoverState(const QPoint &pos);
    void updateMenuIconTargetSize();
    void updateStateProperties();

    QRibbonButtonSize m_size { QRibbonButtonSize::Large };
    QIcon m_icon;
    QString m_text;
    QSize m_iconSize { 32, 32 };
    bool m_checkable { false };
    bool m_checked { false };
    bool m_hovered { false };
    bool m_pressed { false };
    bool m_arrowPressed { false };
    bool m_arrowHovered { false };
    QMenu *m_menu { nullptr };
};

#endif // QRIBBONSPLITBUTTON_H