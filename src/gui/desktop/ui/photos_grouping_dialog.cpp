
#include "photos_grouping_dialog.hpp"

#include <QFileInfo>
#include <QProcess>
#include <QMovie>
#include <QTemporaryFile>

#include <core/iexif_reader.hpp>

#include "ui_photos_grouping_dialog.h"


struct AnimationGenerator: QObject
{
    struct Data
    {
        double fps;
        double delay;
        double scale;
        QStringList photos;

        Data(): fps(0.0), delay(0.0), scale(0.0), photos() {}
    };

    AnimationGenerator(const std::function<void(QWidget *)>& callback, const QString& location):
        m_callback(callback),
        m_location(location),
        m_movie(),
        m_baseSize()
    {
    }


    void generatePreviewWidget(const Data& data)
    {
        assert(m_location.isEmpty() == false);

        if (m_movie.get() != nullptr)
            m_movie->stop();

        m_baseSize = QSize();

        const QString location = QString("%1/animation.gif").arg(m_location);

        QStringList images;

        for(const QString& photo: data.photos)
        {
            const QFileInfo fileInfo(photo);
            const QString absoluteFilePath = fileInfo.absoluteFilePath();

            images.append(absoluteFilePath);
        }

        QStringList args;
        args << "-delay" << QString::number(1/data.fps * 100);   // convert fps to 1/100th of a second
        args << images;
        args << "-loop" << "0";
        args << "-resize" << QString::number(100/data.scale) + "%";
        args << location;

        QProcess convert;
        convert.start("convert", args);
        convert.waitForFinished(-1);

        m_movie = std::make_unique<QMovie>(location);
        QLabel* label = new QLabel;

        label->setMovie(m_movie.get());
        m_movie->start();

        m_callback(label);
    }

    void scalePreview(double scale)
    {
        if (m_movie.get() != nullptr)
        {
            if (m_baseSize.isValid() == false)
                m_baseSize = m_movie->frameRect().size();

            const double scaleFactor = scale/100;
            QSize size = m_baseSize;
            size.rheight() *= scaleFactor;
            size.rwidth() *= scaleFactor;

            m_movie->setScaledSize(size);
        }
    }

    std::function<void(QWidget *)> m_callback;
    const QString m_location;
    std::unique_ptr<QMovie> m_movie;
    QSize m_baseSize;
};


PhotosGroupingDialog::PhotosGroupingDialog(const std::vector<IPhotoInfo::Ptr>& photos, IExifReader* exifReader, QWidget *parent):
    QDialog(parent),
    m_model(),
    m_tmpLocation(),
    m_animationGenerator(),
    m_sortProxy(),
    ui(new Ui::PhotosGroupingDialog),
    m_exifReader(exifReader)
{
    fillModel(photos);

    ui->setupUi(this);

    m_sortProxy.setSourceModel(&m_model);

    ui->photosView->setModel(&m_sortProxy);
    ui->photosView->setSortingEnabled(true);
    ui->photosView->sortByColumn(0, Qt::AscendingOrder);
    ui->photosView->resizeColumnsToContents();

    using namespace std::placeholders;
    auto callback = std::bind(&PhotosGroupingDialog::updatePreview, this, _1);

    m_animationGenerator = std::make_unique<AnimationGenerator>(callback, m_tmpLocation.path());

    connect(ui->applyButton, &QPushButton::pressed, this, &PhotosGroupingDialog::makeAnimation);
    connect(ui->previewScaleSlider, &QSlider::sliderMoved, m_animationGenerator.get(), &AnimationGenerator::scalePreview);
}


PhotosGroupingDialog::~PhotosGroupingDialog()
{
    delete ui;
}


void PhotosGroupingDialog::updatePreview(QWidget* preview)
{
    if (preview != nullptr)
        ui->resultPreview->setWidget(preview);
}


void PhotosGroupingDialog::typeChanged()
{

}


void PhotosGroupingDialog::makeAnimation()
{
    AnimationGenerator::Data generator_data;

    generator_data.photos = getPhotos();
    generator_data.fps = ui->speedSpinBox->value();
    generator_data.scale = ui->scaleSpinBox->value();

    m_animationGenerator->generatePreviewWidget(generator_data);
}


void PhotosGroupingDialog::fillModel(const std::vector<IPhotoInfo::Ptr>& photos)
{
    m_model.clear();

    for(const IPhotoInfo::Ptr& photo: photos)
    {
        const QString path = photo->getPath();
        const boost::any sequence_number = m_exifReader->get(path, IExifReader::ExtraData::SequenceNumber);

        const QString sequence_str = sequence_number.empty()? "-" : QString::number( boost::any_cast<int>(sequence_number));

        QStandardItem* pathItem = new QStandardItem(path);
        QStandardItem* sequenceItem = new QStandardItem(sequence_str);

        m_model.appendRow({pathItem, sequenceItem});
        m_model.setHeaderData(0, Qt::Horizontal, tr("photo path"));
        m_model.setHeaderData(1, Qt::Horizontal, tr("sequence number"));
    }
}


QStringList PhotosGroupingDialog::getPhotos() const
{
    QStringList result;

    for(int r = 0; r < m_sortProxy.rowCount(); r++)
    {
        const QModelIndex pathItemIdx = m_sortProxy.index(r, 0);
        const QVariant pathRaw = pathItemIdx.data(Qt::DisplayRole);
        const QString path = pathRaw.toString();

        result.append(path);
    }

    return result;
}
