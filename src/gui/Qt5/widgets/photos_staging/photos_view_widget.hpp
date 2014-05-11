
#ifndef PHOTOS_EDITOR_WIDGET_HPP
#define PHOTOS_EDITOR_WIDGET_HPP

#include <QWidget>

#include <core/photo_info.hpp>
#include <core/time_guardian.hpp>

class QItemSelectionModel;
class QItemSelection;
class QFileDialog;
class QPushButton;
class QLineEdit;
class QPushButton;
class QtExtChooseFile;

class DBDataModel;
class ImagesTreeView;

struct BrowseLine: public QWidget
{
    Q_OBJECT

    public:
        BrowseLine(QWidget *);
        ~BrowseLine();

        BrowseLine(const BrowseLine &) = delete;
        void operator=(const BrowseLine &) = delete;

    signals:
        void addPath(QString);

    private:
        QFileDialog *m_dialog;
        QPushButton *m_button;
        QLineEdit   *m_line;
        QPushButton *m_addButton;
        QtExtChooseFile *m_chooser;

    private slots:
        void browseButtonClicked();
        void addButtonClicked();
};


struct BrowseList: public QWidget
{
        Q_OBJECT

    public:
        BrowseList(QWidget *);
        ~BrowseList();

    signals:
        void addPath(QString);
};


class PhotosViewWidget: public QWidget
{
        Q_OBJECT

    public:
        explicit PhotosViewWidget(QWidget *parent = 0);
        PhotosViewWidget(const PhotosViewWidget &) = delete;
        virtual ~PhotosViewWidget();

        void operator=(const PhotosViewWidget &) = delete;

        template<class T>
        void addPhotos(const T &collection)
        {
            TIME_GUARDIAN("Photos storage", 100, "Too long!");

            for(auto &photo: collection)
            {
                addPhoto(photo);
            }
        }

        void addPhoto(const std::string &);
        std::vector<PhotoInfo::Ptr> getPhotos() const;

    private:
        DBDataModel*    m_photosModel;
        ImagesTreeView* m_photosView;

    private slots:
        void selectionChanged();

    signals:
        void selectionChanged(const std::vector<PhotoInfo::Ptr> &);
};

#endif // PHOTOS_EDITOR_WIDGET_HPP