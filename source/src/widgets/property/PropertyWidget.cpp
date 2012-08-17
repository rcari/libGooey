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

#include <widgets/property/PropertyWidget.hpp>
using namespace Gooey::widgets;

using namespace Kore::data;

#include <QtCore/QMetaMethod>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>

PropertyWidget::PropertyWidget()
:	_block(K_NULL),
 	_propertyIndex(K_NULL)
{
}

void PropertyWidget::valueChanged(const QVariant& newValue)
{
	emit valueChanged(_block, _propertyIndex, newValue);
}

QVariant PropertyWidget::value()
{
	QMetaProperty property = _block->metaObject()->property(_propertyIndex);
	return property.read(_block);
}

Block* PropertyWidget::block()
{
	return _block;
}

kint PropertyWidget::propertyIndex()
{
	return _propertyIndex;
}

void PropertyWidget::setData(Kore::data::Block* block, kint propertyIndex)
{
	_block = block;
	_propertyIndex = propertyIndex;

	QMetaProperty property = _block->metaObject()->property(_propertyIndex);
	if(property.hasNotifySignal())
	{
		static const int slotIndex = staticMetaObject.indexOfMethod(QMetaObject::normalizedSignature("update()"));
		Q_ASSERT(slotIndex != -1);
		connect(_block, property.notifySignal(), this, staticMetaObject.method(slotIndex));
	}
	else
	{
		qWarning(
				"%s / Block %s's property %s has no notify signal, it won't be updated!",
				__FUNCTION__,
				qPrintable(block->objectClassName()),
				property.name()
			);
	}
}
