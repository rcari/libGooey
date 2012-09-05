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

#include <data/Library.hpp>

#include "widgets/property/PropertyWidget.hpp"

#include "common/LibraryTreeModel.hpp"

#include <QtCore/QList>
#include <QtCore/QMetaObject>
#include <QtCore/QMultiHash>

#include <QtGui/QMimeSource>
#include <QtGui/QUndoStack>

namespace Gooey {

namespace layout { class View; }
namespace windows { class MainWindow; }

class GooeyExport GooeyEngine : public Kore::data::Library
{
	Q_OBJECT
	K_BLOCK

public:
	static kbool RegisterView(const Kore::data::MetaBlock*);
	static const QList<const Kore::data::MetaBlock*>& Views();

	static void RegisterViewInstance(Gooey::layout::View* view);
	static void UnregisterViewInstance(Gooey::layout::View* view);
	static const QList<Gooey::layout::View*>& InstantiatedViews();

	static kbool RegisterPropertyWidget(const QMetaObject*);
	static Gooey::widgets::PropertyWidget* CreatePropertyWidget(Kore::data::Block* block, kint propertyIndex);
	static Gooey::widgets::PropertyWidget* CreatePropertyWidgetForType(QString type);

	static void IconsPath(const QString& path);
	static const QString& IconsPath();
	static QIcon GetIcon(const QString& path);
	static QIcon GetBlockIcon(const Kore::data::Block* block);

	static Gooey::windows::MainWindow* MainWindow();
	static void RegisterMainWindow(Gooey::windows::MainWindow*);

	static Kore::data::Block* BlockPtrFromMimeSource(const QMimeSource* src);
	static Kore::data::Block* ByteArrayToBlockPtr(const QByteArray& byteArray);
	static QByteArray BlockPtrToByteArray(Kore::data::Block* block);

	static Gooey::common::LibraryTreeModel* RootModel();

	static kbool IsQuitting();
	static void RequestQuit();
	static void Quit();

	static GooeyEngine* Instance();

private:
	GooeyEngine();

signals:
	void quitRequested();
	void viewAppeared(Gooey::layout::View* view);

private:
	QList<const Kore::data::MetaBlock*> _views;

	QList<Gooey::layout::View*> _instantiatedViews;

	QMultiHash<QString,const QMetaObject*> _inputWidgets;

	QString _iconsPath;

	Gooey::windows::MainWindow* _mainWindow;

	Gooey::common::LibraryTreeModel* _rootModel;

	QUndoStack* _undoStack;

	kbool _quitting;

private:
	static GooeyEngine* _Instance;
};

}

#define G_VIEW_I( view ) kbool view::__G_Registered = Gooey::GooeyEngine::RegisterView( view::StaticMetaBlock() );

#define G_PROPERTY_WIDGET_I( widget ) const kbool widget::__G_Registered = Gooey::GooeyEngine::RegisterPropertyWidget( &widget::staticMetaObject );
#define G_PROPERTY_WIDGET_TYPES "G_PROPERTY_WIDGET_TYPES"

// Static block actions will be deleted with the GooeyEngine instance !
#define G_STATIC_BLOCK_ACTION_I( blockAction ) Gooey::common::BlockAction* blockAction::_gooey_block_action_instance = new blockAction(GooeyEngine::Instance());

#define G_MIME_BLOCK_INTERNAL	"application/vnd.gooey.block.pointer" // Mime for a pointer to an internal Block.

// Declare useful types for QVariant in a GUI context !
Q_DECLARE_METATYPE( Qt::KeyboardModifier )
