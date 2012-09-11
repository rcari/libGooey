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

#include <common/GooeyStyle.hpp>
using namespace Gooey::common;

#include <QtGui/QPainter>
#include <QtGui/QStyleOption>
#include <QtGui/QStyleOptionButton>

static QColor mergedColors(const QColor &colorA, const QColor &colorB, int factor = 50)
{
	const int maxFactor = 100;
	QColor tmp = colorA;
	tmp.setRed((tmp.red() * factor) / maxFactor + (colorB.red() * (maxFactor - factor)) / maxFactor);
	tmp.setGreen((tmp.green() * factor) / maxFactor + (colorB.green() * (maxFactor - factor)) / maxFactor);
	tmp.setBlue((tmp.blue() * factor) / maxFactor + (colorB.blue() * (maxFactor - factor)) / maxFactor);
	return tmp;
}

void GooeyStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option,
									QPainter *painter, const QWidget *widget) const
{
	switch (element)
	{
	case PE_FrameDockWidget:
		return;
	/*case PE_PanelButtonCommand:
		if(const QStyleOptionButton* button = qstyleoption_cast<const QStyleOptionButton *>(option))
		{
			QStyleOptionButton buttonStyle(*button);
			if(buttonStyle.state & State_MouseOver)
			{
				buttonStyle.state ^= State_MouseOver;
				buttonStyle.palette.setColor(QPalette::ButtonText, Qt::red);
			}
			QPlastiqueStyle::drawPrimitive(element, &buttonStyle, painter, widget);
		}
		else
		{
			QPlastiqueStyle::drawPrimitive(element, option, painter, widget);
		}
		return;*/
	case PE_FrameFocusRect:
		return;
	case PE_IndicatorBranch:
		{
			int mid_h = option->rect.x() + option->rect.width() / 2;
			int mid_v = option->rect.y() + option->rect.height() / 2;
			int bef_h = mid_h;
			int bef_v = mid_v;
			int aft_h = mid_h;
			int aft_v = mid_v;
			static const int decoration_size = 9;

			QColor borderColor = option->palette.background().color().lighter(178);
			QColor crossColor = mergedColors(option->palette.background().color(), option->palette.text().color());

			painter->setPen(borderColor);

			if (option->state & State_Children)
			{
				int delta = decoration_size / 2;
				bef_h -= delta; bef_v -= delta; aft_h += delta; aft_v += delta;
				painter->drawRect(bef_h, bef_v, aft_h - bef_h, aft_v - bef_v);

				painter->setPen(crossColor);

				// Draw + or -
				painter->drawLine(bef_h + 2, mid_v, aft_h - 2, mid_v);
				if(!(option->state & State_Open))
					painter->drawLine(mid_h, bef_v + 2, mid_h, aft_v - 2);
			}

			painter->setPen(borderColor);

			if (option->state & State_Item)
			{
				if (option->direction == Qt::RightToLeft)
					painter->drawLine(option->rect.left(), mid_v, bef_h, mid_v);
				else
					painter->drawLine(aft_h, mid_v, option->rect.right(), mid_v);
			}

			if (option->state & State_Sibling)
				painter->drawLine(mid_h, aft_v, mid_h, option->rect.bottom());
			if (option->state & (State_Open | State_Children | State_Item | State_Sibling))
				painter->drawLine(mid_h, option->rect.y(), mid_h, bef_v);
			break;
		}
	default:
		QPlastiqueStyle::drawPrimitive(element, option, painter, widget);
		break;
	}
}

QIcon GooeyStyle::standardIconImplementation(StandardPixmap standardIcon, const QStyleOption *opt, const QWidget *widget) const
{
	switch(standardIcon)
	{
	case SP_DockWidgetCloseButton:
		return QIcon(":/gooey/images/icons/area.close.png");
	default:
		return QPlastiqueStyle::standardIconImplementation(standardIcon, opt, widget);
	}
}
