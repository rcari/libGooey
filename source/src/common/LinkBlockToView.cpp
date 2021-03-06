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

#include <common/LinkBlockToView.hpp>
using namespace Gooey::common;

#include <layout/View.hpp>
using namespace Gooey::layout;

#include <GooeyEngine.hpp>
using namespace Gooey;

using namespace Kore::data;

LinkBlockToView::LinkBlockToView(View* view)
:	BlockAction(view)
{
	setText(tr("Edit with %1").arg(view->blockName()));
	setIcon(GooeyEngine::GetBlockIcon(view));
}

kbool LinkBlockToView::prepareForBlock(Block* block)
{
	// Only if the block is not already the view's active block !
	return static_cast<View*>(parent())->activeBlock() != block;
}

void LinkBlockToView::performAction(Block* block) const
{
	// Set the active block on the view !
	static_cast<View*>(parent())->activeBlock(block);
}

void LinkBlockToView::notifyHoveredIn()
{
	static_cast<View*>(parent())->setOverlayVisible(true);
	BlockAction::notifyHoveredIn();
}

void LinkBlockToView::notifyHoveredOut()
{
	static_cast<View*>(parent())->setOverlayVisible(false);
	BlockAction::notifyHoveredOut();
}
