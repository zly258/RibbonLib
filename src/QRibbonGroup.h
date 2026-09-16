#ifndef QRIBBONGROUP_H
#define QRIBBONGROUP_H

#include "RibbonLibGlobal.h"
#include "QRibbonButtonSize.h"

#include <QFrame>
#include <QList>
#include <QString>

class QAction;
class QGridLayout;
class QHBoxLayout;
class QIcon;
class QLabel;
class QMenu;
class QVBoxLayout;
class QRibbonButton;
class QRibbonSplitButton;

class RIBBONLIB_EXPORT QRibbonGroup : public QFrame
{
    Q_OBJECT
public:
    explicit QRibbonGroup(const QString &title, QWidget *parent = nullptr);

    void addButton(QRibbonButton *button);
    void addButton(const QIcon &icon,
                   const QString &text,
                   QRibbonButtonSize size = QRibbonButtonSize::Large);
    QRibbonButton *addAction(QAction *action,
                             QRibbonButtonSize size = QRibbonButtonSize::Large);
    QRibbonButton *addAction(QAction *action,
                             QRibbonButtonSize size,
                             const QString &displayText);
    void addActions(const QList<QAction*> &actions,
                    QRibbonButtonSize size = QRibbonButtonSize::Small);
    QRibbonSplitButton *addSplitAction(QAction *defaultAction,
                                       QMenu *menu,
                                       QRibbonButtonSize size = QRibbonButtonSize::Large,
                                       const QString &displayText = QString());
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
    int m_contentWidth {0};
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
