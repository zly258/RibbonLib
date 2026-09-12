#ifndef RIBBONACTION_H
#define RIBBONACTION_H

#include "RibbonLibGlobal.h"

#include <QObject>
#include <QIcon>
#include <QKeySequence>
#include <QString>

/*
 * RibbonAction
 * ------------------------------------------------------------
 * Base action abstraction for RibbonLib.
 *
 * Design goals:
 * 1. Provides a generic action interface independent of any application-level
 *    Document, Viewport, or ApplicationContext.
 * 2. Applications can subclass RibbonAction and implement execute(params) and cleanup().
 * 3. QRibbonHelper binds RibbonAction instances to internal QActions.
 * 4. Button click -> QAction::triggered -> RibbonAction::execute(params).
 */
class RIBBONLIB_EXPORT RibbonAction : public QObject
{
    Q_OBJECT

public:
    explicit RibbonAction(QObject *parent = nullptr);
    ~RibbonAction() override = default;

    // Basic action properties
    virtual QString id() const = 0;

    QString name() const;
    void setName(const QString &name);

    QString description() const;
    void setDescription(const QString &description);

    QIcon icon() const;
    void setIcon(const QIcon &icon);

    QKeySequence shortcut() const;
    void setShortcut(const QKeySequence &shortcut);

    QString defaultParams() const;
    void setDefaultParams(const QString &params);

    bool isEnabled() const;
    void setEnabled(bool enabled);

    bool isVisible() const;
    void setVisible(bool visible);

    bool isCheckable() const;
    void setCheckable(bool checkable);

    bool isChecked() const;
    void setChecked(bool checked);

    // Pre-execution validation. Subclasses can override (e.g. check document state, selection).
    virtual bool validate() const;

    // Unified execution entry point: validate -> execute -> emit executed / executeRejected.
    bool trigger(const QString &params = QString());

public slots:
    // Execution interface
    virtual void execute(const QString &params) = 0;

    // Cleanup interface for releasing transient state, canceling previews, or exiting interactions.
    virtual void cleanup() = 0;

signals:
    // Emitted when properties change; QRibbonHelper syncs changes to QAction/Button.
    void changed();

    // Execution notifications for logging, status bar updates, etc.
    void executed(const QString &id, const QString &params);
    void executeRejected(const QString &id, const QString &params);
    void errorMessage(const QString &id, const QString &message);

protected:
    void notifyChanged();
    void setErrorMessage(const QString &message);

private:
    QString m_name;
    QString m_description;
    QIcon m_icon;
    QKeySequence m_shortcut;
    QString m_defaultParams;
    bool m_enabled { true };
    bool m_visible { true };
    bool m_checkable { false };
    bool m_checked { false };
};

#endif // RIBBONACTION_H
