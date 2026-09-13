#include <QApplication>
#include <QMainWindow>

#include <RibbonLib.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    auto *ribbon = new QRibbonWidget(&window);
    window.setMenuWidget(ribbon);

    QRibbonTab *home = ribbon->addTab(QStringLiteral("Home"), QStringLiteral("home"));
    QRibbonGroup *group = home->addGroup(QStringLiteral("File"));
    group->addButton(QIcon(), QStringLiteral("Open"), QRibbonButtonSize::Large);

    if (ribbon->styleSheet().isEmpty()) return 1;
    if (ribbon->tabCount() != 1) return 2;
    if (ribbon->currentTab() != home) return 3;

    return 0;
}
