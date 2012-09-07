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

#include <QtGui/QMenu>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>

#include <windows/View.hpp>
using namespace Gooey::windows;

ViewBar::ViewBar(View* v)
:	QFrame(v)
{
	QHBoxLayout* hLayout = new QHBoxLayout(this);
	QAbstractButton* button = new QPushButton(tr("Switch View"));
	connect(button, SIGNAL(clicked()), SLOT(switchView()));
	hLayout->addWidget(button);
	hLayout->addStretch();
	hLayout->setContentsMargins(0,0,0,0);
	hLayout->setSpacing(0);
	button = new QPushButton;
	button->setObjectName("closeButton");
	connect(button, SIGNAL(clicked()), parent(), SLOT(close()));
	hLayout->addWidget(button);
}

void ViewBar::switchView()
{
	QWidget* bar = static_cast<QWidget*>(sender());
	QMenu menu;
	menu.addAction(tr("Arf"));
	menu.addAction(tr("Erf"));
	menu.addAction(tr("Lol"));
	menu.addAction(tr("Loul"));
	menu.setMinimumWidth(bar->width());
	menu.exec(bar->mapToGlobal(QPoint(0,0)));
}

View::View(const QString& name)
:	QDockWidget(name)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setTitleBarWidget(new ViewBar(this));

}
