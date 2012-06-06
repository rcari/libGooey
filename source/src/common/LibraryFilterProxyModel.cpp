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
#include <common/LibraryFilterProxyModel.hpp>
using namespace Gooey::common;

#include <data/Block.hpp>
#include <data/Library.hpp>
using namespace Kore::data;

LibraryFilterProxyModel::LibraryFilterProxyModel(QObject* parent)
: QSortFilterProxyModel(parent), _rootBlock(K_NULL)
{
	setDynamicSortFilter(true);
}

bool LibraryFilterProxyModel::filterAcceptsColumn(int, const QModelIndex&) const
{
	return true;
}

bool LibraryFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
	QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

	if(!index.isValid())
	{
		return false;
	}

	Block* block = static_cast<Block*>(index.internalPointer());

	for(int i = 0; i < _metaBlocks.size(); i++)
	{
		if(block->fastInherits(_metaBlocks.at(i)))
		{
			return true;
		}
	}
	return false;
}
