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

#include <windows/MainWindow.hpp>
#include <windows/MainMenu.hpp>
using namespace Gooey::windows;

#include <GooeyEngine.hpp>
using namespace Gooey;

#include <views/LibraryBrowser.hpp>
using namespace Gooey::views;

#include <QtCore/QBuffer>
#include <QtCore/QSettings>

#include <QtGui/QApplication>
#include <QtGui/QCloseEvent>

#include <serialization/KoreV1.hpp>
using namespace Kore::serialization;

using namespace Kore::data;

#define ROOT_AREA	"ROOT_AREA"
#define GEOMETRY	"GEOMETRY"

MainWindow::MainWindow()
{
	GooeyEngine::RegisterMainWindow(this);

	loadWindowGeometry();

	_mainMenu = new MainMenu(this);
	setMenuBar(_mainMenu);

	setAttribute(Qt::WA_DeleteOnClose);

	setWindowTitle(
			QString("%1 (%2) - %3 bits %4")
			.arg(QApplication::applicationName(), QApplication::applicationVersion())
			.arg(8 * sizeof(size_t))
#ifdef K_DEBUG
			.arg("debug")
#else
			.arg("release")
#endif
		);

	_statusBar = new QStatusBar;
	setStatusBar(_statusBar);
}

MainWindow::~MainWindow()
{
	// Save the geometry.
	saveWindowGeometry();
}

MainMenu* MainWindow::mainMenu()
{
	return _mainMenu;
}

QStatusBar* MainWindow::statusBar()
{
	return _statusBar;
}

void MainWindow::setFullscreen(bool full)
{
	setWindowState(
			full
			? Qt::WindowFullScreen
			: Qt::WindowNoState
		);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	// Save the geometry.
	saveWindowGeometry();

	if(GooeyEngine::IsQuitting())
	{
		QMainWindow::closeEvent(event);
		return;
	}
	event->ignore();
	// The user requested to quit !
	GooeyEngine::RequestQuit();
}

void MainWindow::loadWindowGeometry()
{
	QSettings settings;
	settings.beginGroup("gooey");
	settings.beginGroup("mainWindow");

	if(settings.contains(GEOMETRY))
	{
		restoreGeometry(settings.value(GEOMETRY).toByteArray());
	}
	else
	{
		resize(800,600); // Default is to set the size of the window!
	}
}

void MainWindow::saveWindowGeometry()
{
	QSettings settings;
	settings.beginGroup("gooey");
	settings.beginGroup("mainWindow");
	settings.setValue(GEOMETRY, saveGeometry());
}
