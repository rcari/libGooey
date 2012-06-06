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

#include <common/BlockMenu.hpp>
#include <common/BlockAction.hpp>
using namespace Gooey::common;

#include <data/Block.hpp>
using namespace Kore::data;

BlockMenu::SubMenu::~SubMenu()
{
	qDeleteAll(menus.values());
	menus.clear();
}

bool BlockMenu::BlockActionLessThan(BlockAction* first, BlockAction* second)
{
	return first->priority() == second->priority()
		?	( first->text() < second->text() )
		:	( first->priority() > second->priority() ); // We invert as sorting is in descending order !
}

BlockMenu::BlockMenu(Block* block)
:	_block(block),
 	_hoveredAction(K_NULL)
{
	_subMenu = new SubMenu;

	// Build the internal structure.
	for(const MetaBlock* mb = block->metaBlock(); mb != K_NULL; mb = mb->superMetaBlock())
	{
		// Retrieve all the block actions for the given block !
		QList<BlockExtension*> extensions = mb->blockExtensions(GOOEY_BLOCK_ACTION_NAME);
		for(kint i = 0; i < extensions.size(); i++)
		{
			BlockAction* action = static_cast<BlockAction*>(extensions.at(i));
			if(action->prepareForBlock(_block))
			{
				// Add the action to the menu.
				subMenu(action->categories())->actions.append(action);
			}
		}
	}

	// Generate the menu
	generateMenu(this, _subMenu);
}

BlockMenu::~BlockMenu()
{
	// Hover...
	clearHoveredAction();

	// Cleanup !
	delete _subMenu;
}

void BlockMenu::execBlockMenu(const QPoint& pt)
{
	BlockAction* result = static_cast<BlockAction*>( exec(pt) );
	if(result)
	{
		// Perform the action on the block !
		result->performAction(_block);
	}
}

void BlockMenu::leaveEvent(QEvent*)
{
	clearHoveredAction();
}

void BlockMenu::generateMenu(QMenu* menu, SubMenu* subMenu)
{
	// Sort the actions !
	qSort(subMenu->actions.begin(), subMenu->actions.end(), &BlockMenu::BlockActionLessThan);

	int priority = -1;
	for(kint i = 0; i < subMenu->actions.size(); i++)
	{
		BlockAction* action = subMenu->actions.at(i);

		// Insert a separator at each priority change !
		if(priority != -1 && priority != action->priority())
		{
			menu->addSeparator();
		}

		// Store the priority !
		priority = action->priority();

		// Add the action to the menu.
		menu->addAction(action);

		// Connect to the hover signal !
		connect(action, SIGNAL(hovered()), SLOT(actionHovered()));
	}

	// Add a separator only if there are entries in the menu before !
	if(!subMenu->actions.isEmpty())
	{
		menu->addSeparator();
	}

	// Add submenus
	for(QMap<QString,SubMenu*>::iterator it = subMenu->menus.begin(); it != subMenu->menus.end(); it++)
	{
		QMenu* tempSubMenu = menu->addMenu(it.key());
		generateMenu(tempSubMenu, it.value());
	}
}

BlockMenu::SubMenu* BlockMenu::subMenu(const QStringList& subNamesList)
{
	SubMenu* menu = _subMenu;

	for(kint i = 0; i < subNamesList.size(); i++)
	{
		SubMenu* subMenu = menu->menus.value(subNamesList.at(i), K_NULL);
		if(!subMenu)
		{
			subMenu = new SubMenu;
			subMenu->text = subNamesList.at(i);
			menu->menus.insert(subMenu->text, subMenu);
		}
		menu = subMenu;
	}

	return menu;
}

void BlockMenu::clearHoveredAction()
{
	if(_hoveredAction)
	{
		_hoveredAction->notifyHoveredOut();
		_hoveredAction = K_NULL;
	}
}

void BlockMenu::actionHovered()
{
	if(_hoveredAction)
	{
		_hoveredAction->notifyHoveredOut();
	}
	_hoveredAction = static_cast<BlockAction*>(sender());
	// Notify of the hovering !
	_hoveredAction->notifyHoveredIn();
}
