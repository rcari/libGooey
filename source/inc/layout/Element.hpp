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

#include <data/LibraryT.hpp>

#include <QtCore/QPointer>

#include <QtGui/QWidget>

namespace Gooey { namespace layout {

class Area;

class GooeyExport Element : public Kore::data::LibraryT<Element>
{
	Q_OBJECT
	Q_PROPERTY( QRect geometry READ geometry WRITE geometry STORED true DESIGNABLE false )
	Q_PROPERTY( bool wasDetached READ isDetached WRITE wasDetached STORED true DESIGNABLE false )

	K_BLOCK

	friend class Area;

protected:
	Element(QWidget* elementWidget);

public:
	virtual ~Element();

	QWidget* widget() const;

	kbool isHidden() const;
	kbool isDetached() const;
	void wasDetached(kbool);
	kbool wasDetached() const;

	QRect geometry() const;
	void geometry(const QRect& g);

	static QVariant BlockProperty(kint property);

protected:
	Area* area();

	template<typename T>
	T* privateWidget() const { return static_cast<T*>(_widget.data()); }

private:
	QPointer<QWidget>	_widget;
	kbool				_wasDetached;
};

}}
