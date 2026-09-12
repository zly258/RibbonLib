#include "RibbonAction.h"

RibbonAction::RibbonAction(QObject *parent)
    : QObject(parent)
{
}

QString RibbonAction::name() const
{
    return m_name;
}

void RibbonAction::setName(const QString &name)
{
    if (m_name == name) return;
    m_name = name;
    notifyChanged();
}

QString RibbonAction::description() const
{
    return m_description;
}

void RibbonAction::setDescription(const QString &description)
{
    if (m_description == description) return;
    m_description = description;
    notifyChanged();
}

QIcon RibbonAction::icon() const
{
    return m_icon;
}

void RibbonAction::setIcon(const QIcon &icon)
{
    m_icon = icon;
    notifyChanged();
}

QKeySequence RibbonAction::shortcut() const
{
    return m_shortcut;
}

void RibbonAction::setShortcut(const QKeySequence &shortcut)
{
    if (m_shortcut == shortcut) return;
    m_shortcut = shortcut;
    notifyChanged();
}

QString RibbonAction::defaultParams() const
{
    return m_defaultParams;
}

void RibbonAction::setDefaultParams(const QString &params)
{
    if (m_defaultParams == params) return;
    m_defaultParams = params;
    notifyChanged();
}

bool RibbonAction::isEnabled() const
{
    return m_enabled;
}

void RibbonAction::setEnabled(bool enabled)
{
    if (m_enabled == enabled) return;
    m_enabled = enabled;
    notifyChanged();
}

bool RibbonAction::isVisible() const
{
    return m_visible;
}

void RibbonAction::setVisible(bool visible)
{
    if (m_visible == visible) return;
    m_visible = visible;
    notifyChanged();
}

bool RibbonAction::isCheckable() const
{
    return m_checkable;
}

void RibbonAction::setCheckable(bool checkable)
{
    if (m_checkable == checkable) return;
    m_checkable = checkable;
    notifyChanged();
}

bool RibbonAction::isChecked() const
{
    return m_checked;
}

void RibbonAction::setChecked(bool checked)
{
    if (m_checked == checked) return;
    m_checked = checked;
    notifyChanged();
}

bool RibbonAction::validate() const
{
    return isEnabled() && isVisible();
}

bool RibbonAction::trigger(const QString &params)
{
    const QString actualParams = params.isEmpty() ? defaultParams() : params;
    if (!validate()) {
        emit executeRejected(id(), actualParams);
        return false;
    }

    execute(actualParams);
    emit executed(id(), actualParams);
    return true;
}

void RibbonAction::notifyChanged()
{
    emit changed();
}

void RibbonAction::setErrorMessage(const QString &message)
{
    emit errorMessage(id(), message);
}
