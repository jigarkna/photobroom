
#include "mainwindow.hpp"
#include "project_picker.hpp"

#include <functional>

#include <QCloseEvent>
#include <QMenuBar>
#include <QFileDialog>
#include <QLayout>
#include <QMessageBox>
#include <QPainter>

#include <database/database_builder.hpp>
#include <database/idatabase.hpp>
#include <database/database_tools/photos_analyzer.hpp>
#include <project_utils/iproject_manager.hpp>
#include <project_utils/iproject.hpp>

#include "config.hpp"

#include "widgets/info_widget.hpp"
#include "widgets/project_creator/project_creator_dialog.hpp"
#include "widgets/photos_data_model.hpp"
#include "widgets/staged_photos_data_model.hpp"
#include "widgets/photos_widget.hpp"
#include "widgets/staged_photos_widget.hpp"
#include "utils/photos_collector.hpp"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *p): QMainWindow(p),
    ui(new Ui::MainWindow),
    m_prjManager(nullptr),
    m_pluginLoader(nullptr),
    m_currentPrj(nullptr),
    m_imagesModel(nullptr),
    m_stagedImagesModel(nullptr),
    m_configuration(nullptr),
    m_photosCollector(new PhotosCollector(this)),
    m_views(),
    m_photosAnalyzer(new PhotosAnalyzer),
    m_infoWidget(nullptr)
{
    qRegisterMetaType<Database::BackendStatus >("Database::BackendStatus ");
    connect(this, SIGNAL(projectOpenedSignal(const Database::BackendStatus &)), this, SLOT(projectOpened(const Database::BackendStatus &)));
    connect(m_photosCollector, SIGNAL(finished()), this, SLOT(updateInfoWidget()));

    ui->setupUi(this);
    setupView();
    createMenus();
    updateMenus();
    updateGui();
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::set(IProjectManager* prjManager)
{
    m_prjManager = prjManager;
}


void MainWindow::set(IPluginLoader* pluginLoader)
{
    m_pluginLoader = pluginLoader;
}


void MainWindow::set(ITaskExecutor* taskExecutor)
{
    m_imagesModel->set(taskExecutor);
    m_stagedImagesModel->set(taskExecutor);
    m_photosAnalyzer->set(taskExecutor);
}


void MainWindow::set(IConfiguration* configuration)
{
    m_configuration = configuration;
    m_photosAnalyzer->set(configuration);

    for(IView* view: m_views)
        view->set(configuration);
}


void MainWindow::closeEvent(QCloseEvent *e)
{
    // TODO: close project!
    //m_currentPrj->close();

    closeProject();
    m_photosAnalyzer->stop();

    e->accept();
}


void MainWindow::openProject(const ProjectInfo& prjInfo)
{
    if (prjInfo.isValid())
    {
        closeProject();

        auto openCallback = std::bind(&MainWindow::projectOpenedNotification, this, std::placeholders::_1);
        
        m_currentPrj = m_prjManager->open(prjInfo, openCallback);
        
        Database::IDatabase* db = m_currentPrj->getDatabase();

        m_imagesModel->setDatabase(db);
        m_stagedImagesModel->setDatabase(db);
    }

    updateMenus();
    updateGui();
    updateTools();
}


void MainWindow::closeProject()
{
    if (m_currentPrj)
    {
        // Move m_currentPrj to a temporary place, so m_currentPrj is null and all tools will change theirs state basing on this.
        // Project object will be destroyed at the end of this routine
        auto prj = std::move(m_currentPrj);

        updateInfoWidget();

        m_imagesModel->setDatabase(nullptr);
        m_stagedImagesModel->setDatabase(nullptr);

        updateMenus();
        updateGui();
        updateTools();
    }
}


void MainWindow::setupView()
{
    m_imagesModel = new PhotosDataModel(this);
    PhotosWidget* photosWidget = new PhotosWidget(this);
    photosWidget->setWindowTitle(tr("Photos"));
    photosWidget->setModel(m_imagesModel);
    m_views.push_back(photosWidget);
    ui->viewsContainer->addWidget(photosWidget);

    m_stagedImagesModel = new StagedPhotosDataModel(this);
    StagedPhotosWidget* stagedPhotosWidget = new StagedPhotosWidget(this);
    stagedPhotosWidget->setWindowTitle(tr("Staged photos"));
    stagedPhotosWidget->setModel(m_stagedImagesModel);
    m_views.push_back(stagedPhotosWidget);
    ui->viewsContainer->addWidget(stagedPhotosWidget);

    //photos collector will write to stagedPhotosArea
    m_photosCollector->set(m_stagedImagesModel);

    m_infoWidget = new InfoWidget(this);

    viewChanged();

    connect(m_imagesModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(imagesModelChanged()));
    connect(m_imagesModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(imagesModelChanged()));

    connect(m_stagedImagesModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(staggedAreaModelChanged()));
    connect(m_stagedImagesModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(staggedAreaModelChanged()));

    updateInfoWidget();
}


void MainWindow::createMenus()
{
    for(size_t i = 0; i < m_views.size(); i++)
    {
        IView* view = m_views[i];
        const QString title = view->getName();
        QAction* action = ui->menuWindows->addAction(title);

        action->setData(static_cast<int>(i));
        connect(ui->menuWindows, SIGNAL(triggered(QAction *)), this, SLOT(activateWindow(QAction*)));
    }
}


void MainWindow::updateMenus()
{
    const bool prj = m_currentPrj.get() != nullptr;

    ui->menuPhotos->menuAction()->setVisible(prj);
    ui->menuWindows->menuAction()->setVisible(prj);
}


void MainWindow::updateGui()
{
    const bool prj = m_currentPrj.get() != nullptr;
    const QString title = tr("Photo broom: ") + (prj? m_currentPrj->getName(): tr("No collection opened"));

    setWindowTitle(title);
}


void MainWindow::updateTools()
{
    const bool prj = m_currentPrj.get() != nullptr;

    if (prj)
        m_photosAnalyzer->setDatabase(m_currentPrj->getDatabase());
    else
        m_photosAnalyzer->setDatabase(nullptr);
}


void MainWindow::viewChanged()
{
    const int w = ui->viewsContainer->currentIndex();

    IView* view = m_views[w];
    ui->tagEditor->set( view->getSelectionModel() );
    ui->tagEditor->set( view->getModel() );
}


void MainWindow::on_actionNew_collection_triggered()
{
    ProjectCreator prjCreator;
    const bool creation_status = prjCreator.create(m_prjManager, m_pluginLoader);

    if (creation_status)
        openProject(prjCreator.project());
}


void MainWindow::on_actionOpen_collection_triggered()
{
    ProjectPicker picker;

    picker.set(m_pluginLoader);
    picker.set(m_prjManager);
    const int s = picker.exec();
    
    if (s == QDialog::Accepted)
    {
        const ProjectInfo prjName = picker.choosenProject();

        openProject(prjName);
    }
}


void MainWindow::on_actionClose_triggered()
{
    closeProject();
}


void MainWindow::on_actionQuit_triggered()
{
    close();
}


void MainWindow::on_actionAdd_photos_triggered()
{
    const QString path = QFileDialog::getExistingDirectory(this, tr("Choose directory with photos"));

    if (path.isEmpty() == false)
        m_photosCollector->addDir(path);

    updateInfoWidget();
}


void MainWindow::activateWindow(QAction* action)
{
    const int w = action->data().toInt();

    ui->viewsContainer->setCurrentIndex(w);

    viewChanged();
}


void MainWindow::on_actionHelp_triggered()
{

}


void MainWindow::on_actionAbout_triggered()
{
    QString about;
    about =  QString("Photo Broom ") + PHOTO_BROOM_VERSION + "\n";
    about += "by Michał Walenciak";

    QMessageBox::about(this, tr("About Photo Broom"), about);
}


void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}


