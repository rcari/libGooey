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

#include <widgets/BlockPanel.hpp>
using namespace Gooey::widgets;

#include <KoreEngine.hpp>
using namespace Kore::data;
using namespace Kore;

#include <GooeyEngine.hpp>

BlockPanel::BlockPanel(Block* b)
{
	// Some blocks may not have a MetaBlock (simple libraries / simple blocks).
	QHash<QString,QVariant> propertiesNames;
	if(b->metaBlock() != K_NULL)
	{
		propertiesNames = b->metaBlock()->blockProperty(Block::BlockPropertiesName).toHash();
	}
	else
	{
		propertiesNames = Block::DefaultBlockProperty(Block::BlockPropertiesName).toHash();
		qWarning("Block %s / %s does not have a MetaBlock !", qPrintable(b->blockName()), qPrintable(b->objectClassName()));
	}

	for(const QMetaObject* mo = b->metaObject(); mo != &QObject::staticMetaObject; mo = mo->superClass())
	{
		// Check if this must be displayed.
		bool display = false;
		for(kint i = mo->propertyOffset(); !display && i < mo->propertyCount(); i++)
		{
			display |= mo->property(i).isDesignable(b);
		}

		if(!display)
			continue;

		MetaBlock* mb = KoreEngine::GetMetaBlock(mo->className());

		int group;

		if(mb != K_NULL)
		{
			group = addGroup( mb->blockProperty(Block::BlockTypeName).toString() );
			setGroupIcon(
					group,
					GooeyEngine::GetIcon( mb->blockIconPath() )
				);
		}
		else
		{
			group = addGroup( QString("UNDEFINED: %1").arg(mo->className()) );
		}

		for(kint i = mo->propertyOffset(); i < mo->propertyCount(); i++)
		{
			QMetaProperty property = mo->property(i);

			if(!property.isDesignable(b))
				continue;

			QWidget* widget = GooeyEngine::CreatePropertyWidget(b, i);

			if(widget)
			{
				connect(
						widget,
						SIGNAL(valueChanged(Kore::data::Block*, kint, const QVariant&)),
						SIGNAL(valueChanged(Kore::data::Block*, kint, const QVariant&))
					);
			}
			else
			{
				widget = new QWidget;
			}

			widget->setEnabled(property.isWritable());

			addRow(
					group,
					propertiesNames.value(
						property.name(),
						QString("MISSING: %1 !").arg(property.name())
					).toString(),
					widget
				);
		}
	}
}
