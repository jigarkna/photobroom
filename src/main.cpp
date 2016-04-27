
#include <assert.h>

#include <vector>
#include <string>

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTimer>

#include <configuration/configuration.hpp>
#include <core/logger_factory.hpp>
#include <core/task_executor.hpp>
#include <core/ilogger.hpp>
#include <crash_catcher/crash_catcher.hpp>
#include <database/database_builder.hpp>
#include <gui/gui.hpp>
#include <plugins/plugin_loader.hpp>
#include <project_utils/project_manager.hpp>
#include <system/system.hpp>


int main(int argc, char **argv)
{
    Gui gui;

    std::unique_ptr<QCoreApplication> app = gui.init(argc, argv);
    app->setApplicationName("photo_broom");                                // without this app name may change when binary name changes

    const bool status = CrashCatcher::init(argv[0]);
    const QString basePath = System::getApplicationConfigDir() + "/logs";

    // perform command line parsing
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption crashTestOption("test-crash-catcher", "When specified, photo_broom will crash 3 seconds after being launch");
    parser.addOption(crashTestOption);

    parser.process(*app);

    const bool enableCrashTest = parser.isSet(crashTestOption);
    if (enableCrashTest)
        QTimer::singleShot(3000, []
        {
            int* ptr = nullptr;
            volatile int v = *ptr;
	    (void) v;
        });


    // build objects
    LoggerFactory logger_factory(basePath);

    Configuration configuration;

    PluginLoader pluginLoader;
    pluginLoader.set(&logger_factory);

    TaskExecutor taskExecutor;

    Database::Builder database_builder;
    database_builder.set(&pluginLoader);
    database_builder.set(&logger_factory);
    database_builder.set(&configuration);

    ProjectManager prjManager;
    prjManager.set(&database_builder);

    if (status)
        logger_factory.get("CrashCatcher")->debug("Initialization successful");
    else
        logger_factory.get("CrashCatcher")->error("Initialization failed");

    // start gui
    gui.set(&prjManager);
    gui.set(&pluginLoader);
    gui.set(&configuration);
    gui.set(&logger_factory);
    gui.set(&taskExecutor);
    gui.run();

    return 0;
}
