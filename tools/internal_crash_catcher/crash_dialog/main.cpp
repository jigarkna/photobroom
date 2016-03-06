
#include <iostream>

#include <QApplication>
#include <QDebug>

#include "ui/crash_dialog.hpp"
#include "command_line_parser.hpp"
#include "debugger_factory.hpp"


int main(int argc, char** argv)
{
    int result = 1;

    QApplication app(argc, argv);
    QApplication::setApplicationName("CrashDialog");
    QApplication::setApplicationVersion("1.0");

    CommandLineParser parser(app);

    if (parser.error())
        std::cerr << parser.errorMsg().toStdString() << std::endl;
    else
    {
        DebuggerFactory dbgFactory;
        std::unique_ptr<IDebugger> debugger = dbgFactory.get();

        qDebug().noquote() << "Attaching to: " << parser.exec() << ", pid:" << parser.pid();
        debugger->init(parser.pid(), parser.tid(), parser.exec());

        CrashDialog dialog(debugger.get());
        dialog.show();

        result = app.exec();
    }

    return result;
}
