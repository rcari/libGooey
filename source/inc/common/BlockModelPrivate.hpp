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

#include "BlockModel.hpp"

namespace Gooey { namespace common {

// No export, internal, private classes !

class BlockPropertyIndex;

class BlockModelPrivateIndex : public QObject
{
public:
	enum Type
	{
		Ancestor,
		Property,
	};

public:
	BlockModelPrivateIndex(QObject* parent, Type type, kint index, const QString& displayName)
	:	QObject(parent), // For proper Qt deletion
	 	_type(type),
	 	_index(index),
	 	_displayName(displayName)
	{}

	// For quick cast.
	inline Type type() { return _type; }

	inline kint index() { return _index; }

	inline const QString& displayName() { return _displayName; }

private:
	Type _type;
	kint _index;
	QString _displayName;
};

class BlockPropertyIndex : public BlockModelPrivateIndex
{
	Q_OBJECT

public:
	BlockPropertyIndex(BlockModel* model, kint ancestorIndex, kint index, const QString& displayName, QMetaProperty property)
	:	BlockModelPrivateIndex(model, Property, index, displayName),
	 	_model(model),
	 	_ancestorIndex(ancestorIndex),
	 	_property(property)
	{
		// Connect to the change signal !
		if(_property.hasNotifySignal())
		{
			/*
			 * 	Fix this with Qt 4.8.0 instead of using this "trick" !!
			 *	See http://bugreports.qt.nokia.com/browse/QTBUG-10637
			 *	QString signalSignature = QString("%1%2").arg(QSIGNAL_CODE).arg(_property.notifySignal().signature());
			 *	connect(_model->block(), qPrintable(signalSignature), SLOT(propertyChanged()));
			 */
			// This is the 4.8.0 fix!
			const int slotIndex = staticMetaObject.indexOfMethod(QMetaObject::normalizedSignature("propertyChanged()"));
			Q_ASSERT(slotIndex != -1);
			connect(_model->block(), _property.notifySignal(), this, staticMetaObject.method(slotIndex));
		}
		else if(_property.isWritable())
		{
			qWarning(
					"GooeyEngine / BlockModel / Block %s's writable property %s has no notify signal, "
					"using default unsafe blockChanged() signal.",
					_model->block()->metaObject()->className(),
					_property.name()
				);
			connect(_model->block(), SIGNAL(blockChanged()), SLOT(propertyChanged()));
		}
	}

	inline kint ancestorIndex() const { return _ancestorIndex; }
	inline QMetaProperty blockProperty() const { return _property; }

private slots:
	void propertyChanged()
	{
		_model->blockPropertyChanged(_ancestorIndex, index()); // Push the information to the model.
	}

private:
	BlockModel* _model;
	kint _ancestorIndex;
	QMetaProperty _property;
};

class BlockAncestorIndex : public BlockModelPrivateIndex
{
public:
	BlockAncestorIndex(BlockModel* model, kint index, const QString& displayName, const QVector<BlockPropertyIndex*>& properties)
	:	BlockModelPrivateIndex(model, Ancestor, index, displayName),
	 	_properties(properties)
	{
		for(kint i = 0; i < _properties.size(); i++)
		{
			_properties.at(i)->setParent(this); // For proper deletion.
		}
	}

	inline const QVector<BlockPropertyIndex*>& properties() const
	{
		return _properties;
	}

private:
	QVector<BlockPropertyIndex*> _properties;
};

}}
