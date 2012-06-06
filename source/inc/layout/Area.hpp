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

#include <GooeyExport.hpp>

#include <layout/Element.hpp>

#include <QtCore/QPointer>

#include <QtGui/QSplitter>

namespace Gooey { namespace layout {

class View;

class GooeyExport Area : public Element
{
	Q_OBJECT
	Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE orientation STORED true)

	K_BLOCK

protected:
	Area();

public:
	Area(Qt::Orientation orientation);

public:
	virtual void addBlock(Block* b);
	virtual void removeBlock(Block* b);
	virtual void insertBlock(Block* b, kint index);

	void replace(Element* element, Element* newElement);

	void split(View* view, View* newView, Qt::Orientation orientation, kbool before = false);

	void attach(View* view);

	kbool canDetach(View* view);
	void detach(View* view);

	kbool canClose(Element* element);
	void close(Element* element);

	void cleanup();

	static QVariant BlockProperty(kint property);

private:
	void init();
	QSplitter* splitter() const;
	kint elementSplitterIndex(kint elementIndex);

	Qt::Orientation orientation() const;
	void orientation(Qt::Orientation o);
};

}}
