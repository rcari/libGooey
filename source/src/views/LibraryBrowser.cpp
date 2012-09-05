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

#include <views/LibraryBrowser.hpp>
using namespace Gooey::views;

using namespace Gooey::layout;

#include <common/LibraryTreeModel.hpp>
#include <common/BlockMenu.hpp>
using namespace Gooey::common;

#include <KoreApplication.hpp>

#include <data/BlockSettings.hpp>
using namespace Kore::data;

#include <GooeyModule.hpp>

#define K_BLOCK_TYPE Gooey::views::LibraryBrowser
#include <data/BlockMacros.hpp>
K_BLOCK_BEGIN
	K_BLOCK_ICON("gooey/images/icons/libraryBrowser.png")
	K_BLOCK_ALLOCABLE
	K_BLOCK_PROPERTY_METHOD( Gooey::views::LibraryBrowser::ViewProperties )
K_BLOCK_END

#include <GooeyEngine.hpp>
G_VIEW_I( Gooey::views::LibraryBrowser )

#include <QtGui/QApplication>
#include <QtGui/QDrag>

LibraryBrowser::LibraryBrowser()
:	View(new QTreeView)
{
	blockName(tr("Library Browser"));

	// Drag enabled !
	viewWidget<QTreeView>()->setDragEnabled(true);

	// Context menu !
	viewWidget<QTreeView>()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(
			viewWidget<QTreeView>(),
			SIGNAL(customContextMenuRequested(const QPoint&)),
			SLOT(customContextMenuRequested(const QPoint&))
		);

	privateBar()->addAction(
			GooeyEngine::GetIcon("gooey/images/icons/global.home.png"), tr("Home"),
			this, SLOT(setRootHome())
		);

	_moveRootUp = privateBar()->addAction(
			GooeyEngine::GetIcon("gooey/images/icons/global.go.left.png"), tr("Move up"),
			this, SLOT(moveRootUp())
		);

	privateBar()->addAction(
			GooeyEngine::GetIcon("gooey/images/icons/global.go.right.png"), tr("Select root"),
			this, SLOT(setRoot())
		);

	privateBar()->addAction(
			GooeyEngine::GetIcon("gooey/images/icons/global.expand.png"), tr("Expand all"),
			viewWidget<QTreeView>(), SLOT(expandAll())
		);

	privateBar()->addAction(
			GooeyEngine::GetIcon("gooey/images/icons/global.collapse.png"), tr("Collapse all"),
			viewWidget<QTreeView>(), SLOT(collapseAll())
		);

	setRootHome();

	activeBlock(const_cast<Library*>(kApp->rootLibrary()));
}

kbool LibraryBrowser::isViewForBlock(Block* b)
{
	return b->fastInherits<Kore::data::Library>();
}

kbool LibraryBrowser::activeBlock(Block* b)
{
	Q_UNUSED(b);
	QAbstractItemModel* newModel;
	if(!View::activeBlock(b))
	{
		newModel = K_NULL;
	}
	else
	{
		newModel = new LibraryTreeModel(static_cast<Library*>(b), this);
	}

	if(viewWidget<QTreeView>()->model())
	{
		viewWidget<QTreeView>()->model()->deleteLater();
	}

	// Set the new model.
	viewWidget<QTreeView>()->setModel(newModel);

	return newModel != K_NULL;
}

void LibraryBrowser::customContextMenuRequested(const QPoint& pt)
{
	QModelIndex index = viewWidget<QTreeView>()->indexAt(pt);
	if(index.isValid())
	{
		viewWidget<QTreeView>()->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
		Block* b = static_cast<LibraryTreeModel*>(viewWidget<QTreeView>()->model())->block(index);
		// Create and execute the block menu.
		BlockMenu menu(b);
		menu.execBlockMenu(viewWidget<QTreeView>()->mapToGlobal(pt));
	}
}

void LibraryBrowser::moveRootUp()
{
	QTreeView* view = viewWidget<QTreeView>();
	QModelIndex index = view->rootIndex();

	// The parent node is the root of the model!
	if(!index.parent().isValid())
	{
		setRootHome();
		return;
	}

	view->setRootIndex(index.parent());
}

void LibraryBrowser::setRoot()
{
	if(viewWidget<QTreeView>()->selectionModel())
	{
		QModelIndexList indexes = viewWidget<QTreeView>()->selectionModel()->selectedIndexes();
		if(!indexes.isEmpty())
		{
			viewWidget<QTreeView>()->setRootIndex( indexes.first() );
			_moveRootUp->setEnabled(true);
		}
	}
}

void LibraryBrowser::setRootHome()
{
	viewWidget<QTreeView>()->setRootIndex(QModelIndex());
	_moveRootUp->setEnabled(false);
}

QVariant LibraryBrowser::ViewProperties(kint property)
{
	switch(property)
	{
	case Block::BlockTypeName:
		return tr("Library Browser");
	case Block::BlockSettings:
		{
			// Autosave delay
			QVariantMap dragThreshold;
			dragThreshold.insert(K_BLOCK_SETTING_DEFAULT,	5);
			dragThreshold.insert(K_BLOCK_SETTING_TEXT,		tr("Drag threshold"));
			dragThreshold.insert(K_BLOCK_SETTING_SUFFIX,	tr(" pixels"));

			// Global settings structure
			QVariantMap settings;
			settings.insert("dragThreshold", dragThreshold);

			return settings;
		}
	case View::ViewDescription:
		return tr("Tree view to navigate Library hierarchies.");
	default:
		return View::ViewProperties(property);
	}
}

void LibraryBrowserTreeView::mousePressEvent(QMouseEvent* event)
{
	_index = this->indexAt(event->pos());
	_mouseDownPos = event->pos();
	QTreeView::mousePressEvent(event);
}

void LibraryBrowserTreeView::mouseMoveEvent(QMouseEvent* event)
{
	if(	!_index.isValid() || (event->pos() - _mouseDownPos).manhattanLength() < QApplication::startDragDistance() )
	{
		QTreeView::mouseMoveEvent(event);
		return;
	}

	QDrag* drag = new QDrag(this);

	Block* block = static_cast<Block*>(_index.internalPointer());

	drag->setPixmap(GooeyEngine::GetBlockIcon(block).pixmap(64));

	QMimeData* data = new QMimeData;

	QByteArray ptr(sizeof(block), 0x00);
	memcpy(ptr.data(), &block, sizeof(block));

	data->setData(G_MIME_BLOCK_INTERNAL, ptr);
	drag->setMimeData(data);

	drag->exec();

	event->accept();
}
