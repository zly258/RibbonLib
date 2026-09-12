#ifndef EXAMPLERIBBONACTION_H
#define EXAMPLERIBBONACTION_H

#include "../src/RibbonAction.h"

#include <QTextEdit>

// Sample custom RibbonAction implementation demonstrating business binding
class ExampleRibbonAction : public RibbonAction
{
    Q_OBJECT

public:
    ExampleRibbonAction(const QString &id,
                        const QString &name,
                        QTextEdit *output,
                        QObject *parent = nullptr);

    QString id() const override;

public slots:
    void execute(const QString &params) override;
    void cleanup() override;

private:
    QString m_id;
    QTextEdit *m_output { nullptr };
};

#endif // EXAMPLERIBBONACTION_H
