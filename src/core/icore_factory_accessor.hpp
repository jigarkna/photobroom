
#ifndef ICORE_FACTORY_ACCESSOR_HPP
#define ICORE_FACTORY_ACCESSOR_HPP

struct IConfiguration;
struct IExifReaderFactory;
struct IFeaturesManager;
struct ILoggerFactory;
struct IPythonThread;
struct ITaskExecutor;

struct ICoreFactoryAccessor
{
    virtual ~ICoreFactoryAccessor() = default;

    virtual ILoggerFactory* getLoggerFactory() = 0;
    virtual IExifReaderFactory* getExifReaderFactory() = 0;
    virtual IConfiguration* getConfiguration() = 0;
    virtual ITaskExecutor* getTaskExecutor() = 0;
    virtual IPythonThread* getPythonThread() = 0;
    virtual IFeaturesManager* getFeaturesManager() = 0;
};

#endif
