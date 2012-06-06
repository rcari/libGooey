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

#include <common/LibraryListTreeModel.hpp>
using namespace Gooey::common;

#include <GooeyEngine.hpp>
using namespace Gooey;

using namespace Kore::data;

#include <QtCore/QModelIndex>
#include <QtGui/QIcon>

LibraryListTreeModel::LibraryListTreeModel(Library* lib, QObject* parent)
:	QAbstractListModel(parent),
 	_rootLibrary(lib)
{
	kint pos = 0;
	appendLibraryChildren(lib, pos);
	bindBlockEvents(lib);
}

QModelIndex LibraryListTreeModel::index(kint row, kint column, const QModelIndex& parent) const
{
	if (_rootLibrary.isNull() || !hasIndex(row, column, parent))
	{
		return QModelIndex();
	}

	K_ASSERT(row < _list.size())

	return createIndex(row, column, _list.at(row));
}

kint LibraryListTreeModel::rowCount(const QModelIndex& parent) const
{
	if(_rootLibrary.isNull())
	{
		return 0;
	}

	return _list.size();
}

kint LibraryListTreeModel::columnCount(const QModelIndex&) const
{
	return 1; // Constant !
}

QVariant LibraryListTreeModel::data(const QModelIndex& index, kint role) const
{
	if (_rootLibrary.isNull() || !index.isValid())
	{
		return QVariant();
	}

	Block* block = static_cast<Block*> ( index.internalPointer() );
	if(!block)
	{
		return QVariant();
	}

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
	}
	return QVariant();
}

bool LibraryListTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if(index.isValid() && role == Qt::EditRole)
	{
		Block* block = static_cast<Block*>(index.internalPointer());
		block->blockName(value.toString());
		return true;
	}
	return false;
}

QVariant LibraryListTreeModel::headerData(int, Qt::Orientation, int) const
{
	return QVariant("Block");
}

Qt::ItemFlags LibraryListTreeModel::flags(const QModelIndex &index) const
{
	if(_rootLibrary.isNull() || !index.isValid())
	{
		return Qt::ItemIsDropEnabled;
	}
	else
	{
		Block* block = static_cast<Block*>( index.internalPointer() );
		if(!block)
		{
			return Qt::ItemIsDropEnabled;
		}

		Qt::ItemFlags flags = Qt::ItemIsEnabled;
		flags |= block->isLibrary() ? Qt::ItemIsDropEnabled : Qt::ItemIsEnabled;
		flags |= block->checkFlag(Block::Browsable) ? Qt::ItemIsSelectable : Qt::ItemIsEnabled;
		flags |= block->checkFlag(Block::SystemOwned) ? Qt::NoItemFlags : Qt::ItemIsDragEnabled;
		return flags;
	}
}

Library* LibraryListTreeModel::rootLibrary()
{
	return _rootLibrary;
}

void LibraryListTreeModel::blockChanged()
{
	Block* block = static_cast<Block*>(sender());

	QModelIndex pos = index(_list.indexOf(block), 0, QModelIndex());
	emit dataChanged(pos, pos);
}

void LibraryListTreeModel::addingBlock(kint index)
{
	Library* lib = static_cast<Library*>(sender());
	kint pos = _list.indexOf(lib)+1;
	for(kint i = 0; i < index; i++)
	{
		if(lib->at(i)->isLibrary())
		{
			pos += static_cast<Library*>(lib->at(i))->totalSize()+1;
		}
		else
		{
			pos++;
		}
	}
	beginInsertRows(QModelIndex(), pos, pos);
}

void LibraryListTreeModel::blockAdded(kint index)
{
	Library* lib = static_cast<Library*>(sender());
	kint pos = _list.indexOf(lib)+1;
	for(kint i = 0; i < index; ++i)
	{
		if(lib->at(i)->isLibrary())
		{
			pos += static_cast<Library*>(lib->at(i))->totalSize()+1;
		}
		else
		{
			pos++;
		}
	}

	K_ASSERT(pos <= _list.size())

	_list.insert(pos, lib->at(index));

	endInsertRows();

	kint size = 0;
	if(lib->at(index)->isLibrary())
	{
		size = static_cast<Library*>(lib->at(index))->totalSize();
	}
	pos++;

	if(lib->at(index)->isLibrary())
	{
		beginInsertRows(QModelIndex(), pos, pos+size);
		appendLibraryChildren(static_cast<Library*>(lib->at(index)), pos);
		endInsertRows();
	}

	bindBlockEvents(lib->at(index));
}

void LibraryListTreeModel::insertingBlock(kint index)
{
	addingBlock(index);
}

void LibraryListTreeModel::blockInserted(kint index)
{
	blockAdded(index);
}

void LibraryListTreeModel::removingBlock(kint index)
{
	Library* lib = static_cast<Library*>(sender());
	kint pos = _list.indexOf(lib->at(index));
	kint size = 0;
	if(lib->at(index)->isLibrary())
	{
		size = static_cast<Library*>(lib->at(index))->totalSize();
	}
	beginRemoveRows(QModelIndex(), pos, pos+size);
	for(kint i = pos+size; i>=pos; --i)
	{
		_list.removeAt(i);
	}
}

void LibraryListTreeModel::blockRemoved(kint)
{
	endRemoveRows();
}

/*void LibraryListTreeModel::blocksSwapped(kint index1, kint index2)
{
	Library* lib = static_cast<Library*>(sender());
	QModelIndex libIndex = getBlockIndex(lib);
	QModelIndex lastBlockIndex = index((index1 > index2) ? index1 : index2, 0, libIndex);
	emit dataChanged(libIndex, lastBlockIndex);
}*/

void LibraryListTreeModel::clearing()
{
	beginRemoveRows(QModelIndex(), 0, _list.size());
}

void LibraryListTreeModel::cleared()
{
	endRemoveRows();

	_list.clear();
}

void LibraryListTreeModel::bindBlockEvents(Block* block)
{
	connect(block, SIGNAL(blockChanged()), SLOT(blockChanged()));

	Library* lib = block->isLibrary() ? static_cast<Library*>(block) : K_NULL;
	if(lib && lib->isBrowsable())
	{
		connect(lib, SIGNAL(addingBlock(kint)), SLOT(addingBlock(kint)));
		connect(lib, SIGNAL(blockAdded(kint)), SLOT(blockAdded(kint)));

		connect(lib, SIGNAL(insertingBlock(kint)), SLOT(insertingBlock(kint)));
		connect(lib, SIGNAL(blockInserted(kint)), SLOT(blockInserted(kint)));

		connect(lib, SIGNAL(removingBlock(kint)), SLOT(removingBlock(kint)));
		connect(lib, SIGNAL(blockRemoved(kint)), SLOT(blockRemoved(kint)));

		//connect(lib, SIGNAL(blocksSwapped(kint,kint)), SLOT(blocksSwapped(kint,kint)));

		connect(lib, SIGNAL(clearing()), SLOT(clearing()));
		connect(lib, SIGNAL(cleared()), SLOT(cleared()));

		for(kint i = 0; i < lib->size(); i++)
		{
			bindBlockEvents(lib->at(i));
		}
	}
}

void LibraryListTreeModel::appendLibraryChildren(Library* lib, kint& pos)
{
	for(kint i = 0; i < lib->size(); i++)
	{
		_list.insert(pos, lib->at(i));
		pos++;
		if(lib->at(i)->isLibrary())
		{
			appendLibraryChildren(static_cast<Library*>(lib->at(i)), pos);
		}
	}
}
