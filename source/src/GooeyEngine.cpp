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

#include <GooeyEngine.hpp>
using namespace Gooey;
using namespace Gooey::common;

#include <GooeyModule.hpp>

#include <layout/View.hpp>
using namespace Gooey::layout;

#include <widgets/property/PropertyWidget.hpp>
using namespace Gooey::widgets;

#include <windows/MainWindow.hpp>
using namespace Gooey::windows;

#include <KoreApplication.hpp>
using namespace Kore;
using namespace Kore::data;

#include <QtCore/QDebug>
#include <QtCore/QMetaProperty>
#include <QtCore/QMetaClassInfo>

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>

#define K_BLOCK_TYPE Gooey::GooeyEngine
#include <data/BlockMacros.hpp>
K_BLOCK_BEGIN
	K_BLOCK_ICON_DEFAULT
	K_BLOCK_VIRTUAL
	K_BLOCK_PROPERTY_DEFAULT
K_BLOCK_END

GooeyEngine::GooeyEngine()
:	_iconsPath(":"),
 	_mainWindow(K_NULL),
 	_rootModel(K_NULL),
 	_undoStack(new QUndoStack(this)),
 	_quitting(false)
{
	addFlag(Block::System);
	blockName(tr("Gooey Engine"));
}

kbool GooeyEngine::RegisterView( const MetaBlock* mb )
{
	//qDebug("GooeyEngine / Registering view %s", mb->blockMetaObject()->className());

	// Register in the global factory store.
	Instance()->_views.append(mb);

	return true;
}

const QList<const Kore::data::MetaBlock*>& GooeyEngine::Views()
{
	return Instance()->_views;
}

void GooeyEngine::RegisterViewInstance(Gooey::layout::View* view)
{
	Instance()->_instantiatedViews.append(view);

	// Notify that the view appeared.
	emit Instance()->viewAppeared(view);
}

void GooeyEngine::UnregisterViewInstance(Gooey::layout::View* view)
{
	Instance()->_instantiatedViews.removeOne(view);
}

const QList<Gooey::layout::View*>& GooeyEngine::InstantiatedViews()
{
	return Instance()->_instantiatedViews;
}

kbool GooeyEngine::RegisterPropertyWidget(const QMetaObject* mo)
{
	kint typesIndex = mo->indexOfClassInfo(G_PROPERTY_WIDGET_TYPES);
	if(typesIndex == -1)
	{
		return false;
	}

	QString types = QString::fromAscii(mo->classInfo(typesIndex).value());
	QStringList typesList = types.split(';');

	for(kint i = 0; i < typesList.size(); i++)
	{
		Instance()->_inputWidgets.insert(typesList.at(i), mo);
	}

	//qDebug() << "GooeyEngine / Registering property widget" << mo->className() << "for type(s)" << typesList;

	return true;
}

PropertyWidget* GooeyEngine::CreatePropertyWidget(Block* block, kint propertyIndex)
{
	QMetaProperty property = block->metaObject()->property(propertyIndex);
	QString type = QString::fromAscii(property.typeName());

	const QMetaObject* mo = Instance()->_inputWidgets.value(type, K_NULL);
	if(mo == K_NULL)
	{
		qWarning("GooeyEngine / Could not find property widget for type %s !", property.typeName());
		return K_NULL;
	}

	// Create an instance
	PropertyWidget* widget = static_cast<PropertyWidget*>(mo->newInstance());
	// Setup the data
	widget->setData(block, propertyIndex);
	// Build the UI
	widget->update();

	return widget;
}

PropertyWidget* GooeyEngine::CreatePropertyWidgetForType(QString type)
{
	const QMetaObject* mo = Instance()->_inputWidgets.value(type, K_NULL);
	if(mo == K_NULL)
	{
		qWarning("GooeyEngine / Could not find property widget for type %s", qPrintable(type));
		return K_NULL;
	}

	// Create an instance and return it
	return static_cast<PropertyWidget*>(mo->newInstance());
}

void GooeyEngine::IconsPath(const QString& path)
{
	Instance()->_iconsPath = path;
}

const QString& GooeyEngine::IconsPath()
{
	return Instance()->_iconsPath;
}

QIcon GooeyEngine::GetIcon(const QString& path)
{
	if(path.startsWith(':'))
	{
		// The icon comes from resources, that's it, not customizable !
		return QIcon(path);
	}

	return QIcon(QString("%1/%2").arg(Instance()->_iconsPath).arg(path));
}

QIcon GooeyEngine::GetBlockIcon(const Block* block)
{
	return GetIcon(block->iconPath());
}

Gooey::windows::MainWindow* GooeyEngine::MainWindow()
{
	return Instance()->_mainWindow;
}

void GooeyEngine::RegisterMainWindow(Gooey::windows::MainWindow* window)
{
	K_ASSERT( !Instance()->_mainWindow  )
	Instance()->_mainWindow = window;
}

Block* GooeyEngine::BlockPtrFromMimeSource(const QMimeSource* src)
{
	if(src->provides(G_MIME_BLOCK_INTERNAL))
	{
		return ByteArrayToBlockPtr(src->encodedData(G_MIME_BLOCK_INTERNAL));
	}
	return NULL;
}

Block* GooeyEngine::ByteArrayToBlockPtr(const QByteArray& byteArray)
{
	Block* result;
	K_ASSERT( byteArray.size() == sizeof(result) )
	memcpy(&result, byteArray.constData(), sizeof(result));
	return result;
}

QByteArray GooeyEngine::BlockPtrToByteArray(Kore::data::Block* block)
{
	QByteArray result(sizeof(block), 0x00);
	memcpy(result.data(), &block, sizeof(block));
	return result;
}

LibraryTreeModel* GooeyEngine::RootModel()
{
	return Instance()->_rootModel
			?	Instance()->_rootModel
			:	Instance()->_rootModel = new LibraryTreeModel(KoreApplication::Instance()->dataLibrary(), Instance());
}

kbool GooeyEngine::IsQuitting()
{
	return Instance()->_quitting;
}

void GooeyEngine::RequestQuit()
{
	emit Instance()->quitRequested();
}

void GooeyEngine::Quit()
{
	Instance()->_quitting = true;

	if(Instance()->_mainWindow)
	{
		Instance()->_mainWindow->deleteLater();
	}

	QApplication::quit();
}

GooeyEngine* GooeyEngine::Instance()
{
	return _Instance ? _Instance : _Instance = new GooeyEngine;
}

GooeyEngine* GooeyEngine::_Instance = K_NULL;
