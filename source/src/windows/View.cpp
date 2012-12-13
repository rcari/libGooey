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

#include <QtGui/QHBoxLayout>

#include <QtCore/QtDebug>

#include <windows/View.hpp>
#include <windows/Perspective.hpp>
using namespace Gooey::windows;

View::View()
:	_perspective(K_NULL)
{
	setAttribute(Qt::WA_DeleteOnClose);

	QFrame* titleBar = new QFrame;
	titleBar->setObjectName("_g_title");
	setTitleBarWidget(titleBar);

	QHBoxLayout* hLayout = new QHBoxLayout(titleBar);
	_viewCombo = new QComboBox;
	_viewCombo->blockSignals(true); // No signals emitted until ready!
	_viewCombo->setToolTip(tr("Switch view"));
	hLayout->addWidget(_viewCombo);

	hLayout->addStretch(1);

	_splitButton = new QPushButton();
	_splitButton->setObjectName("_g_split");
	_splitButton->setToolTip(tr("Split view"));
	hLayout->addWidget(_splitButton);

	_floatButton = new QPushButton();
	_floatButton->setObjectName("_g_float");
	_floatButton->setToolTip(tr("Detach/Attach view"));
	hLayout->addWidget(_floatButton);

	_closeButton = new QPushButton();
	_closeButton->setObjectName("_g_close");
	_closeButton->setToolTip(tr("Close view"));
	hLayout->addWidget(_closeButton);

	featuresHaveChanged(features());

	// Combobox changes
	connect(_viewCombo, SIGNAL(currentIndexChanged(int)), SLOT(comboIndexChanged(int)));
	// Close button
	connect(_closeButton, SIGNAL(clicked()), SLOT(close()));
	// Float button
	connect(_floatButton, SIGNAL(clicked()), SLOT(toggleFloat()));
	// When the dock floats, it can't be split!
	connect(this, SIGNAL(topLevelChanged(bool)), _splitButton, SLOT(setHidden(bool)));
	// When the features (closable & floatable) change, update the buttons accordingly!
	connect(this, SIGNAL(featuresChanged(QDockWidget::DockWidgetFeatures)), SLOT(featuresHaveChanged(QDockWidget::DockWidgetFeatures)));
}

View::~View()
{
	if(_perspective)
		_perspective->unregisterView(this);
}

void View::loadProperties(const QByteArray& data)
{
	Q_UNUSED(data);
}

QByteArray View::saveProperties()
{
	return QByteArray();
}

Qt::DockWidgetArea View::preferredArea() const
{
	return Qt::LeftDockWidgetArea;
}

void View::setPerspective(Perspective* p)
{
	K_ASSERT( _perspective == K_NULL )
	_perspective = p;
	// Register !
	_perspective->registerView(this);

	_viewCombo->setModel(_perspective->viewsModel());
	QModelIndex index = _perspective->indexForView(metaObject());
	K_ASSERT(index.isValid());
	_viewCombo->setCurrentIndex(index.row());
	_viewCombo->blockSignals(false); // Ready to emit signals now!
}

void View::comboIndexChanged(int newIndex)
{
	QModelIndex index = _viewCombo->model()->index(newIndex, _viewCombo->modelColumn(), _viewCombo->rootModelIndex());
	emit replace(this, _perspective->viewForIndex(index));
}

void View::featuresHaveChanged(QDockWidget::DockWidgetFeatures features)
{
	_closeButton->setVisible(features & DockWidgetClosable);
	_floatButton->setVisible(features & DockWidgetFloatable);
}

void View::toggleFloat()
{
	if(!isFloating())
	{
		hide(); // No flicker
		QRect g = QRect(
				parentWidget()->mapToGlobal(geometry().topLeft()),
				parentWidget()->mapToGlobal(geometry().bottomRight())
			);
		qDebug() << geometry() << g;
		setFloating(true);
		setGeometry(g);
		show();
	}
	else
	{
		setFloating(false);
	}
}
