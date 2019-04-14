/*
 * Class for performing operations on groups
 * Copyright (C) 2019  Michał Walenciak <Kicer86@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "group_operator.hpp"

#include <QSqlDatabase>
#include <QSqlQuery>

#include "isql_query_constructor.hpp"
#include "isql_query_executor.hpp"
#include "query_structs.hpp"
#include "tables.hpp"

namespace Database
{

    GroupOperator::GroupOperator(const QString& name, const IGenericSqlQueryGenerator* generator, Database::ISqlQueryExecutor* executor):
        m_connectionName(name),
        m_queryGenerator(generator),
        m_executor(executor)
    {
    }


    Group::Id GroupOperator::addGroup(const Photo::Id& id, GroupInfo::Type type)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        Group::Id grp_id;

        InsertQueryData insertData(TAB_GROUPS);

        insertData.setColumns("id", "representative_id", "type");
        insertData.setValues(InsertQueryData::Value::Null, id, static_cast<int>(type));

        QSqlQuery query = m_queryGenerator->insert(db, insertData);

        bool status = m_executor->exec(query);

        //update id
        if (status)                                    //Get Id from database after insert
        {
            QVariant group_id  = query.lastInsertId(); //TODO: WARNING: may not work (http://qt-project.org/doc/qt-5.1/qtsql/qsqlquery.html#lastInsertId)
            status = group_id.isValid();

            if (status)
                grp_id = Group::Id(group_id.toInt());
        }

        return grp_id;
    }

}
