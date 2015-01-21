
#ifndef DATABASE_ISQL_QUERY_CONSTRUCTOR
#define DATABASE_ISQL_QUERY_CONSTRUCTOR

#include <vector>

#include <QString>

#include "sql_backend_base_export.h"

namespace Database
{

    class InsertQueryData;
    class UpdateQueryData;

    struct ColDefinition;

    struct SQL_BACKEND_BASE_EXPORT SqlQuery
    {
            SqlQuery();
            SqlQuery(const QString &);
            ~SqlQuery();

            void addQuery(const QString &);

            const std::vector<QString>& getQueries() const;

        private:
            std::vector<QString> m_queries;
    };


    struct ISqlQueryConstructor
    {
        virtual ~ISqlQueryConstructor();

        // Create table with given name and columns decription.
        // It may be necessary for table to meet features:
        // - FOREIGN KEY
        //
        // More features may be added in future.
        // Default implementation returns QString("CREATE TABLE %1(%2);").arg(name).arg(columnsDesc)
        virtual QString prepareCreationQuery(const QString& name, const QString& columns) const = 0;

        //prepare query for finding table with given name
        virtual QString prepareFindTableQuery(const QString& name) const = 0;

        //prepare column description for CREATE TABLE matching provided info.
        virtual QString prepareColumnDescription(const ColDefinition &) const = 0;

        virtual SqlQuery insert(const InsertQueryData &) const = 0;                 // construct an insert sql query.
        virtual SqlQuery update(const UpdateQueryData &) const = 0;                 // construct an update sql query.
        virtual SqlQuery insertOrUpdate(const InsertQueryData &) const = 0;         // construct a query which will try to insert data. If it fails due to UNIQUE column attribute, try to update
    };
}

#endif
