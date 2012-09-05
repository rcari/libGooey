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

#pragma once

#include <GooeyExport.hpp>

#include <QtGui/QSpinBox>
#include <QtGui/QFrame>

namespace Gooey { namespace widgets {

class GooeyExport GooeySpinBox : public QFrame
{
	Q_OBJECT
	Q_PROPERTY( int maximum READ maximum WRITE maximum )
	Q_PROPERTY( int minimum READ minimum WRITE minimum )
	Q_PROPERTY( int singleStep READ singleStep WRITE singleStep )
	Q_PROPERTY( int value READ value WRITE value NOTIFY valueChanged USER true )

public:
	GooeySpinBox(QWidget* parent = 0);

	int minimum() const { return _spinBox->minimum(); }
	void minimum(int i) { _spinBox->setMinimum(i); }

	int maximum() const { return _spinBox->maximum(); }
	void maximum(int i) { _spinBox->setMaximum(i); }

	void setRange(int min, int max) { _spinBox->setRange(min, max); }

	int singleStep() const { return _spinBox->singleStep(); }
	void singleStep(int i) { _spinBox->setSingleStep(i); }

	int value() const { return _spinBox->value(); }
	void value(int i);
signals:
	void valueChanged(int);

protected:
	bool eventFilter(QObject* object, QEvent* event);

private:
	QSpinBox*	_spinBox;
	QPoint		_lastMousePos;
};

}}
