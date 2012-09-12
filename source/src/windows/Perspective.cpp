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

#include <QtCore/QSettings>

#include <windows/Perspective.hpp>
#include <windows/MainWindow.hpp>
#include <windows/View.hpp>
#include <windows/ToolBar.hpp>
using namespace Gooey::windows;

#include <GooeyEngine.hpp>
using namespace Gooey;

#include <GooeyModule.hpp>
#define K_BLOCK_TYPE Gooey::windows::Perspective
#include <data/BlockMacros.hpp>
K_BLOCK_BEGIN
	K_BLOCK_ICON_DEFAULT
	K_BLOCK_VIRTUAL
	K_BLOCK_PROPERTY_DEFAULT
K_BLOCK_END

#define MAIN_WINDOW_STATE	"mainWindowState"
#define PERSPECTIVES_DIR	"perspectives"
#define LAYOUTS_DIR			"views"
#define LAYOUT_DEFAULT		"default"
#define LAYOUT_NAME			"name"
#define LAYOUT_INDEX			"index"
#define LAYOUT_DATA			"data"

Perspective::Perspective()
:	_action(this),
	_window(K_NULL),
	_centralWidget(K_NULL)
{
	_action.setCheckable(true); // Togglable!
	connect(&_action, SIGNAL(toggled(bool)), SLOT(activationToggled(bool)));
}

bool Perspective::restoreLayout(const QString& layout)
{
	QSettings settings;
	settings.beginGroup("Gooey");
	settings.beginGroup( PERSPECTIVES_DIR );
	settings.beginGroup(objectClassName());

	QString actualLayout = layout.isNull()
		? actualLayout = settings.value( LAYOUT_DEFAULT ).toString()
		: layout;

	if(actualLayout.isNull())
	{
		// There is no stored default layout, reset!
		resetLayout();
		return true;
	}

	// Load the layout!
	settings.beginGroup(actualLayout);

	// Views
	int size = settings.beginReadArray( LAYOUTS_DIR );
	for(int i = 0; i < size; i++)
	{
		settings.setArrayIndex(i);

		const QString viewName = settings.value(LAYOUT_NAME).toString();
		const int viewIndex = settings.value(LAYOUT_INDEX).toInt();
		const QByteArray viewData = settings.value(LAYOUT_DATA).toByteArray();

		View* v = createView(viewName);

		// Set the object name to properly restore its state!
		v->setObjectName(QString("%1@%2").arg(viewName).arg(viewIndex));

		// Set the object saved properties!
		v->loadProperties(viewData);

		// Add to the view
		window()->addView(v);
	}
	settings.endArray();

	// Retrieve the main window layout...
	QByteArray mainWindowState = settings.value(MAIN_WINDOW_STATE).toByteArray();
	// And restore it
	window()->restoreState(mainWindowState);

	return true;
}

void Perspective::saveLayout(const QString& name)
{
	Q_UNUSED(name);
}

QAction* Perspective::activateAction()
{
	return &_action;
}

bool Perspective::isActive() const
{
	return _action.isChecked();
}

QAbstractItemModel* Perspective::viewsModel()
{
	return &_viewsModel;
}

void Perspective::library(Kore::data::Library* lib)
{
	// Set a default icon if none is set!
	if(_action.icon().isNull())
	{
		_action.setIcon(GooeyEngine::GetBlockIcon(this));
	}
	Block::library(lib);
}

MainWindow* Perspective::window()
{
	return _window;
}

void Perspective::addViewType(const QString& displayName, const QIcon& icon, const QMetaObject* mo)
{
	bool inheritsView = false;
	for(const QMetaObject* m = mo; m != K_NULL && !inheritsView; m = mo->superClass())
	{
		inheritsView = (m == &View::staticMetaObject);
	}

	if(!inheritsView)
	{
		qWarning("Gooey / Class %s does not inherit Gooey::windows::View: can't add to perspective %s.",
				mo->className(),
				metaObject()->className()
			);
		return;
	}
	else if(mo->constructorCount() == 0)
	{
		qWarning("Gooey / Class %s does not have an invokable constructor: can't add to perspective %s.",
				mo->className(),
				metaObject()->className()
			);
		return;
	}

	// Keep it in the hash!
	_viewTypes.insert(mo->className(), mo);

	// Add the model item
	QStandardItem* item = new QStandardItem(
			icon.isNull() ? QIcon(":/gooey/images/icons/view.icon.default.png") : icon,
			displayName
		);
	item->setData(QVariant::fromValue((void*)mo), Qt::UserRole + 1); // Store the metablock in the user property!
	_viewsModel.appendRow(item);
}

