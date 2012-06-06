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

#include <layout/Area.hpp>
#include <layout/ViewBar.hpp>
#include <layout/View.hpp>
using namespace Gooey::layout;

#include <common/BlockDrag.hpp>
using namespace Gooey::common;

#include <GooeyEngine.hpp>
#include <KoreEngine.hpp>
using namespace Kore;

#include <QtGui/QApplication>

void ViewBar::polish()
{
	// This is to avoid problems of margins in the view bar.
	layout()->setContentsMargins(0,0,0,0);
	layout()->invalidate();
}

bool ViewBar::event(QEvent* event)
{
	switch(event->type())
	{
	case QEvent::Polish:
		polish();
		break;
	default:
		break;
	}

	return QToolBar::event(event);
}

ViewBarSpacer::ViewBarSpacer()
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

ViewBarButton::ViewBarButton(View* view)
:	_view(view)
{
	setIcon(GooeyEngine::GetIcon("gooey/images/icons/area.move.png"));
}

void ViewBarButton::mousePressEvent(QMouseEvent* event)
{
	_mouseDownPos = event->pos();
	QToolButton::mousePressEvent(event);
}

void ViewBarButton::mouseMoveEvent(QMouseEvent* event)
{
	if( (event->pos() - _mouseDownPos).manhattanLength() < QApplication::startDragDistance() )
	{
		// The move is not big enough: no drag !
		return;
	}

	BlockDrag* drag = new BlockDrag(this, _view);
	drag->exec(Qt::LinkAction | Qt::CopyAction);
}
