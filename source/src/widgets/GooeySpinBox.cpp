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

#include <widgets/GooeySpinBox.hpp>
using namespace Gooey::widgets;

#include <QtGui/QLineEdit>
#include <QtGui/QMouseEvent>
#include <QtGui/QVBoxLayout>

GooeySpinBox::GooeySpinBox(QWidget* parent)
:	QFrame(parent),
 	_spinBox(new QSpinBox)
{
	setRange(INT_MIN, INT_MAX);

	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->setSpacing(0);
	vLayout->setMargin(0);
	vLayout->setContentsMargins(1, 0, 1, 0);
	vLayout->addWidget(_spinBox, 1);

	setLayout(vLayout);

	// For mouse interaction
	QLineEdit* edit = _spinBox->findChild<QLineEdit*>();
	if(edit)
	{
		edit->installEventFilter(this);
	}
}

void GooeySpinBox::value(int i)
{
	_spinBox->blockSignals(true);
	_spinBox->setValue(i);
	_spinBox->blockSignals(false);
}

bool GooeySpinBox::eventFilter(QObject* object, QEvent* event)
{
	QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
	switch(event->type())
	{
	case QEvent::MouseButtonPress:
		if(mouseEvent->button() == Qt::LeftButton)
		{
			_lastMousePos = mouseEvent->globalPos();
		}
		break;
	case QEvent::MouseButtonRelease:
		if(mouseEvent->button() == Qt::LeftButton)
		{
			_lastMousePos = QPoint(); // Reset the point !
		}
		break;
	case QEvent::MouseMove:
		if(mouseEvent->buttons() & Qt::LeftButton)
		{
			QPoint pos = mouseEvent->globalPos();

			int dx = _spinBox->singleStep() * (pos.x() - _lastMousePos.x());

			if(mouseEvent->modifiers() & Qt::CTRL)
			{
				dx <<= 2;
			}
			else if(mouseEvent->modifiers() & Qt::SHIFT)
			{
				dx >>= 2;
			}

			_spinBox->setValue( _spinBox->value() + dx );

			// Update position !
			_lastMousePos = pos;
		}
		break;
	default:
		break;
	}
	return false;
}
