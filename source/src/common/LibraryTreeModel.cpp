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

#include <common/LibraryTreeModel.hpp>
using namespace Gooey::common;

#include <GooeyEngine.hpp>
using namespace Gooey;

using namespace Kore::data;

#include <QtCore/QModelIndex>
#include <QtGui/QIcon>

LibraryTreeModel::LibraryTreeModel(Library* lib, QObject* parent)
:	QAbstractItemModel(parent),
 	_rootLibrary(lib)
{
	bindBlockEvents(lib);
	setSupportedDragActions(Qt::LinkAction | Qt::MoveAction);
}

kbool LibraryTreeModel::hasChildren(const QModelIndex& index)
{
	if(_rootLibrary.isNull())
	{
		return false;
	}

	// Root element
	if(!index.isValid())
	{
		return _rootLibrary->isBrowsable() && !_rootLibrary->isEmpty();
	}

	// Retrieve the actual block (this is fast!)
	Block* block = getInternalBlock<Block>(index);
	if(block->isLibrary())
	{
		Library* lib = static_cast<Library*>(block);
		return lib->isBrowsable() && !lib->isEmpty();
	}
	else
	{
		// Block : leaf node !
		return false;
	}
}

QModelIndex LibraryTreeModel::parent(const QModelIndex& index) const
{
	if( !checkIndexAndModelIntegrity(index) )
	{
		return QModelIndex();
	}

	Block* block = getInternalBlock<Block>(index);
	return getBlockIndex(block->library());
}

QModelIndex LibraryTreeModel::index(kint row, kint column, const QModelIndex& parent) const
{
	if(_rootLibrary.isNull() || !hasIndex(row, column, parent))
	{
		return QModelIndex();
	}

	Library* lib = parent.isValid() ? getInternalBlock<Library>(parent) : _rootLibrary.data();
	if(lib->size() < row)
	{
		return QModelIndex();
	}

	return createIndex(row, column, lib->at(row));
}

kint LibraryTreeModel::rowCount(const QModelIndex& parent) const
{
	if(_rootLibrary.isNull())
	{
		// No data in this model
		return 0;
	}
	else if(!parent.isValid())
	{
		// Invalid parent: root item
		return _rootLibrary->size();
	}

	Block* block = getInternalBlock<Block>(parent);
	if(block->isLibrary())
	{
		// Library
		Library* lib = static_cast<Library*>( block );
		return lib->isBrowsable() ? lib->size() : 0;
	}
	else
	{
		// Standard leaf block
		return 0;
	}
}

kint LibraryTreeModel::columnCount(const QModelIndex&) const
{
	return 1; // Constant !
}

QVariant LibraryTreeModel::data(const QModelIndex& index, kint role) const
{
	K_ASSERT( index.column() == 0 )
	if(!checkIndexAndModelIntegrity(index))
	{
		return QVariant();
	}

	Block* block = getInternalBlock<Block>(index);
	K_ASSERT(block)
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
		return block->blockName();
	case Qt::ToolTipRole:
		return block->infoString();
	case Qt::DecorationRole:
		return GooeyEngine::GetBlockIcon( block );
	default:
		break;
	}
	return QVariant();
}

bool LibraryTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if(index.isValid() && role == Qt::EditRole)
	{
		Block* block = getInternalBlock<Block>(index);
		block->blockName(value.toString());
		return true;
	}
	return false;
}

QVariant LibraryTreeModel::headerData(int, Qt::Orientation, int) const
{
	return QVariant("Block");
}

Qt::ItemFlags LibraryTreeModel::flags(const QModelIndex &index) const
{
	if(!checkIndexAndModelIntegrity(index))
	{
		return Qt::NoItemFlags;
	}

	Block* block = getInternalBlock<Block>(index);

	QMetaProperty userProperty;

	for(const QMetaObject* mo = block->metaObject(); mo != &QObject::staticMetaObject; mo = mo->superClass())
	{
		userProperty = mo->userProperty();
		if(userProperty.isValid())
		{
			break;
		}
	}
	// TODO: This might be slightly more complex, it must be taken care of...
	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsDragEnabled;
	flags |= block->isLibrary() ? Qt::ItemIsDropEnabled : Qt::NoItemFlags;
	flags |= block->checkFlag(Block::Browsable) ? Qt::ItemIsSelectable : Qt::NoItemFlags;
	flags |= (block->checkFlag(Block::System|Block::SystemOwned) || !userProperty.isDesignable(block)) ? Qt::NoItemFlags : Qt::ItemIsEditable;
	flags |= (block->checkFlag(Block::System|Block::SystemOwned) || !userProperty.isDesignable(block)) ? Qt::NoItemFlags : Qt::ItemIsDragEnabled;
	return flags;
}

bool LibraryTreeModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int col, const QModelIndex& parent)
{
	// TODO: Worry about that :D
	return false;
}

QMimeData* LibraryTreeModel::mimeData(const QModelIndexList& indexes) const
{
	// TODO: Worry about that too...
	if(indexes.isEmpty())
	{
		return K_NULL;
	}

	K_ASSERT( indexes.size() == 1 ) // Only one item can be selected at a time !

	Block* block = getInternalBlock<Block>(indexes.first());

	QMimeData* data = new QMimeData;
	// Store the Block pointer in the block.
	data->setData(G_MIME_BLOCK_INTERNAL, GooeyEngine::BlockPtrToByteArray(block));
	return data;
}

