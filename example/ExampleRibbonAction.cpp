#include "ExampleRibbonAction.h"

ExampleRibbonAction::ExampleRibbonAction(const QString &id,
                                         const QString &name,
                                         QTextEdit *output,
                                         QObject *parent)
    : RibbonAction(parent)
    , m_id(id)
    , m_output(output)
{
    setName(name);
}

QString ExampleRibbonAction::id() const
{
    return m_id;
}

void ExampleRibbonAction::execute(const QString &params)
{
    Q_UNUSED(params)
    if (m_output) {
        m_output->append(QString("Execute RibbonAction: %1").arg(id()));
    }
}

void ExampleRibbonAction::cleanup()
{
}
