#include <QAction>
#include <QApplication>
#include <QDate>
#include <QMainWindow>
#include <QMessageBox>
#include <QStatusBar>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <RibbonLib.h>

class DemoWindow final : public QMainWindow
{
public:
    DemoWindow()
        : m_helper(new QRibbonHelper(this))
    {
        resize(1180, 760);

        m_editor = new QTextEdit(this);
        m_editor->setStyleSheet(QStringLiteral(
            "QTextEdit { background: #f7f8fa; color: #202124; border: none; padding: 12px; }"));
        m_editor->setPlainText(QStringLiteral(
            "RibbonLib Example\n\n"
            "This example is built from JSON and uses only RibbonLib plus Qt Widgets.\n"
            "Use Home, Insert and View for normal scenarios. Open Tests for checkable rows, disabled states, mixed large/small layouts, split buttons, long labels and Qt native icons.\n\n"
            "All example icons come from Qt standard icons, so Windows uses the active native Qt/Windows style."));
        setCentralWidget(m_editor);

        m_ribbonHost = new QWidget(this);
        m_ribbonLayout = new QVBoxLayout(m_ribbonHost);
        m_ribbonLayout->setContentsMargins(0, 0, 0, 0);
        m_ribbonLayout->setSpacing(0);
        setMenuWidget(m_ribbonHost);

        connect(m_helper, &QRibbonHelper::actionTriggered, this, [this](const QString &id) {
            handleAction(id);
        });

        rebuildRibbon();
    }

private:
    bool isChinese() const
    {
        return m_language == QStringLiteral("zh-CN");
    }

    void rebuildRibbon()
    {
        if (m_ribbon) {
            delete m_ribbon;
            m_ribbon = nullptr;
        }

        const QString suffix = isChinese() ? QStringLiteral("zh-CN") : QStringLiteral("en");
        const QString actionsPath = QStringLiteral(":/RibbonExample/actions_%1.json").arg(suffix);
        const QString ribbonPath = QStringLiteral(":/RibbonExample/ribbon_%1.json").arg(suffix);

        if (!m_helper->loadFromResources(ribbonPath, actionsPath)) {
            statusBar()->showMessage(m_helper->errorString());
            return;
        }

        m_ribbon = new QRibbonWidget(m_ribbonHost);
        m_ribbonLayout->addWidget(m_ribbon);

        if (!m_helper->buildRibbon(m_ribbon)) {
            statusBar()->showMessage(m_helper->errorString());
            return;
        }

        setWindowTitle(isChinese()
            ? QStringLiteral("RibbonLib 示例")
            : QStringLiteral("RibbonLib Example"));
        statusBar()->showMessage(isChinese()
            ? QStringLiteral("Ribbon 已从中文 JSON 配置加载")
            : QStringLiteral("Ribbon loaded from English JSON configuration"),
            2500);
    }

    void scheduleLanguageSwitch()
    {
        m_language = isChinese() ? QStringLiteral("en") : QStringLiteral("zh-CN");
        QTimer::singleShot(0, this, [this]() {
            rebuildRibbon();
        });
    }

    void resetZoom()
    {
        if (m_zoomSteps > 0) {
            m_editor->zoomOut(m_zoomSteps);
        } else if (m_zoomSteps < 0) {
            m_editor->zoomIn(-m_zoomSteps);
        }
        m_zoomSteps = 0;
    }

    void showSimpleStatus(const QString &english, const QString &chinese)
    {
        statusBar()->showMessage(isChinese() ? chinese : english, 2500);
    }

