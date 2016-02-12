/*
 * View's low level data structure
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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
 *
 */

#ifndef VIEW_DATA_SET_HPP
#define VIEW_DATA_SET_HPP

#include <iostream>

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QRect>

#include <core/tree.hpp>

#ifndef NDEBUG
#define assert_dump(expr,dump)                          \
    if (!expr) { dump(), abort(); }                     \
    else static_cast<void>(0)
#else
#define assert_dump(expr,dump) static_cast<void>(0)
#endif

struct IViewDataSet
{
        virtual ~IViewDataSet();

        virtual void rowsInserted(const QModelIndex &, int, int) = 0;
        virtual void rowsRemoved(const QModelIndex &, int, int) = 0;
        virtual void rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int) = 0;
        virtual void modelReset() = 0;
};


template<typename T>
class ViewDataSet final: public IViewDataSet
{
        template<typename IT, typename M>
        IT _find(M& model, const std::vector<size_t>& hierarchy) const
        {
            assert(hierarchy.empty() == false);

            //setup first item
            IT item_it = model.end();

            if (model.empty() == false)
                for(size_t i = 0; i < hierarchy.size(); i++)
                {
                    const size_t pos = hierarchy[i];

                    if (i == 0)
                    {
                        IT b(model.begin());
                        IT e(model.end());

                        const size_t c = e - b;               //how many items?
                        if (pos < c)
                            item_it = IT(b) + pos;
                        else
                        {
                            item_it = e;
                            break;                            //out of scope
                        }
                    }
                    else
                    {
                        const size_t c = item_it.children_count();

                        if (pos < c)
                            item_it = item_it.begin() + pos;
                        else
                        {
                            item_it = model.end();
                            break;                            //out of scope
                        }
                    }
                }

            return item_it;
        }

    public:
        typedef tree<T>  Model;
        typedef typename Model::const_iterator const_iterator;
        typedef typename Model::iterator       iterator;

        typedef typename Model::const_level_iterator const_level_iterator;
        typedef typename Model::level_iterator       level_iterator;

        ViewDataSet(): m_model(), m_db_model(nullptr)
        {
            clear();
        }

        ViewDataSet(const ViewDataSet<T> &) = delete;

        ~ViewDataSet()
        {
        }

        ViewDataSet<T>& operator=(const ViewDataSet<T> &) = delete;

        void set(QAbstractItemModel* model)
        {
            m_db_model = model;
        }

        const_iterator find(const QModelIndex& index) const
        {
            std::vector<size_t> hierarchy = generateHierarchy(index);

            return _find<const_level_iterator>(m_model, hierarchy);
        }

        const_iterator begin() const
        {
            return m_model.begin();
        }

        const_iterator end() const
        {
            return m_model.end();
        }

        const_iterator cfind(const QModelIndex& index) const
        {
            return find(index);
        }

        const_iterator cbegin() const
        {
            return m_model.cbegin();
        }

        const_iterator cend() const
        {
            return m_model.cend();
        }

        iterator find(const QModelIndex& index)
        {
            std::vector<size_t> hierarchy = generateHierarchy(index);

            return _find<level_iterator>(m_model, hierarchy);
        }

        iterator begin()
        {
            return m_model.begin();
        }

        iterator end()
        {
            return m_model.end();
        }

        bool empty() const
        {
            return m_model.empty();
        }

        size_t size() const
        {
            return m_model.cend() - m_model.cbegin();
        }

        std::string dumpModel() const
        {
            const std::string dump = m_model.dump();
            return dump;
        }

        // to be called by view:
        void rowsInserted(const QModelIndex& parent, int from, int to) override
        {
            //update model
            auto parentIt = find(parent);
            iterator childIt = level_iterator(parentIt).begin() + from;

            for( int i = from; i <= to; i++)
            {
                QModelIndex child_idx = m_db_model->index(i, 0, parent);
                childIt = insert(childIt, T(child_idx));                       // each next sub node is being placed at the same position

                //check if inserted item has children, and add them if any
                const int children = m_db_model->rowCount(child_idx);
                if (children)
                    rowsInserted(child_idx, 0, children - 1);
            }
        }

