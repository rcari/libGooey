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

#include <common/GooeyItemDelegate.hpp>
using namespace Gooey::common;

#include <widgets/GooeyCheckBox.hpp>
#include <widgets/GooeyDoubleSpinBox.hpp>
#include <widgets/GooeyLineEdit.hpp>
#include <widgets/GooeySpinBox.hpp>
using namespace Gooey::widgets;

#include <QtCore/QEvent>

GooeyItemDelegate::GooeyItemDelegate(QObject* parent)
:	QStyledItemDelegate(parent),
 	_mapper(new QSignalMapper(this))
{
	connect(_mapper, SIGNAL(mapped(QWidget*)), SIGNAL(commitData(QWidget*)));
}

QWidget*
GooeyItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	const QAbstractItemModel* model = index.model();
	QVariant data = model->data(index, Qt::EditRole);

	QWidget* result = K_NULL;

	// Static types...
    switch((int)data.type())
	{
	case QMetaType::Bool:
		result = new GooeyCheckBox(parent);
		break;
	case QMetaType::Double:
	case QMetaType::Float:
		result = new GooeyDoubleSpinBox(parent);
		break;
	case QMetaType::Int:
		result = new GooeySpinBox(parent);
		break;
	case QMetaType::UInt:
		result = new GooeySpinBox(parent);
		static_cast<GooeySpinBox*>(result)->minimum(0); // Unsigned integer !
		break;
	case QMetaType::QString:
		result = new GooeyLineEdit(parent);
		break;
	default:
		result = QStyledItemDelegate::createEditor(parent, option, index);
		break;
	}

	QMetaProperty property = result->metaObject()->userProperty();
	if(property.hasNotifySignal())
	{
		connect(result, SIGNAL(destroyed(QObject*)), SLOT(editorDestroyed(QObject*)));
		_mapper->setMapping(result, result);
		const int index = _mapper->metaObject()->indexOfMethod(QMetaObject::normalizedSignature("map()"));
		Q_ASSERT( index != -1 );
		connect(result, property.notifySignal(), _mapper, _mapper->metaObject()->method(index));
	}
	else
	{
		qWarning("GooeyItemDelegate: The editor widget %s has no notify signal on its USER property!", result->metaObject()->className());
	}

	return result;
}

void
GooeyItemDelegate::editorDestroyed(QObject* editor)
{
	_mapper->removeMappings(editor);
}
