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
 *	DISCLAIMED. IN NO EVENT SHALL Romuald CARI BE LIABLE FOR ANY
 *	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <common/LibraryListModel.hpp>
using namespace Gooey::common;

#include <GooeyEngine.hpp>
using namespace Gooey;

using namespace Kore::data;

#include <QtCore/QModelIndex>
#include <QtGui/QIcon>

LibraryListModel::LibraryListModel(Library* lib, QObject* parent)
:	QAbstractListModel(parent),
 	_rootLibrary(lib)
{
	connect(lib, SIGNAL(addingBlock(kint)), SLOT(addingBlock(kint)));
	connect(lib, SIGNAL(blockAdded(kint)), SLOT(blockAdded(kint)));

	connect(lib, SIGNAL(insertingBlock(kint)), SLOT(insertingBlock(kint)));
	connect(lib, SIGNAL(blockInserted(kint)), SLOT(blockInserted(kint)));

	connect(lib, SIGNAL(removingBlock(kint)), SLOT(removingBlock(kint)));
	connect(lib, SIGNAL(blockRemoved(kint)), SLOT(blockRemoved(kint)));

	//connect(lib, SIGNAL(swappingBlocks(kint,kint)), SLOT(swappingBlocks(kint,kint)));
	connect(lib, SIGNAL(blocksSwapped(kint,kint)), SLOT(blocksSwapped(kint,kint)));

	connect(lib, SIGNAL(clearing()), SLOT(clearing()));
	connect(lib, SIGNAL(cleared()), SLOT(cleared()));

	// Bind to the present blocks.
	for(kint i = 0; i < _rootLibrary->size(); i++)
	{
		bindBlock(_rootLibrary->at(i));
	}
}

kbool LibraryListModel::hasChildren(const QModelIndex& index)
{
	if(_rootLibrary.isNull())
	{
		return false;
	}

	return !index.isValid() && !_rootLibrary->isEmpty();
}

kint LibraryListModel::rowCount(const QModelIndex& parent) const
{
	if(_rootLibrary.isNull() || parent.isValid())
	{
		return 0;
	}

	// Invalid parent: root item
	return _rootLibrary->size();
}

QModelIndex LibraryListModel::index(kint row, kint column, const QModelIndex& parent) const
{
	QModelIndex index = QAbstractListModel::index(row, column, parent);
	return index.isValid() ? createIndex(row, column, _rootLibrary->at(row)) : QModelIndex();
}

QVariant LibraryListModel::data(const QModelIndex& index, kint role) const
{
	if (!checkIndexAndModelIntegrity(index) || index.column() != 0)
	{
		return QVariant();
	}

	Block* block = _rootLibrary->at(index.row());
	switch(role)
	{
	case Qt::DisplayRole:
		for(const QMetaObject* mo = block->metaObject(); mo != &QObject::staticMetaObject; mo = mo->superClass())
		{
			QMetaProperty userProperty = mo->userProperty();
			if(userProperty.isValid())
			{
				return userProperty.read(block);
			}
		}
		return block->objectName();
	case Qt::ToolTipRole:
		return block->infoString();
	case Qt::DecorationRole:
		return GooeyEngine::GetBlockIcon( block );
	default:
		break;
	}
	return QVariant();
}

bool LibraryListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if(index.isValid() && role == Qt::EditRole)
	{
		Block* block = _rootLibrary->at(index.row());
		block->blockName(value.toString());
		return true;
	}
	return false;
}

QVariant LibraryListModel::headerData(int, Qt::Orientation, int) const
{
	return QVariant("Block");
}

Qt::ItemFlags LibraryListModel::flags(const QModelIndex &index) const
{
	if(!checkIndexAndModelIntegrity(index))
	{
		return Qt::NoItemFlags;
	}

	Block* block = _rootLibrary->at(index.row());

	Qt::ItemFlags flags = Qt::ItemIsEnabled;
	flags |= block->isLibrary() ? Qt::ItemIsDropEnabled : Qt::ItemIsEnabled;
	flags |= block->checkFlag(Block::Browsable) ? Qt::ItemIsSelectable : Qt::ItemIsEnabled;
	flags |= block->checkFlag(Block::SystemOwned) ? Qt::NoItemFlags : Qt::ItemIsDragEnabled;
	return flags;
}

Library* LibraryListModel::rootLibrary()
{
	return _rootLibrary;
}

Block* LibraryListModel::block(const QModelIndex& index)
{
	K_ASSERT( index.row() < _rootLibrary->size() )
	return _rootLibrary->at(index.row());
}

kbool LibraryListModel::checkIndexAndModelIntegrity(const QModelIndex& index) const
{
	return !_rootLibrary.isNull() && index.isValid() && (index.internalPointer() != K_NULL);
}

void LibraryListModel::bindBlock(Block* block)
{
	// Bind to this newly added block.
	connect(block, SIGNAL(blockNameChanged(const QString&)), SLOT(blockNameChanged()));
}

void LibraryListModel::unbindBlock(Block* block)
{
	block->disconnect(this);
}

void LibraryListModel::blockNameChanged()
{
	Block* block = static_cast<Block*>(sender());
	QModelIndex blockIndex = index(block->index(), 0, QModelIndex());
	emit dataChanged(blockIndex, blockIndex);
}

void LibraryListModel::addingBlock(kint index)
{
	beginInsertRows(QModelIndex(), index, index);
}

void LibraryListModel::blockAdded(kint index)
{
	endInsertRows();
	bindBlock(_rootLibrary->at(index));
}

void LibraryListModel::insertingBlock(kint index)
{
	addingBlock(index);
}

void LibraryListModel::blockInserted(kint index)
{
	blockAdded(index);
}

void LibraryListModel::removingBlock(kint index)
{
	unbindBlock(_rootLibrary->at(index));
	beginRemoveRows(QModelIndex(), index, index);
}

void LibraryListModel::blockRemoved(kint)
{
	endRemoveRows();
}

void LibraryListModel::swappingBlocks(kint, kint)
{
	// Nothing to do
}

void LibraryListModel::blocksSwapped(kint index1, kint index2)
{
	emit dataChanged(index(index1, 0, QModelIndex()), index(index1, 0, QModelIndex()));
	emit dataChanged(index(index2, 0, QModelIndex()), index(index2, 0, QModelIndex()));
}

void LibraryListModel::movingBlock(kint from, kint to)
{
	// Nothing to do
}

void LibraryListModel::blockMoved(kint from, kint to)
{
	emit dataChanged(
			index(K_MIN(from, to), 0, QModelIndex()),
			index(K_MAX(from, to), 0, QModelIndex())
		);
}

void LibraryListModel::clearing()
{
	beginRemoveRows(QModelIndex(), 0, _rootLibrary->size() - 1);
}

void LibraryListModel::cleared()
{
	endRemoveRows();
}
