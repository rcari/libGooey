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

#include <commands/ChangeBlockProperty.hpp>
using namespace Gooey::commands;

using namespace Kore::data;

kuint ChangeBlockProperty::Command_ID = qHash("Pantin::commands::ChangeBlockProperty");

ChangeBlockProperty::ChangeBlockProperty
(Block* block, const QMetaProperty& property, const QVariant& newValue, kbool canMerge, Command* command)
:	Command(command),
 	_block(block),
 	_propertyIndex(property.propertyIndex()),
 	_newValue(newValue),
 	_commandId(canMerge ? ChangeBlockProperty::Command_ID : -1)
{
	K_ASSERT( _propertyIndex != -1 )

	// Retrieve the actual property name.
	QString propertyName = _block->metaObject()->property(_propertyIndex).name();
	propertyName = _block->metaBlock()->blockProperty(Block::BlockPropertiesName).toHash()
			.value(propertyName, propertyName).toString();

	setText(QString("Changing %1 on %2").arg(propertyName, _block->blockName()));

	_oldValue = _block->metaObject()->property(_propertyIndex).read(_block);
}

int ChangeBlockProperty::id() const
{
	return _commandId;
}

bool ChangeBlockProperty::mergeWith(const QUndoCommand* command)
{
	// We can't merge booleans... Makes no sense !
	if(_oldValue.type() == QVariant::Bool)
	{
		return false;
	}

	const ChangeBlockProperty* other = static_cast<const ChangeBlockProperty*>(command);
	if( ((other->_block != _block) || (other->_propertyIndex != _propertyIndex)) && other != this )
	{
		return false;
	}

	_newValue = QVariant(other->_newValue);

	return true;
}

void ChangeBlockProperty::redoCommand()
{
	_block->metaObject()->property(_propertyIndex).write(_block, _newValue);
}

void ChangeBlockProperty::undoCommand()
{
	_block->metaObject()->property(_propertyIndex).write(_block, _oldValue);
}
