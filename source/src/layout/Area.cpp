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
#include <layout/View.hpp>
using namespace Gooey::layout;

#include <GooeyModule.hpp>

#define K_BLOCK_TYPE Gooey::layout::Area
#include <BlockMacros.hpp>
K_BLOCK_BEGIN
	K_BLOCK_ICON("gooey/images/icons/area.png")
	K_BLOCK_ALLOCABLE
	K_BLOCK_PROPERTY_METHOD( Gooey::layout::Area::BlockProperty )
K_BLOCK_END

Area::Area()
:	Element(new QSplitter)
{
	init();
}

Area::Area(Qt::Orientation orientation)
:	Element(new QSplitter)
{
	init();
	this->orientation(orientation);
}

void Area::addBlock(Block* b)
{
	Element::addBlock(b);

	Element* element = static_cast<Element*>(b);

	// This is to be tested only when restoring a layout !
	if(element->fastInherits<View>())
	{
		View* view = static_cast<View*>(element);
		if(view->wasDetached())
		{
			view->wasDetached(false); // Clear the flag.
			view->widget()->show();
			return;
		}
	}

	splitter()->addWidget(element->widget());
	kint index = splitter()->indexOf(element->widget());
	splitter()->setCollapsible(index, false);
	element->widget()->show();
}

void Area::insertBlock(Block* b, kint index)
{
	Element::insertBlock(b, index);
	Element* element = static_cast<Element*>(b);
	splitter()->insertWidget(elementSplitterIndex(index), element->widget());
	splitter()->setCollapsible(index, false);
	element->widget()->show();
}

void Area::removeBlock(Block* b)
{
	Element::removeBlock(b);
	Element* element = static_cast<Element*>(b);
	// Remove the widget from the splitter.
	element->widget()->hide();
	element->widget()->setParent(K_NULL);
}

void Area::replace(Element* element, Element* newElement)
{
	K_ASSERT( element->library() == this )

	if(newElement->area())
	{
		newElement->area()->removeBlock(newElement);
	}

	if(element->isDetached())
	{
		QRect geometry = element->widget()->geometry();
		kint index = element->index();

		insertBlock(newElement, index);
		newElement->widget()->show();

		removeBlock(element);
		detach(static_cast<View*>(newElement));
		newElement->widget()->setGeometry(geometry);
	}
	else
	{
		QList<int> sizes = splitter()->sizes();
		kint index = element->index();

		insertBlock(newElement, index);

		removeBlock(element);

		splitter()->setSizes(sizes);
	}

	// Delete the replaced element...
	element->destroy();

	cleanup();
}

void Area::split(View* view, View* newView, Qt::Orientation orientation, kbool before)
{
	// Save the sizes
	QList<int> sizes = splitter()->sizes();

	// Begin by removing the view from its previous area if it has one.
	if(newView->area())
	{
		newView->area()->removeBlock(newView);
	}

	if(size() == 1)
	{
		this->orientation(orientation);
	}

	int index = qMax(0, view->index());
	if(orientation == splitter()->orientation()) // This is the same orientation.
	{
		insertBlock(newView, before ? index : index + 1);
		newView->widget()->show();

		// Deal with the sizes.
		kint size = sizes.at(index) / 2;
		sizes.replace(index, size);
		sizes.insert(index+1, size);
	}
	else // Orientation differs, we have to insert a new Area that will have the proper orientation.
	{
		Area* area = Area::StaticMetaBlock()->createBlockT<Gooey::layout::Area>();
		area->orientation(orientation);
		area->addBlock(view);
		area->split(view, newView, orientation, before);

		insertBlock(area, index);
	}

	// Restore the sizes.
	splitter()->setSizes(sizes);

	cleanup();
}

void Area::attach(View* view)
{
	K_ASSERT( view->isDetached() )
	splitter()->insertWidget(elementSplitterIndex(view->index()), view->widget());
	view->widget()->show();
}

kbool Area::canDetach(View*)
{
	if(splitter()->count() == 1)
	{
		return area() != K_NULL;
	}
	return true;
}

void Area::detach(View* view)
{
	K_ASSERT( view->Block::library() == this )

	QPoint position = view->widget()->mapToGlobal(QPoint(0,0));
	QSize sz = view->widget()->size();

	view->widget()->setParent(K_NULL);

	view->widget()->resize(sz);
	view->widget()->move(position);
	view->widget()->show();
}

void Area::close(Element* element)
{
	if(canClose(element))
	{
		// Last element in the Area
		if(size() == 1)
		{
			area()->close(this);
		}
		else
		{
			removeBlock(element);
			element->destroy();

			cleanup();
		}
	}
}

kbool Area::canClose(Element* element)
{
	if(element->isDetached())
	{
		return true;
	}
	else if(size() == 1)
	{
		return area() ? area()->canClose(this) : false;
	}
	return true;
}

void Area::init()
{
	blockName(tr("Layout Area"));
	splitter()->setAcceptDrops(false);
}

QSplitter* Area::splitter() const
{
	return privateWidget<QSplitter>();
}

void Area::cleanup()
{
	Area* parentArea = area();
	if(parentArea)
	{
		if(this->size() == 1)
		{
			// Replace this area by its only child element, this will delete this Area instance.
			parentArea->replace(this, at(0));
		}
		parentArea->cleanup();
	}
}

kint Area::elementSplitterIndex(kint elementIndex)
{
	int index = 0;
	for(int i = 0; i < elementIndex; i++)
	{
		index += at(i)->isDetached() ? 0 : 1;
	}
	return index;
}

Qt::Orientation Area::orientation() const
{
	return splitter()->orientation();
}

void Area::orientation(Qt::Orientation o)
{
	splitter()->setOrientation(o);
}

QVariant Area::BlockProperty(kint property)
{
	switch(property)
	{
	case BlockTypeName:
		return tr("Layout Area");
	default:
		return Element::BlockProperty(property);
	}
}
