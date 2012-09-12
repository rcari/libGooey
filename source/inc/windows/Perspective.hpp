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

#include <GooeyEngine.hpp>
#include <GooeyExport.hpp>

#include <data/Block.hpp>
#include <QtGui/QAction>
#include <QtGui/QStandardItemModel>

namespace Gooey { namespace windows {

class MainWindow;
class ToolBar;
class View;

class GooeyExport Perspective : public Kore::data::Block
{
	Q_OBJECT
	K_BLOCK

public:
	Perspective();

	/*!
	 * \brief reset
	 * \param window the mainwindow to apply a default perspective on
	 * This will be called to reset the Perspective to its initial state.
	 */
	virtual void resetLayout() = K_NULL;

	/*!
	 * \brief restoreLayout
	 * \param window
	 */
	virtual bool restoreLayout(const QString& name);

	/*!
	 * \brief saveLayout
	 * \param window
	 */
	virtual void saveLayout(const QString& name);

	QAction* activateAction();

	bool isActive() const;

	QAbstractItemModel* viewsModel();

protected:
	virtual void library(Kore::data::Library* lib);
	MainWindow* window();
	void addViewType(const QString& displayName, const QIcon& icon, const QMetaObject* mo);
	void setCentralWidget(QWidget* widget);

signals:
	void activated();
	void deactivated();

private slots:
	void activationToggled(bool active);
	void splitView(View* view, Qt::Orientation direction);
	void replaceView(View* view, const QMetaObject* mo);

private:
	void setMainWindow(MainWindow* window);
	View* createView(const QString& name);
	View* createView(const QMetaObject* mo);
	QModelIndex indexForView(const QMetaObject* mo);
	const QMetaObject* viewForIndex(const QModelIndex& index);

	void registerView(View* v);
	void unregisterView(View* v);

	void registerToolBar(ToolBar* b);
	void unregisterToolBar(ToolBar* b);

private:
	QAction _action;
	QStandardItemModel _viewsModel;
	QHash<QString,const QMetaObject*> _viewTypes;

	QByteArray _windowState;
	QList<View*> _views;
	QList<ToolBar*> _toolBars;

	MainWindow* _window;
	QWidget*	_centralWidget;

	friend class MainWindow;
	friend class View;
};

}}
