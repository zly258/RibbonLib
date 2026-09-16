#ifndef QRIBBONBUTTON_H
#define QRIBBONBUTTON_H

#include "RibbonLibGlobal.h"
#include "QRibbonButtonSize.h"

#include <QAction>
#include <QIcon>
#include <QKeySequence>
#include <QPointer>
#include <QRect>
#include <QSize>
#include <QString>
#include <QWidget>

class RIBBONLIB_EXPORT QRibbonButton : public QWidget
{
    Q_OBJECT

public:
    explicit QRibbonButton(QWidget *parent = nullptr);
    QRibbonButton(const QIcon &icon,
                  const QString &text,
                  QRibbonButtonSize size = QRibbonButtonSize::Large,
                  QWidget *parent = nullptr);
    QRibbonButton(QAction *action,
                  QRibbonButtonSize size = QRibbonButtonSize::Large,
                  QWidget *parent = nullptr);

    void setButtonSize(QRibbonButtonSize size);
    QRibbonButtonSize buttonSize() const;

    void setIcon(const QIcon &icon);
    void setText(const QString &text);

    QIcon icon() const { return m_icon; }
    QSize iconSize() const { return m_iconSize; }
    QString text() const { return m_text; }

    void setEnabled(bool enabled);

    void setCheckable(bool checkable);
    bool isCheckable() const { return m_checkable; }

    void setChecked(bool checked);
    bool isChecked() const { return m_checked; }

    void setShortcut(const QKeySequence &shortcut);
    QKeySequence shortcut() const;

    void setDefaultAction(QAction *action);
    QAction *defaultAction() const { return m_action.data(); }

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void clicked();
    void toggled(bool checked);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void changeEvent(QEvent *event) override;

    QRect getIconRect() const;
    QRect getTextRect() const;

private:
    void updateIconSize();
    QIcon effectiveIcon() const;
    void setupButton();
    void updateStateProperties();
    void syncFromAction();

    QRibbonButtonSize m_size {QRibbonButtonSize::Large};
    QIcon m_icon;
    QString m_text;
    QString m_shortcutText;
    QSize m_iconSize {32, 32};
    QPointer<QAction> m_action;

    bool m_checkable {false};
    bool m_checked {false};
    bool m_pressed {false};
};

#endif // QRIBBONBUTTON_H