void Perspective::setCentralWidget(QWidget* widget)
{
	_centralWidget = widget;
}

void Perspective::setMainWindow(MainWindow* window)
{
	_window = window;
}

View* Perspective::createView(const QString& name)
{
	const QMetaObject* mo = _viewTypes.value(name, K_NULL);
	if(!mo)
	{
		qFatal("Gooey / Can not instantiate view of type %s", qPrintable(name));
		return K_NULL;
	}

	return createView(mo);
}

View* Perspective::createView(const QMetaObject* mo)
{
	View* v = static_cast<View*>(mo->newInstance());
	if(!v)
	{
		qFatal("Gooey / Can not instantiate view of type %s (no invokable constructor)", mo->className());
		return K_NULL;
	}
	return v;
}

void Perspective::registerView(View* v)
{
	// Cleanup upon deactivation!
	v->connect(this, SIGNAL(deactivated()), SLOT(deleteLater()));
	// Split
	connect(v, SIGNAL(split(View*,Qt::Orientation)), SLOT(splitView(View*,Qt::Orientation)));
	// Replace
	connect(v, SIGNAL(replace(View*,const QMetaObject*)), SLOT(replaceView(View*,const QMetaObject*)));
	// Set a unique name
	if(v->objectName().isNull())
	{
		v->setObjectName(QString("%1-%2").arg(v->metaObject()->className()).arg((qulonglong)v));
	}
	// And append to the views list...
	_views.append(v);
}

void Perspective::unregisterView(View* v)
{
	_views.removeOne(v);
}

void Perspective::registerToolBar(ToolBar* b)
{
	_toolBars.append(b);
}

void Perspective::unregisterToolBar(ToolBar* b)
{
	_toolBars.removeOne(b);
}

void Perspective::activationToggled(bool active)
{
	if(active)
	{
		// Add the views
		for(int i = 0; i < _views.size(); i++)
		{
			View* v = _views.at(i);
			_window->addView(v);
			v->show();
		}

		// Add the toolbars
		for(int i = 0; i < _views.size(); i++)
		{
			View* v = _views.at(i);
			_window->addView(v);
			v->show();
		}

		_window->setCentralWidget(_centralWidget);
		_centralWidget->show();

		_window->restoreState(_windowState);

		emit activated();
	}

	else
	{
		// Save the current state/layout
		_windowState = _window->saveState();

		// Remove the views
		for(int i = 0; i < _views.size(); i++)
		{
			View* v = _views.at(i);
			_window->removeDockWidget(v);
			v->hide();
		}

		// Remove the toolbars
		for(int i = 0; i < _toolBars.size(); i++)
		{
			ToolBar* t = _toolBars.at(i);
			_window->removeToolBar(t);
			t->hide();
		}

		_centralWidget->hide();

		emit deactivated();
	}
}

void Perspective::splitView(View* view, Qt::Orientation direction)
{
	_window->splitDockWidget(view, createView(view->metaObject()), direction);
}

void Perspective::replaceView(View* view, const QMetaObject* mo)
{
	_window->splitDockWidget(view, createView(mo), Qt::Vertical);
	view->close();
}

QModelIndex Perspective::indexForView(const QMetaObject* mo)
{
	// Linear search...
	for(QModelIndex i = _viewsModel.index(0,0); i.isValid(); i = i.sibling(i.row() + 1, 0))
	{
		const QMetaObject* m = viewForIndex(i);
		if(m == mo)
		{
			return i;
		}
	}
	return QModelIndex();
}

const QMetaObject* Perspective::viewForIndex(const QModelIndex& index)
{
	K_ASSERT( index.isValid() )
	return static_cast<const QMetaObject*>(_viewsModel.data(index, Qt::UserRole + 1).value<void*>());
}
