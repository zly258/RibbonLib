#ifndef QRIBBONTAB_H
#define QRIBBONTAB_H

#include "RibbonLibGlobal.h"

#include <QHBoxLayout>
#include <QList>
#include <QScrollArea>
#include <QString>
#include <QWidget>

class QRibbonGroup;

class RIBBONLIB_EXPORT QRibbonTab : public QWidget
{
    Q_OBJECT
public:
    explicit QRibbonTab(const QString &title, QWidget *parent = nullptr);

    QRibbonGroup *addGroup(const QString &title);
    void removeGroup(QRibbonGroup *group);
    void clear();
    void refreshLayout();

    QString id() const { return m_id; }
    void setId(const QString &id) { m_id = id; }

    QString title() const { return m_title; }

private:
    void setupUI();
    void updateContentWidth();
    void scheduleContentWidthUpdate();
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

    QString m_id;
    QString m_title;
    QScrollArea *m_scrollArea {nullptr};
    QWidget *m_content {nullptr};
    QHBoxLayout *m_layout {nullptr};
    QList<QRibbonGroup*> m_groups;
    bool m_widthUpdatePending { false };
};

#endif // QRIBBONTAB_H
