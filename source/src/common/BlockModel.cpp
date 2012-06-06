/*
 * 	Copyright (c) 2010-2011, Romuald CARI
 *	All rights reserved.
 *
 *	Redistribution and use in source and binary forms, with or without
 *	modification, are permitted provided that the following conditions are met:
 *		* Redistributions of source code must retain the above copyright
 *		  notice, this list of conditions and the following disclaimer.
 *		* Redistributions in binary form must reproduce the above copyright
 *		  notice, this list of conditions and the following disclaimer in the
 *		  documentation and/or other materials provided with the distribution.
 *		* Neither the name of the <organization> nor the
 *		  names of its contributors may be used to endorse or promote products
 *		  derived from this software without specific prior written permission.
 *
 *	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *	DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 *	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <common/BlockModel.hpp>
#include <common/BlockModelPrivate.hpp>
using namespace Gooey::common;

#include <GooeyEngine.hpp>
using namespace Gooey;

using namespace Kore::data;

#include <QtCore/QModelIndex>
#include <QtGui/QIcon>

BlockModel::BlockModel(Block* block, QObject* parent)
:	QAbstractItemModel(parent),
 	_block(block)
{
	// Clear / set the model when the block is removed from the tree and inserted in the tree.
	connect( _block, SIGNAL(blockRemoved()), SLOT(clear()) );
	connect( _block, SIGNAL(blockInserted()), SLOT(createModel()) );

	// Build the model !
	createModel();
}

Block* BlockModel::block()
{
	return _block;
}

QMetaProperty BlockModel::blockProperty(const QModelIndex& index)
{
	if(!index.isValid())
	{
		return QMetaProperty(); // An invalid property !
	}

	BlockModelPrivateIndex* mIndex = static_cast<BlockModelPrivateIndex*>(index.internalPointer());
	if(mIndex->type() == BlockModelPrivateIndex::Ancestor)
	{
		return QMetaProperty(); // An invalid property !
	}

	BlockPropertyIndex* pIndex = static_cast<BlockPropertyIndex*>(mIndex);
	return pIndex->blockProperty();
}

void BlockModel::blockPropertyChanged(kint ancestorIndex, kint propertyIndex)
{
	// Retrieve the ancestor model index.
	QModelIndex resultIndex = index(propertyIndex, 1, index(ancestorIndex, 0, QModelIndex())); // Retrieve the data cell model index of the property.

	emit dataChanged(resultIndex, resultIndex);
}

kbool BlockModel::hasChildren(const QModelIndex& index)
{
	if(!index.isValid()) // Children of the root
	{
		return !_indices.isEmpty();
	}

	BlockModelPrivateIndex* mIndex = static_cast<BlockModelPrivateIndex*>(index.internalPointer());
	if(mIndex->type() == BlockModelPrivateIndex::Ancestor)
	{
		// Block ancestors always have children !
		return true;
	}

	// Block property entries do not have children.
	return false;
}

QModelIndex BlockModel::parent(const QModelIndex& index) const
{
	if(!index.isValid())
	{
		return QModelIndex();
	}

	BlockModelPrivateIndex* mIndex = static_cast<BlockModelPrivateIndex*>(index.internalPointer());
	if(mIndex->type() == BlockModelPrivateIndex::Ancestor)
	{
		return QModelIndex();
	}

	// It is a PropertyIndex !
	BlockPropertyIndex* pIndex = static_cast<BlockPropertyIndex*>(mIndex);
	return this->index(pIndex->ancestorIndex(), 0, QModelIndex());
}

QModelIndex BlockModel::index(kint row, kint column, const QModelIndex& parent) const
{
	if(!parent.isValid())
	{
		if(column == 0)
		{
			return createIndex(row, column, _indices.at(row));
		}
		else
		{
			return QModelIndex();
		}
	}

	BlockModelPrivateIndex* mIndex = static_cast<BlockModelPrivateIndex*>(parent.internalPointer());
	K_ASSERT( mIndex->type() == BlockModelPrivateIndex::Ancestor )

	K_ASSERT( column < 2 )

	return createIndex(
			row,
			column,
			static_cast<BlockAncestorIndex*>(mIndex)->properties().at(row)
		);
}

kint BlockModel::rowCount(const QModelIndex& parent) const
{
	if(!parent.isValid())
	{
		return _indices.size();
	}

	BlockModelPrivateIndex* index = static_cast<BlockModelPrivateIndex*>(parent.internalPointer());
	if(index->type() == BlockModelPrivateIndex::Ancestor)
	{
		return static_cast<BlockAncestorIndex*>(index)->properties().size();
	}

	// Default for BlockPropertyIndex: no children !
	return 0;
}

kint BlockModel::columnCount(const QModelIndex& parent) const
{
	// 2 columns !
	return 2;
}

QVariant BlockModel::data(const QModelIndex& index, kint role) const
{
	if(!index.isValid())
	{
		return QVariant();
	}

	switch(role)
	{
	case Qt::DisplayRole:
	case Qt::EditRole:
		return getDisplayName(index);
		break;
	case Qt::DecorationRole:
		return GooeyEngine::GetIcon("gooey/block.property.png");
	 default:
		return QVariant();
	}
}

bool BlockModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	K_ASSERT( index.column() == 1 ) // Only column index 1 is editable !

	if(Qt::EditRole == role)
	{
		return blockProperty(index).write(_block, value);
	}
	return false;
}

QVariant BlockModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(Qt::Horizontal != orientation)
	{
		return QVariant();
	}

	switch(role)
	{
	case Qt::DisplayRole:
		return (section == 0) ? tr("Property") : tr("Value");
	default:
		return QVariant();
	}
}

Qt::ItemFlags BlockModel::flags(const QModelIndex &index) const
{
	K_ASSERT( index.isValid() )

	// Labels for Ancestors and Properties
	if(index.column() == 1)
	{
		BlockModelPrivateIndex* mIndex = static_cast<BlockModelPrivateIndex*>(index.internalPointer());
		K_ASSERT(mIndex->type() == BlockModelPrivateIndex::Property)

		BlockPropertyIndex* pIndex = static_cast<BlockPropertyIndex*>(mIndex);

		Qt::ItemFlags result = Qt::ItemIsSelectable;
		// Is the data editable ?
		result |= ( pIndex->blockProperty().isWritable() ) ? Qt::ItemIsEditable : Qt::NoItemFlags;
		// Is it enabled ?
		result |= _block->isFrozen() ? Qt::NoItemFlags : Qt::ItemIsEnabled;
		// Is it checkable ?
		result |= ( pIndex->blockProperty().type() == QVariant::Bool ) ? Qt::ItemIsUserCheckable : Qt::NoItemFlags;

		return result;
	}

	if(index.parent().isValid())
	{
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	}
	return Qt::ItemIsEnabled;
}

void BlockModel::clear()
{
	beginResetModel();
	qDeleteAll(_indices);
	_indices.clear();
	endResetModel();
}

void BlockModel::createModel()
{
	beginResetModel();

	qDeleteAll(_indices);
	_indices.clear();

	kint ancestorIndex = 0;

	// Loop over all of the block's ancestors
	for(const MetaBlock* mb = _block->metaBlock(); mb != K_NULL; mb = mb->superMetaBlock())
	{
		// Retrieve the properties names for that block.
		QHash<QString,QVariant> propertiesNames = mb->blockProperty(Block::BlockPropertiesName).toHash();

		QVector<BlockPropertyIndex*> properties;

		const QMetaObject* mo = mb->blockMetaObject();

		kint propertyIndex = 0;
		for(kint i = mo->propertyOffset(); i < mo->propertyCount(); i++)
		{
			// Retrieve the property
			QMetaProperty property = mo->property(i);

			// We are not interested in non-design-able properties.
			if(!property.isDesignable(_block))
			{
				continue;
			}

			// Retrieve the property display name
			QString propertyName = propertiesNames.value(property.name()).toString();

			// Create a property index
			BlockPropertyIndex* index = new BlockPropertyIndex(
					this,
					ancestorIndex,
					propertyIndex,
					propertyName.isEmpty() ? QString("# %1").arg(property.name()) : propertyName,
					property
				);

			// Store the entry in the ancestor's list
			properties.append(index);
			// Get ready for the next entry.
			propertyIndex++;
		}

		// The block has no design-able properties.
		if(properties.isEmpty())
		{
			continue;
		}

		// Retrieve the block type name
		QString blockTypeName = mb->blockProperty(Block::BlockTypeName).toString();

		BlockAncestorIndex* index = new BlockAncestorIndex(
				this,
				ancestorIndex,
				blockTypeName.isEmpty() ? _block->metaBlock()->blockClassName() : blockTypeName,
				properties
			);

		// Add it to the list.
		_indices.append(index);
		ancestorIndex++;
	}

	endResetModel();
}

QVariant BlockModel::getDisplayName(const QModelIndex& index) const
{
	BlockModelPrivateIndex* mIndex = static_cast<BlockModelPrivateIndex*>(index.internalPointer());

	// Label column
	if(index.column() == 0)
	{
		return mIndex->displayName();
	}
	// Data column
	else if(index.column() == 1)
	{
		K_ASSERT( mIndex->type() == BlockModelPrivateIndex::Property )
		return static_cast<BlockPropertyIndex*>(mIndex)->blockProperty().read(_block);
	}

	return QVariant();
}
