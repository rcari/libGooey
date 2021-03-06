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

#include <windows/MainMenu.hpp>
#include <windows/MainWindow.hpp>
#include <windows/SettingsDialog.hpp>
using namespace Gooey::windows;

#include <GooeyEngine.hpp>
using namespace Gooey;

#include <QtGui/QMessageBox>

/* TRANSLATOR Gooey::windows::MainMenu */

MainMenu::MainMenu(MainWindow* window)
:	_mainWindow(window)
{
	_fileMenu = addMenu(tr("File"));
	_quitAction = _fileMenu->addAction(tr("Quit"), GooeyEngine::Instance(), SIGNAL(quitRequested()), QKeySequence::Quit);

	_windowMenu = addMenu(tr("Window"));
	_fullScreenAction = _windowMenu->addAction(tr("Fullscreen"));
	_fullScreenAction->setCheckable(true);
	_fullScreenAction->setChecked(_mainWindow->isFullScreen());
	_fullScreenAction->setShortcut(Qt::Key_F11);
	connect(_fullScreenAction, SIGNAL(toggled(bool)), _mainWindow, SLOT(setFullscreen(bool)));

	_aboutMenu = addMenu(tr("About"));
	_aboutQtAction = _aboutMenu->addAction(tr("About Qt"), this, SLOT(aboutQt()));

	_aboutMenu->addAction(tr("Settings (TEMP)"), this, SLOT(settings()));
}

QMenu* MainMenu::fileMenu()
{
	return _fileMenu;
}

QAction* MainMenu::quitAction()
{
	return _quitAction;
}

QMenu* MainMenu::windowMenu()
{
	return _windowMenu;
}
QAction* MainMenu::fullScreenAction()
{
	return _fullScreenAction;
}

QMenu* MainMenu::aboutMenu()
{
	return _aboutMenu;
}

QAction* MainMenu::aboutQtAction()
{
	return _aboutQtAction;
}

void MainMenu::aboutQt()
{
	QMessageBox::aboutQt(_mainWindow, tr("About the Qt Application Framework"));
}

void MainMenu::settings()
{
	SettingsDialog dialog;
	dialog.exec();
}