        void rowsRemoved(const QModelIndex& parent, int from , int to) override
        {
            //update model
            auto parentIt = find(parent);
            level_iterator flat_parent(parentIt);

            if (flat_parent.children_count() > static_cast<unsigned int>(to))
            {
                for(int i = from; i <= to; i++)
                {
                    auto childIt = flat_parent.begin() + from;        // keep deleting item at the same position
                    erase(childIt);
                }
            }
            else
                assert(!"model is not consistent");                   // parent is expanded, so should be loaded (have children)
        }

        void rowsMoved(const QModelIndex& sourceParent, int src_from, int src_to, const QModelIndex& destinationParent, int dst_from) override
        {
            const int n = src_to - src_from;
            const int dst_to = dst_from + n;

            // TODO: implement variant which would do a real move

            if (sourceParent != destinationParent || src_from > dst_to)
            {
                rowsRemoved(sourceParent, src_from, src_to);
                rowsInserted(destinationParent, dst_from, dst_to);
            }
            else
            {
                // The same parent, and source rows are before destination rows.
                // In such case we need to do some adjustments in destination row

                const int rows_removed = n + 1;

                rowsRemoved(sourceParent, src_from, src_to);
                rowsInserted(destinationParent, dst_from - rows_removed , dst_to - rows_removed);
            }
        }

        void modelReset() override
        {
            clear();

            if (m_db_model != nullptr)
            {
                //load all data
                loadIndex(QModelIndex(), begin());
            }
        }

        bool validate() const
        {
            return validate(m_db_model, QModelIndex(), begin());
        }

    private:
        Model m_model;
        QAbstractItemModel* m_db_model;

        // TODO: introduce tests for validity
        bool validate(QAbstractItemModel* model, const QModelIndex& index, const_level_iterator it) const
        {
            bool equal = true;

            if (it->expanded)                                          // never expanded nodes are not loaded due to lazy initialization
            {
                const size_t it_children = it.children_count();
                const size_t idx_children = static_cast<size_t>(model->rowCount(index));
                equal = it_children == idx_children;

                assert_dump(equal, [&]
                {
                    std::cerr << m_model.dump() << std::endl;
                });

                if (equal && it_children != 0)                         // still ok && has children
                    for(int i = 0; i < it_children; i++)
                        equal = validate(model, model->index(i, 0, index), it.begin() + i);
            }

            return equal;
        }

        std::vector<size_t> generateHierarchy(const QModelIndex& index) const
        {
            std::vector<size_t> result;

            if (index.isValid())
            {
                assert(index.row() >= 0);

                std::vector<size_t> parents = generateHierarchy(index.parent());
                result.insert(result.begin(), parents.cbegin(), parents.cend());
                result.push_back( static_cast<size_t>(index.row()) );
            }
            else
                result.push_back(0);             //top item

                return result;
        }

        void erase(const iterator& it)
        {
            m_model.erase(it);
        }

        void clear()
        {
            m_model.clear();

            //add item for QModelIndex() which is always present
            insert( begin(), T(QModelIndex()) );
        }