QStringList LibraryTreeModel::mimeTypes() const
{
	// TODO: Worry about that too...
	return QStringList() << G_MIME_BLOCK_INTERNAL;
}

QModelIndex LibraryTreeModel::getBlockIndex(Block* block) const
{
	// Test for limit (crash-prone) cases
	if( block == K_NULL || _rootLibrary.isNull() )
	{
		return QModelIndex();
	}

	// Other cases (more standard).
	if( block == _rootLibrary || block->library() == K_NULL )
	{
		return QModelIndex();
	}

	return index(block->index(), 0, getBlockIndex(block->library()));
}

Library* LibraryTreeModel::rootLibrary()
{
	return _rootLibrary;
}

Block* LibraryTreeModel::block(const QModelIndex& index)
{
	return getInternalBlock<Block>(index);
}

kbool LibraryTreeModel::isModelValid() const
{
	return !_rootLibrary.isNull();
}

kbool LibraryTreeModel::checkIndexAndModelIntegrity(const QModelIndex& index) const
{
	return !_rootLibrary.isNull() && index.isValid() && (index.internalPointer() != K_NULL);
}

void LibraryTreeModel::blockChanged()
{
	Block* block = static_cast<Block*>(sender());
	QModelIndex blockIndex = getBlockIndex(block);

	emit dataChanged(blockIndex, blockIndex);
}

void LibraryTreeModel::addingBlock(kint index)
{
	Library* lib = static_cast<Library*>(sender());
	QModelIndex modelIndex = getBlockIndex(lib);
	K_ASSERT( lib == _rootLibrary ? true : modelIndex.isValid() )
	beginInsertRows(modelIndex, index, index);
}

void LibraryTreeModel::blockAdded(kint index)
{
	endInsertRows();

	// Bind to this newly added block.
	Library* lib = static_cast<Library*>(sender());
	bindBlockEvents(lib->at(index));
}

void LibraryTreeModel::insertingBlock(kint index)
{
	addingBlock(index);
}

void LibraryTreeModel::blockInserted(kint index)
{
	blockAdded(index);
}

void LibraryTreeModel::removingBlock(kint index)
{
	Library* lib = static_cast<Library*>(sender());
	unbindBlockEvents(lib->at(index));
	beginRemoveRows(getBlockIndex(lib), index, index);
}

void LibraryTreeModel::blockRemoved(kint)
{
	endRemoveRows();
}

void LibraryTreeModel::swappingBlocks(kint, kint)
{
	// Nothing to do
}

void LibraryTreeModel::blocksSwapped(kint index1, kint index2)
{
	// XXX: This is wrong... We'll need to deal with insertions / removal of childnodes.
	Library* lib = static_cast<Library*>(sender());
	QModelIndex libIndex = getBlockIndex(lib);
	emit dataChanged(index(index1, 0, libIndex), index(index1, 0, libIndex));
	emit dataChanged(index(index2, 0, libIndex), index(index2, 0, libIndex));
}

void LibraryTreeModel::clearing()
{
	Library* lib = static_cast<Library*>(sender());

	for(kint i = 0; i < lib->size(); i++)
	{
		unbindBlockEvents(lib->at(i));
	}

	(lib == _rootLibrary)
	? beginResetModel()
	: beginRemoveRows(getBlockIndex(lib), 0, lib->size() - 1);
}

void LibraryTreeModel::cleared()
{
	Library* lib = static_cast<Library*>(sender());

	if(lib == _rootLibrary)
	{
		endResetModel();
	}
	else
	{
		endRemoveRows();
	}
}

void LibraryTreeModel::bindBlockEvents(Block* block)
{
	QMetaProperty userProperty = block->metaObject()->userProperty();
	if(userProperty.isValid() && userProperty.hasNotifySignal())
	{
		static const int blockChangedIndex = staticMetaObject.indexOfSlot(QMetaObject::normalizedSignature("blockChanged()"));
		connect(block, userProperty.notifySignal(), this, staticMetaObject.method(blockChangedIndex));
	}
	else
	{
		qWarning(
				"%s - Block %s has no default user property with NOTIFY signal!",
				__FUNCTION__,
				qPrintable(block->objectClassName())
			);
	}

	Library* lib = block->isLibrary() ? static_cast<Library*>(block) : K_NULL;
	if(lib && lib->isBrowsable())
	{
		connect(lib, SIGNAL(addingBlock(kint)), SLOT(addingBlock(kint)));
		connect(lib, SIGNAL(blockAdded(kint)), SLOT(blockAdded(kint)));

		connect(lib, SIGNAL(removingBlock(kint)), SLOT(removingBlock(kint)));
		connect(lib, SIGNAL(blockRemoved(kint)), SLOT(blockRemoved(kint)));

		connect(lib, SIGNAL(blocksSwapped(kint,kint)), SLOT(blocksSwapped(kint,kint)));

		connect(lib, SIGNAL(clearing()), SLOT(clearing()));
		connect(lib, SIGNAL(cleared()), SLOT(cleared()));

		for(int i = 0; i < lib->size(); i++)
		{
			bindBlockEvents(lib->at(i));
		}
	}
}

void LibraryTreeModel::unbindBlockEvents(Block* block)
{
	//qDebug("Unbinding block events @ %p : %s / %p", this, qPrintable(block->objectName()), block);
	block->disconnect(this);
	if(block->isLibrary())
	{
		Library* lib = static_cast<Library*>(block);
		for(kint i = 0; i < lib->size(); i++)
		{
			unbindBlockEvents(lib->at(i));
		}
	}
}