    void handleAction(const QString &id)
    {
        if (id == QStringLiteral("language.switch")) {
            scheduleLanguageSwitch();
            return;
        }
        if (id == QStringLiteral("file.new")) {
            m_editor->clear();
            showSimpleStatus(QStringLiteral("New document"), QStringLiteral("已新建文档"));
            return;
        }
        if (id == QStringLiteral("file.open")) {
            m_editor->setPlainText(isChinese()
                ? QStringLiteral("示例文档\n\n这是通过 Ribbon 的“打开”命令载入的示例内容。\nRibbon 布局、动作文本和语言均来自 JSON。")
                : QStringLiteral("Sample document\n\nThis content was loaded by the Ribbon Open command.\nRibbon layout, action text and language come from JSON."));
            showSimpleStatus(QStringLiteral("Sample document opened"), QStringLiteral("已打开示例文档"));
            return;
        }
        if (id == QStringLiteral("file.save")) {
            showSimpleStatus(QStringLiteral("Document saved"), QStringLiteral("文档已保存"));
            return;
        }
        if (id == QStringLiteral("file.close")) {
            QTimer::singleShot(0, this, [this]() {
                close();
            });
            return;
        }
        if (id == QStringLiteral("edit.undo")) {
            m_editor->undo();
            return;
        }
        if (id == QStringLiteral("edit.redo")) {
            m_editor->redo();
            return;
        }
        if (id == QStringLiteral("edit.cut")) {
            m_editor->cut();
            return;
        }
        if (id == QStringLiteral("edit.copy")) {
            m_editor->copy();
            return;
        }
        if (id == QStringLiteral("edit.paste")) {
            m_editor->paste();
            return;
        }
        if (id == QStringLiteral("edit.selectAll")) {
            m_editor->selectAll();
            return;
        }
        if (id == QStringLiteral("edit.find")) {
            const bool found = m_editor->find(QStringLiteral("Ribbon"));
            showSimpleStatus(
                found ? QStringLiteral("Found Ribbon") : QStringLiteral("Ribbon not found"),
                found ? QStringLiteral("已找到 Ribbon") : QStringLiteral("未找到 Ribbon"));
            return;
        }
        if (id == QStringLiteral("insert.heading")) {
            m_editor->insertPlainText(QStringLiteral("\n# Ribbon heading\n"));
            return;
        }
        if (id == QStringLiteral("insert.list")) {
            m_editor->insertPlainText(QStringLiteral("\n- Item 1\n- Item 2\n- Item 3\n"));
            return;
        }
        if (id == QStringLiteral("insert.table")) {
            m_editor->insertPlainText(QStringLiteral("\n| A | B |\n|---|---|\n| 1 | 2 |\n"));
            return;
        }
        if (id == QStringLiteral("insert.link")) {
            m_editor->insertPlainText(QStringLiteral("https://www.qt.io/"));
            return;
        }
        if (id == QStringLiteral("insert.date")) {
            m_editor->insertPlainText(QDate::currentDate().toString(Qt::ISODate));
            return;
        }
        if (id == QStringLiteral("view.zoomIn")) {
            m_editor->zoomIn(1);
            ++m_zoomSteps;
            return;
        }
        if (id == QStringLiteral("view.zoomOut")) {
            m_editor->zoomOut(1);
            --m_zoomSteps;
            return;
        }
        if (id == QStringLiteral("view.resetZoom")) {
            resetZoom();
            return;
        }
        if (id == QStringLiteral("view.readOnly")) {
            QAction *action = m_helper->action(id);
            m_editor->setReadOnly(action && action->isChecked());
            showSimpleStatus(
                m_editor->isReadOnly() ? QStringLiteral("Read-only mode enabled") : QStringLiteral("Read-only mode disabled"),
                m_editor->isReadOnly() ? QStringLiteral("已启用只读模式") : QStringLiteral("已关闭只读模式"));
            return;
        }
        if (id == QStringLiteral("view.fullscreen")) {
            QAction *action = m_helper->action(id);
            if (action && action->isChecked()) {
                showFullScreen();
            } else {
                showNormal();
            }
            return;
        }
        if (id.startsWith(QStringLiteral("test."))) {
            QAction *action = m_helper->action(id);
            if (action && action->isCheckable()) {
                const bool checked = action->isChecked();
                showSimpleStatus(
                    QStringLiteral("%1: %2").arg(action->text(), checked ? QStringLiteral("checked") : QStringLiteral("unchecked")),
                    QStringLiteral("%1：%2").arg(action->text(), checked ? QStringLiteral("已勾选") : QStringLiteral("未勾选")));
            } else {
                showSimpleStatus(
                    QStringLiteral("Test action: %1").arg(id),
                    QStringLiteral("测试动作：%1").arg(id));
            }
            return;
        }
        if (id == QStringLiteral("tools.refresh")) {
            showSimpleStatus(QStringLiteral("Example state refreshed"), QStringLiteral("示例状态已刷新"));
            return;
        }
        if (id == QStringLiteral("tools.settings")) {
            QMessageBox::information(
                this,
                isChinese() ? QStringLiteral("设置") : QStringLiteral("Settings"),
                isChinese()
                    ? QStringLiteral("RibbonLib 保持设置能力在宿主应用中；这里仅展示动作和菜单集成。")
                    : QStringLiteral("RibbonLib keeps application settings in the host; this only demonstrates action and menu integration."));
            return;
        }
        if (id == QStringLiteral("help.about")) {
            QMessageBox::about(
                this,
                isChinese() ? QStringLiteral("关于 RibbonLib") : QStringLiteral("About RibbonLib"),
                isChinese()
                    ? QStringLiteral("一个极简、可嵌入、JSON 可配置的 Qt Widgets Ribbon 控件库。")
                    : QStringLiteral("A minimal, embeddable, JSON-configurable Qt Widgets Ribbon library."));
        }
    }

    QTextEdit *m_editor { nullptr };
    QWidget *m_ribbonHost { nullptr };
    QVBoxLayout *m_ribbonLayout { nullptr };
    QRibbonWidget *m_ribbon { nullptr };
    QRibbonHelper *m_helper { nullptr };
    QString m_language { QStringLiteral("en") };
    int m_zoomSteps { 0 };
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("RibbonLib Example"));

    DemoWindow window;
    window.show();
    return app.exec();
}
