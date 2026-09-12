#ifndef QRIBBONBUTTON_H
#define QRIBBONBUTTON_H

/*
 * QRibbonButton
 * ------------------------------------------------------------
 * Standard Ribbon button.
 *
 * Supports two display sizes: Large and Small.
 * Width is calculated naturally from icon, text, and padding without fixed constraints.
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
class QEnterEvent;
#endif

// Ribbon button widget
class RIBBONLIB_EXPORT QRibbonButton : public QWidget
{
    Q_OBJECT
public:
    explicit QRibbonButton(QWidget *parent = nullptr);
    QRibbonButton(const QIcon &icon, const QString &text, QRibbonButtonSize size = QRibbonButtonSize::Large, QWidget *parent = nullptr);

    void setButtonSize(QRibbonButtonSize size);
    QRibbonButtonSize buttonSize() const;

    void setIcon(const QIcon &icon);
    void setText(const QString &text);
    void setIconName(const QString &iconName);

    QIcon icon() const { return m_icon; }
    QSize iconSize() const { return m_iconSize; }
    QString text() const { return m_text; }

    void setEnabled(bool enabled);

    void setCheckable(bool checkable);
    bool isCheckable() const { return m_checkable; }

    void setChecked(bool checked);
    bool isChecked() const { return m_checked; }

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void clicked();
    void toggled(bool checked);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *event) override;
#else
    void enterEvent(QEvent *event) override;
#endif
    void leaveEvent(QEvent *event) override;
    void changeEvent(QEvent *event) override;

    QRect getIconRect() const;
    QRect getTextRect() const;

private slots:
    void onThemeChanged(bool isDark);

private:
    void updateIconSize();
    void setupButton();
    void updateIconForTheme();
    QIcon effectiveIcon() const;
    void updateStateProperties();

    QRibbonButtonSize m_size { QRibbonButtonSize::Large };
    QIcon m_icon;
    QString m_text;
    QString m_iconName;
    QSize m_iconSize { 32, 32 };

    bool m_checkable { false };
    bool m_checked { false };
    bool m_hovered { false };
    bool m_pressed { false };
};

#endif // QRIBBONBUTTON_H