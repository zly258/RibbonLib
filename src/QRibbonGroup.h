#ifndef QRIBBONGROUP_H
#define QRIBBONGROUP_H

#include "RibbonLibGlobal.h"
#include "QRibbonButtonSize.h"

#include <QFrame>
#include <QString>

class QGridLayout;
class QHBoxLayout;
class QIcon;
class QLabel;
class QVBoxLayout;
class QRibbonButton;

class RIBBONLIB_EXPORT QRibbonGroup : public QFrame
{
    Q_OBJECT
public:
    explicit QRibbonGroup(const QString &title, QWidget *parent = nullptr);

    void addButton(QRibbonButton *button);
    void addButton(const QIcon &icon,
                   const QString &text,
                   QRibbonButtonSize size = QRibbonButtonSize::Large);
    void addSeparator();
    void addWidget(QWidget *widget);
    void addLargeWidget(QWidget *widget);
    void addSmallWidget(QWidget *widget);

    void setTitle(const QString &title);
    QString title() const { return m_title; }
    bool isEmpty() const { return m_contentItemCount <= 0; }

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void layoutChanged();

private:
    void setupLayout();
    void updateContentWidth();
    void markContentAdded();

    QString m_title;
    int m_contentWidth {45};
    int m_contentItemCount {0};
    QLabel *m_titleLabel {nullptr};
    QVBoxLayout *m_mainLayout {nullptr};
    QWidget *m_contentWidget {nullptr};
    QHBoxLayout *m_contentLayout {nullptr};
    QWidget *m_currentSmallColumnWidget {nullptr};
    QGridLayout *m_currentSmallColumnLayout {nullptr};
    int m_currentSmallRow {0};
};

#endif // QRIBBONGROUP_H