void MainWindow::projectOpened(const Database::BackendStatus& status)
{
    switch(status.get())
    {
        case Database::StatusCodes::Ok:
            updateInfoWidget();
            break;

        case Database::StatusCodes::BadVersion:
            QMessageBox::critical(this,
                                  tr("Unsupported photo collection version"),
                                  tr("Photo collection you are trying to open uses database in version which is not supported.\n"
                                     "It means your application is too old to open it.\n\n"
                                     "Please upgrade application to open this collection.")
                                 );
            closeProject();
            break;

        default:
            QMessageBox::critical(this,
                                  tr("Unexpected error"),
                                  tr("An unexpected error occured while opening photo collection.\n"
                                     "Please report a bug.\n"
                                     "Error code: " + static_cast<int>( status.get()) )
                                 );
            closeProject();
            break;
    }
}


void MainWindow::imagesModelChanged()
{
    updateInfoWidget();
}


void MainWindow::staggedAreaModelChanged()
{
    updateInfoWidget();
}


void MainWindow::updateInfoWidget()
{
    QString infoText;

    if (m_currentPrj.get() == nullptr)
        infoText = tr("No photo collection is opened.\n\n"
                      "Use 'open' action form 'Photo collection' menu to choose one\n"
                      "or 'new' action and create new collection.");

    const bool photos_in_staging_area = m_stagedImagesModel->isEmpty() == false;
    const bool photos_in_images_area  = m_imagesModel->isEmpty() == false;
    const bool photos_collector_works = m_photosCollector->isWorking();

    const bool state_photos_for_review = photos_in_images_area == false && (photos_in_staging_area || photos_collector_works);

    if (infoText.isEmpty() && state_photos_for_review)
        infoText = tr("%1.\n\n"
                      "All new photos are added to special area where they can be reviewed before they will be added to collection.\n"
                      "To se those photos choose %2 and then %3\n")
                   .arg(photos_collector_works? tr("Photos are being loaded"): tr("Photos waiting for review"))
                   .arg(ui->menuWindows->title())
                   .arg(m_views[1]->getName());

    if (infoText.isEmpty() && m_imagesModel->isEmpty() == 0)
        infoText = tr("There are no photos in your collection.\n\nAdd some by choosing 'Add photos' action from 'Photos' menu.");

    if (infoText.isEmpty() == false)
    {
        const QRect w_r = rect();
        const QPoint w_c = w_r.center();

        m_infoWidget->setText(infoText);
        m_infoWidget->adjustSize();

        const QSize infoSizeHint = m_infoWidget->sizeHint();

        const QPoint p(w_c.x() - infoSizeHint.width() / 2,
                       w_c.y() - infoSizeHint.height() / 2);

        m_infoWidget->move(p);
    }

    if (infoText.isEmpty() && m_infoWidget->isVisible())
        m_infoWidget->hide();

    if (infoText.isEmpty() == false && m_infoWidget->isHidden())
        m_infoWidget->show();
}


void MainWindow::projectOpenedNotification(const Database::BackendStatus& status)
{
    emit projectOpenedSignal(status);
}

