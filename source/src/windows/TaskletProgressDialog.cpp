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

#include <windows/TaskletProgressDialog.hpp>
using namespace Gooey::windows;
using namespace Kore::parallel;

#include <KoreEngine.hpp>
using namespace Kore;

#include <QtGui/QPushButton>
#include <QtCore/QCoreApplication>

#define RANGE_MAX 1000

TaskletProgressDialog::TaskletProgressDialog(Tasklet* tasklet, QWidget* parent)
:	QProgressDialog(parent),
 	_tasklet(tasklet)
{
	// Set the window title of the progress dialog.
	setWindowTitle(tasklet->blockName());

	setRange(0, 0); // Default is to have an animated progress...
	setAutoClose(false);

	connect(_tasklet, SIGNAL(ended(kint)), SLOT(ended(kint)));
	connect(_tasklet, SIGNAL(progress(const QString&)), SLOT(progress(const QString&)));
	connect(_tasklet, SIGNAL(progress(kuint64,kuint64)), SLOT(progress(kuint64,kuint64)));

	QPushButton* button = new QPushButton(tr("Cancel"));
	setCancelButton(button);
	button->disconnect(this);
	connect(button, SIGNAL(clicked(bool)), SLOT(cancelTasklet()));
}

int TaskletProgressDialog::execTasklet()
{
	KoreEngine::RunTasklet(_tasklet, TaskletRunner::Asynchronous);
	return exec();
}

void TaskletProgressDialog::cancelTasklet()
{
	// Disable the cancel button.
	static_cast<QWidget*>(sender())->setEnabled(false);
	// Cancel the tasklet.
	_tasklet->cancel();
}

void TaskletProgressDialog::ended(kint status)
{
	switch(status)
	{
	case Tasklet::Completed:
		accept();
		break;
	default:
		reject();
		break;
	}
}

void TaskletProgressDialog::progress(const QString& message)
{
	setLabelText(message);
}

void TaskletProgressDialog::progress(kuint64 progress, kuint64 total)
{
	if(progress != total)
	{
		setRange(0, RANGE_MAX);
		double ratio = (kdouble)progress / (kdouble)total;
		setValue((int)( ratio * RANGE_MAX ));
	}
}

