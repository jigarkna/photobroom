/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef IDXDATA_HPP
#define IDXDATA_HPP


#include <unordered_map>

#include <QMap>

#include <database/photo_data.hpp>
#include <database/filter.hpp>

#include "db_data_model.hpp"
#include "model_types.hpp"

class QVariant;

class IdxDataManager;

struct IIdxDataVisitor;

class IdxNodeData;

struct IIdxData
{
    typedef std::unique_ptr<IIdxData> Ptr;

    virtual ~IIdxData() = default;

    virtual void setParent(IdxNodeData *) = 0;
    virtual void setStatus(NodeStatus) = 0;
    virtual void reset() = 0;
    virtual void setNodeFilter(const Database::IFilter::Ptr& filter) = 0;
    virtual void setNodeSorting(const Hierarchy::Level &) = 0;

    virtual IdxNodeData* parent() const = 0;
    virtual QVariant getData(int) const = 0;
    virtual const Database::IFilter::Ptr& getFilter() const = 0;
    virtual std::size_t getLevel() const = 0;

    virtual int getRow() const = 0;
    virtual int getCol() const = 0;
    [[deprecated]] virtual NodeStatus status() const = 0;

    // visitation:
    virtual void visitMe(IIdxDataVisitor *) const = 0;
};


class IdxData: public IIdxData
{
    public:
        // node constructor
        IdxData(IdxDataManager *, const QVariant& name);

        //leaf constructor
        IdxData(IdxDataManager *, const Photo::Data &);

        virtual ~IdxData();

        IdxData(const IdxData &) = delete;
        IdxData& operator=(const IdxData &) = delete;

        void setNodeFilter(const Database::IFilter::Ptr& filter) override;
        void setNodeSorting(const Hierarchy::Level &) override;
        void reset() override;
        void setParent(IdxNodeData *) override;
        void setStatus(NodeStatus) override;
        IdxNodeData* parent() const override;

        QVariant getData(int) const override;
        const Database::IFilter::Ptr& getFilter() const override;
        std::size_t getLevel() const override;

        int getRow() const override;
        int getCol() const override;

        NodeStatus status() const override;

    protected:
        IdxData(IdxDataManager *);

        QMap<int, QVariant> m_data;
        Database::IFilter::Ptr m_filter;         // define which children match
        Hierarchy::Level m_order;                // defines how to sort children
        size_t m_level;
        IdxDataManager* m_manager;
        IdxNodeData* m_parent;
};


// base node
class IdxNodeData: public IdxData
{
    public:
        IdxNodeData(IdxDataManager *, const QVariant& name);
        virtual ~IdxNodeData();

        IIdxData* addChild(IIdxData::Ptr&& child);

        void removeChild(IIdxData* child);                         // removes child (memory is released)
        void removeChildren();
        IIdxData::Ptr takeChild(IIdxData* child);                  // function acts as removeChild but does not delete children

        const std::vector<Ptr>& getChildren() const;
        std::size_t getPositionOf(const IIdxData* child) const;    // returns position of children
        std::size_t findPositionFor(const IIdxData* child) const;  // returns position where child matches
        IIdxData* findChildWithBadPosition() const;                // returns first child which lies in a wrong place
        bool sortingRequired() const;

        // IdxData:
        virtual void reset() override;

    private:
        std::vector<Ptr> m_children;
        mutable std::unordered_map<const IIdxData *, std::size_t> m_positionsCache;

        virtual void visitMe(IIdxDataVisitor *) const override;
};


// base leaf
class IdxLeafData: public IdxData
{
    public:
        IdxLeafData(IdxDataManager *, const Photo::Data &);
        virtual ~IdxLeafData();

        virtual const Photo::Id& getMediaId() const;
        virtual const QString& getMediaPath() const;
        virtual const QSize& getMediaGeometry() const;
        virtual const Tag::TagsList& getTags() const;

        const Photo::Data& getPhoto() const;
        void update(const Photo::Data &);

    private:
        Photo::Data m_photo;
};


// basic leaf imlementation
class IdxRegularLeafData: public IdxLeafData
{
    public:
        IdxRegularLeafData(IdxDataManager *, const Photo::Data &);
        virtual ~IdxRegularLeafData();

    private:
        virtual void visitMe(IIdxDataVisitor *) const override;
};


// group leaf implementation
class IdxGroupLeafData: public IdxLeafData
{
    public:
        IdxGroupLeafData(IdxDataManager *, const Photo::Data &);
        virtual ~IdxGroupLeafData();

    private:
        std::vector<IPhotoInfo::Ptr> m_photos;

        virtual void visitMe(IIdxDataVisitor *) const override;
};


struct IIdxDataVisitor
{
    virtual ~IIdxDataVisitor() = default;

    virtual void visit(const IdxNodeData *) = 0;
    virtual void visit(const IdxRegularLeafData *) = 0;
    virtual void visit(const IdxGroupLeafData *) = 0;
};


template<typename NodeFunctor, typename RegularLeafFunctor, typename GroupLeafFunctor>
class InlineIdxDataVisitor: IIdxDataVisitor
{
    public:
        InlineIdxDataVisitor(NodeFunctor node,
                             RegularLeafFunctor regular_leaf,
                             GroupLeafFunctor group_leaf):
            m_node(node),
            m_regular_leaf(regular_leaf),
            m_group_leaf(group_leaf)
        {

        }

        void apply(const IIdxData* i)
        {
            i->visitMe(this);
        }

    private:
        NodeFunctor m_node;
        RegularLeafFunctor m_regular_leaf;
        GroupLeafFunctor m_group_leaf;

        void visit(const IdxNodeData* i) override
        {
            m_node(i);
        }

        void visit(const IdxRegularLeafData* i) override
        {
            m_regular_leaf(i);
        }

        void visit(const IdxGroupLeafData* i) override
        {
            m_group_leaf(i);
        }
};


template<typename NodeFunctor, typename RegularLeafFunctor, typename GroupLeafFunctor>
void apply_inline_visitor(const IIdxData* i, NodeFunctor node, RegularLeafFunctor regular_leaf, GroupLeafFunctor group_leaf)
{
    InlineIdxDataVisitor<NodeFunctor, RegularLeafFunctor, GroupLeafFunctor> visitor(node, regular_leaf, group_leaf);
    visitor.apply(i);
}


bool isNode(const IIdxData *);
bool isLeaf(const IIdxData *);

bool isRegularLeaf(const IIdxData *);
bool isGroupedLeaf(const IIdxData *);

#endif // IDXDATA_HPP