        iterator insert(const QModelIndex& index, const T& info)
        {
            assert(find(index) == end());                 // we do not expect this item already in model

            std::vector<size_t> hierarchy = generateHierarchy(index);
            const size_t hierarchy_size = hierarchy.size();
            assert(hierarchy_size > 0);

            //setup first item
            level_iterator item_it = m_model.end();

            for(size_t i = 0; i < hierarchy.size(); i++)
            {
                const size_t pos = hierarchy[i];
                const bool last = i + 1 == hierarchy_size;

                if (i == 0)
                {
                    level_iterator b(m_model.begin());
                    level_iterator e(m_model.end());

                    const size_t c = e - b;               // how many items?
                    if (pos < c)
                    {
                        assert(last == false);            // we want to insert item. If this is last level and there is such item, something went wrong (assert at top of the function should have catch it)
                        item_it = level_iterator(m_model.begin()) + hierarchy[i];
                    }
                    else if (pos == c)                    // just append after last item?
                    {
                        if (last == false)                // for last level do nothing - we will instert this item later below
                        {
                            level_iterator ins = b + pos;
                            item_it = m_model.insert(ins, T());
                        }
                    }
                    else
                        assert(!"missing siblings");
                }
                else
                {
                    const size_t c = item_it.children_count();
                    if (pos < c)
                    {
                        assert(last == false);            // we want to insert item. If this is last level and there is such item, something went wrong (assert at top of the function should have catch it)
                        item_it = item_it.begin() + pos;
                    }
                    else if (pos == c)                    //just append after last item?
                    {
                        level_iterator ins = item_it.begin() + pos;

                        if (last)
                            item_it = ins;                // for last level of hierarchy set item_it to desired position
                            else
                                item_it = m_model.insert(ins, T());
                    }
                    else
                        assert(!"missing siblings");
                }
            }

            auto it = m_model.insert(item_it, info);

            return it;
        }

        iterator insert(level_iterator it, const T& info)
        {
            return m_model.insert(it, info);
        }

        void loadIndex(const QModelIndex& p, level_iterator p_it)
        {
            assert(p_it.children_count() == 0);
            const int c = m_db_model->rowCount(p);

            for(int i = 0; i < c; i++)
            {
                level_iterator c_it = p_it.begin() + i;
                QModelIndex c_idx = m_db_model->index(i, 0, p);

                c_it = m_model.insert(c_it, T(c_idx));
                loadIndex(c_idx, c_it);
            }
        }
};


// A helper class which purpose is to store  in tree any model change automatically
// The only problem is that if owner of ViewDataSet (View) also connects
// itself to model's signals and will try to access ViewDataSet in
// for example rowsInserted, ViewDataSet may not be up to date yet.
// In such case, do not use this class but call all ViewDataSet's functions
// related to model changes manually.

struct ViewDataModelObserver: public QObject
{
    Q_OBJECT

    public:
        explicit ViewDataModelObserver(IViewDataSet* viewDataSet, QAbstractItemModel* model, QObject* p = 0): QObject(p), m_db_model(nullptr), m_viewDataSet(viewDataSet)
        {
            set(model);
        }

        ViewDataModelObserver(const ViewDataModelObserver &) = delete;

        ViewDataModelObserver& operator=(const ViewDataModelObserver &) = delete;

        void set(QAbstractItemModel* model)
        {
            if (m_db_model != nullptr)
                m_db_model->disconnect(this);

            m_db_model = model;

            if (m_db_model != nullptr)
            {
                connect(m_db_model, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(rowsInserted(QModelIndex, int, int)));
                connect(m_db_model, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(rowsRemoved(QModelIndex, int, int)));
                connect(m_db_model, SIGNAL(rowsMoved(QModelIndex,int, int, QModelIndex, int)),
                        this, SLOT(rowsMoved(QModelIndex,int, int, QModelIndex, int)));

                connect(m_db_model, SIGNAL(modelReset()), this, SLOT(modelReset()));
            }

            modelReset();
        }

    private slots:
        virtual void rowsInserted(const QModelIndex& p, int f, int t) { m_viewDataSet->rowsInserted(p, f, t); }
        virtual void rowsRemoved(const QModelIndex& p, int f, int t)  { m_viewDataSet->rowsRemoved(p, f, t); }
        virtual void rowsMoved(const QModelIndex& p, int f, int t,
                               const QModelIndex& d, int dt)          { m_viewDataSet->rowsMoved(p, f, t, d, dt); }
        virtual void modelReset()                                     { m_viewDataSet->modelReset(); }

    private:
        QAbstractItemModel* m_db_model;
        IViewDataSet* m_viewDataSet;
};


#endif // VIEW_DATA_SET_HPP
