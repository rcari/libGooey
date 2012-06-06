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

#pragma once

#include <GooeyExport.hpp>

#include <QtCore/QAbstractListModel>

#include <data/Library.hpp>

#include <QtCore/QPointer>

namespace Gooey { namespace common {

class GooeyExport LibraryListTreeModel : public QAbstractListModel {

	Q_OBJECT

public:
	LibraryListTreeModel(Kore::data::Library* lib, QObject* parent = K_NULL);

	virtual QModelIndex index(kint row, kint column, const QModelIndex& parent = QModelIndex()) const;
	virtual kint rowCount(const QModelIndex& parent) const;
	virtual kint columnCount(const QModelIndex& parent) const;
	virtual QVariant data(const QModelIndex& index, kint role) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;

	QModelIndex getBlockIndex(Kore::data::Block* block) const;
	Kore::data::Library* rootLibrary();

private slots:
	void blockChanged();

	void addingBlock(kint index);
	void blockAdded(kint index);

	void insertingBlock(kint index);
	void blockInserted(kint index);

	void removingBlock(kint index);
	void blockRemoved(kint index);

	//void blocksSwapped(kint index1, kint index2);

	void clearing();
	void cleared();

private:
	void bindBlockEvents(Kore::data::Block* lib);
	void appendLibraryChildren(Kore::data::Library* lib, kint& pos);

private:
	QPointer<Kore::data::Library> _rootLibrary;
	QList<Kore::data::Block*> _list;
};

}}
