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

#pragma once

#include <GooeyExport.hpp>

#include <QtCore/QAbstractItemModel>

#include <data/Block.hpp>

#include <QtCore/QPointer>

namespace Gooey { namespace common {

class BlockModelPrivateIndex;

class GooeyExport BlockModel : public QAbstractItemModel {

	Q_OBJECT

public:
	BlockModel(Kore::data::Block* block, QObject* parent = K_NULL);

	Kore::data::Block* block();

	void blockPropertyChanged(kint ancestorIndex, kint propertyIndex);

	virtual kbool hasChildren(const QModelIndex& index);

	virtual QModelIndex parent(const QModelIndex& index) const;
	virtual QModelIndex index(kint row, kint column, const QModelIndex& parent) const;
	virtual kint rowCount(const QModelIndex& parent) const;
	virtual kint columnCount(const QModelIndex& parent) const;
	virtual QVariant data(const QModelIndex& index, kint role) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, kint role);
	virtual QVariant headerData(kint section, Qt::Orientation orientation, kint role) const;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;

private slots:
	void clear();
	void createModel();

protected:
	QMetaProperty blockProperty(const QModelIndex& index);

private:
	QVariant getDisplayName(const QModelIndex& index) const;

private:
	Kore::data::Block* _block;
	QVector<BlockModelPrivateIndex*> _indices;
};

}}
