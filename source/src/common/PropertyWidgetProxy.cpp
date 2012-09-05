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

#include <common/PropertyWidgetProxy.hpp>
using namespace Gooey::common;

#include <widgets/GooeyCheckBox.hpp>
#include <widgets/GooeyDoubleSpinBox.hpp>
#include <widgets/GooeyLineEdit.hpp>
#include <widgets/GooeySpinBox.hpp>
using namespace Gooey::widgets;

PropertyWidgetProxy::PropertyWidgetProxy(QWidget* widget, QObject* object, QMetaProperty property)
:	QObject(widget), // To be destroyed with the widget !
 	_object(object),
 	_objectProperty(property),
 	_widget(widget),
 	_widgetUserProperty(widget->metaObject()->userProperty())
{
	// Check the object
	K_ASSERT( _objectProperty.isReadable() )
	K_ASSERT( _objectProperty.isEditable(object) )
	K_ASSERT( _objectProperty.hasNotifySignal() )
	// Check the widget
	K_ASSERT( _widgetUserProperty.isValid() )
	K_ASSERT( _widgetUserProperty.hasNotifySignal() )

	// Connect everything...
	const int objIdx = staticMetaObject.indexOfMethod(QMetaObject::normalizedSignature("objectPropertyValueChanged()"));
	Q_ASSERT( objIdx != -1 );
	connect(_object, _objectProperty.notifySignal(), this, staticMetaObject.method(objIdx));

	const int widIdx = staticMetaObject.indexOfMethod(QMetaObject::normalizedSignature("widgetUserPropertyValueChanged()"));
	Q_ASSERT( widIdx != -1 );
	connect(_widget, _widgetUserProperty.notifySignal(), this, staticMetaObject.method(widIdx));
}

void PropertyWidgetProxy::objectPropertyValueChanged()
{
	K_ASSERT( sender() == _object )
	QVariant value = _objectProperty.read(_object);
	_widget->blockSignals(true);
	_widgetUserProperty.write(_widget, value);
	_widget->blockSignals(false);
}

void PropertyWidgetProxy::widgetUserPropertyValueChanged()
{
	if(_object.isNull())
	{
		qWarning("The object does not exist anymore !");
		return;
	}
	QVariant value = _widgetUserProperty.read(_widget);
	_objectProperty.write(_object, value);
}

QWidget* PropertyWidgetProxy::GetPropertyWidget(QObject* object, QMetaProperty property)
{
	QWidget* result = K_NULL;

	switch(property.type())
	{
	case QVariant::Bool:
		result = new GooeyCheckBox;
		break;
	case QVariant::Double:
		result = new GooeyDoubleSpinBox;
		break;
	case QVariant::Int:
		result = new GooeySpinBox;
		break;
	case QVariant::UInt:
		{
			GooeySpinBox* spin = new GooeySpinBox;
			spin->minimum(0);
			result = spin;
		}
		break;
	case QVariant::String:
		result = new GooeyLineEdit;
		break;
	default:
		result = K_NULL;
		break;
	}

	if(result)
	{
		// Create the proxy for signals and stuff...
		new PropertyWidgetProxy(result, object, property);
	}

	return result;
}
