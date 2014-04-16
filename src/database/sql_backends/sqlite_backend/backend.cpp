
#include "backend.hpp"

#include <stdexcept>

#include <QSqlDatabase>

#include <boost/filesystem.hpp>

#include <configuration/configurationfactory.hpp>
#include <configuration/iconfiguration.hpp>
#include <configuration/entrydata.hpp>
#include <database/databasebuilder.hpp>
#include <sql_backends/table_definition.hpp>

namespace Database
{
    const char* QDatabaseName = "Backend";

    struct SQLiteBackend::Data
    {
        Data(): m_initialized(false) {}

        ~Data()
        {

        }

        bool prepareDB(QSqlDatabase* db, const char* name)
        {
            bool status = true;

            if (m_initialized == false)
            {
                auto entry = ConfigurationFactory::get()->findEntry(Database::databaseLocation);

                //create base directory
                if (entry)
                {
                    boost::filesystem::path storage(entry->value());

                    storage /= "SQLite";

                    if (boost::filesystem::exists(storage) == false)
                        status = boost::filesystem::create_directories(storage);

                    if (status)
                    {
                        storage /= std::string(name) + ".db";

                        QSqlDatabase db_obj;
                        //setup db connection
                        db_obj = QSqlDatabase::addDatabase("QSQLITE", QDatabaseName);
                        db_obj.setDatabaseName(storage.c_str());

                        *db = db_obj;
                    }
                }
            }

            return status;
        }

        bool m_initialized;
    };


    SQLiteBackend::SQLiteBackend(): m_data(new Data)
    {

    }


    SQLiteBackend::~SQLiteBackend()
    {

    }


    bool SQLiteBackend::prepareDB(QSqlDatabase* db, const char* name)
    {
        return m_data->prepareDB(db, name);
    }


    QString SQLiteBackend::prepareFindTableQuery(const QString& name) const
    {
        return QString("SELECT name FROM sqlite_master WHERE name='%1';").arg(name);
    }


    QString SQLiteBackend::prepareColumnDescription(const ColDefinition& col) const
    {
        QString result;

        switch(col.type)
        {
            case ColDefinition::Type::Regular:
                result = col.name;
                break;

            case ColDefinition::Type::ID:
                result = col.name + " " + "INTEGER PRIMARY KEY AUTOINCREMENT";
                break;
        }

        return result;
    }

}
