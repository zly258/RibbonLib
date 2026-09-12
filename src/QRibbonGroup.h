#ifndef QRIBBONGROUP_H
#define QRIBBONGROUP_H

/*
 * QRibbonGroup
 * ------------------------------------------------------------
 * Ribbon group panel.
 *
 * Group width is determined naturally by its internal buttons, separators,
 * and title text width. The group itself does not enforce a fixed width;
 * excess horizontal space is absorbed by the stretch item in QRibbonTab.
 */

#include "RibbonLibGlobal.h"

#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QString>

#include "QRibbonButton.h"

// Ribbon group: hosts Large and Small buttons as well as custom widgets
class RIBBONLIB_EXPORT QRibbonGroup : public QFrame
{
    Q_OBJECT
public:
    explicit QRibbonGroup(const QString &title, QWidget *parent = nullptr);

    void addButton(QRibbonButton *button);
    void addButton(const QIcon &icon, const QString &text, QRibbonButtonSize size = QRibbonButtonSize::Large);
    void addSeparator();
    void addWidget(QWidget *widget);
    // Inserts arbitrary QWidgets into Large or Small column positions to preserve ordering
    void addLargeWidget(QWidget *widget);
    void addSmallWidget(QWidget *widget);

    void setTitle(const QString &title);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    QString title() const { return m_title; }
    bool isEmpty() const { return m_contentItemCount <= 0; }

signals:
    void layoutChanged();

private:
    void setupLayout();
    void updateContentWidth();
    void markContentAdded();

    QString m_title;
    int m_contentWidth { 45 };
    int m_contentItemCount { 0 };
    QLabel *m_titleLabel {nullptr};
    QVBoxLayout *m_mainLayout {nullptr};
    QWidget *m_contentWidget {nullptr};
    QHBoxLayout *m_contentLayout {nullptr};
    // Column model: horizontally sequenced columns
    // - Large button: takes a single full-height column (height 72)
    // - Small button: placed in current 3-row column; opens new column when full
    QWidget *m_currentSmallColumnWidget {nullptr};
    QGridLayout *m_currentSmallColumnLayout {nullptr};
    int m_currentSmallRow {0};
};

#endif // QRIBBONGROUP_H
