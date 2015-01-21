
#ifndef PHOTO_INFO_HPP
#define PHOTO_INFO_HPP

#include <memory>
#include <cstdint>
#include <string>

#include <QString>

#include "../iphoto_info.hpp"

#include "database_export.h"
#include <OpenLibrary/putils/ts_resource.hpp>

class QPixmap;

struct HashAssigner;

class DATABASE_EXPORT PhotoInfo final: public IPhotoInfo, ol::ThreadSafeResource<Tag::TagsList>::INotify
{
    public:
        PhotoInfo(const QString &path);          //load all data from provided path
        PhotoInfo(const PhotoInfo &) = delete;
        virtual ~PhotoInfo();

        PhotoInfo& operator=(const PhotoInfo &) = delete;

        //data getting
        const QString& getPath() const;
        const Tag::TagsList& getTags() const;       // a access to tags
        const QPixmap& getThumbnail() const;        // a temporary thumbnail may be returned when final one is not yet generated.
        const Hash& getHash() const;                // Do not call until isHashLoaded()
        Id getID() const;

        //status checking
        bool isFullyInitialized() const;            // returns true if photo fully loaded (all items below are loaded)
        bool isHashLoaded() const;                  // returns true if hash is not null
        bool isThumbnailLoaded() const;             // returns true if thumbnail is loaded
        bool isExifDataLoaded() const;              // returns true is tags were loaded

        //observers
        void registerObserver(IObserver *);
        void unregisterObserver(IObserver *);

        //data initializing
        void initHash(const Hash &);
        void initThumbnail(const QPixmap &);
        void initID(const Id &);

        //set data
        virtual ol::ThreadSafeResource< Tag::TagsList >::Accessor accessTags();
        virtual void setTags(const Tag::TagsList &);

        //flags
        void markFlag(FlagsE, int) override;
        int getFlag(FlagsE) const override;

    private:
        struct Data;
        std::unique_ptr<Data> m_data;

        void updated();

        //INotify:
        void unlocked() override;
};

#endif