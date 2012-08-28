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
#include <GooeyModule.hpp>
#include <layout/Area.hpp>
#include <layout/View.hpp>
using namespace Gooey::layout;

using namespace Kore::data;

#include <QtCore/QEvent>
#include <QtGui/QMenu>
#include <QtGui/QStandardItemModel>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>

#define K_BLOCK_TYPE Gooey::layout::View
#include <data/BlockMacros.hpp>
K_BLOCK_BEGIN
	K_BLOCK_ICON("gooey/images/icons/view.png")
	K_BLOCK_VIRTUAL
	K_BLOCK_PROPERTY_METHOD( Gooey::layout::View::ViewProperties )
K_BLOCK_END

#include <data/BlockSettings.hpp>
using namespace Kore::data;
using namespace Kore;

View::View(QWidget* viewWidget)
:	Element(new QWidget),
 	_activeBlock(K_NULL),
 	_viewWidget(viewWidget),
 	_draggedBlock(K_NULL),
 	_overlayWidget(new ViewOverlay(viewWidget))
{
	widget()->setWindowTitle(tr("Unknown view"));
	widget()->setAcceptDrops(true);
	widget()->installEventFilter(this);

	// We do not want to show that !
	_overlayWidget->hide();

	_toolBar = new ViewBar;

	_toolBarComboBox = new QComboBox;
	_toolBarComboBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
	_toolBar->addWidget(_toolBarComboBox);

	_toolBarButton = new ViewBarButton(this);
	_toolBar->addWidget(_toolBarButton);

	_toolBar->addWidget(new ViewBarSpacer);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setSpacing(0);
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(_viewWidget, 1);
	layout->addWidget(_toolBar, 0);

	widget()->setLayout(layout);
}

void View::library(Library* lib)
{
	if(!hasParent())
	{
		// First time inserted, notify !
		GooeyEngine::RegisterViewInstance(this);
	}
	Element::library(lib);
	if(!hasParent())
	{
		// Removed from the tree, notify !
		GooeyEngine::UnregisterViewInstance(this);
	}
	else
	{
		polishView();
	}
}

kbool View::isViewForBlock(Block* block)
{
	return false;
}

Block* View::activeBlock()
{
	return _activeBlock;
}

kbool View::activeBlock(Block* b)
{
	if( b == K_NULL )
	{
		// The view is being cleared !
		return true;
	}

	if(!isViewForBlock(b))
	{
		// This view can't handle that block type !
		return false;
	}

	// Everything is OK, store the active block !
	_activeBlock = b;
	return true;
}

void View::setOverlayVisible(bool show)
{
	if(show)
	{
		QRect regionRect = _viewWidget->rect();
		_overlayWidget->move(regionRect.topLeft());
		_overlayWidget->raise();
		_overlayWidget->resize(regionRect.size());
		_overlayWidget->show();
	}
	else
	{
		_overlayWidget->hide();
	}
	_overlayWidget->setVisible(show);
}

ViewBar* View::privateBar()
{
	return _toolBar;
}

void View::polishView()
{
	if(checkFlag(Polished))
	{
		return;
	}

	// Combo box !
	setupComboBox();

	// View icon
	QIcon icon = GooeyEngine::GetBlockIcon(this);
	widget()->setWindowIcon(icon);

	// Name / text !
	QString viewTitle = this->metaBlock()->blockProperty(Block::BlockTypeName).toString();
	widget()->setWindowTitle(viewTitle);

	addFlag(Polished);
}

bool View::eventFilter(QObject* watched, QEvent* event)
{
	switch(event->type())
	{
	case QEvent::DragEnter:
		return privateDragEnterEvent(static_cast<QDragEnterEvent*>(event));
	case QEvent::DragMove:
		return privateDragMoveEvent(static_cast<QDragMoveEvent*>(event));
	case QEvent::DragLeave:
		return privateDragLeaveEvent(static_cast<QDragLeaveEvent*>(event));
	case QEvent::Drop:
		return privateDropEvent(static_cast<QDropEvent*>(event));
	default:
		return Element::eventFilter(watched, event);
	}
}

bool View::privateDragEnterEvent(QDragEnterEvent* event)
{
	_draggedBlock = GooeyEngine::BlockPtrFromMimeSource(event);
	if(_draggedBlock)
	{
		if(_draggedBlock->fastInherits<View>() && !isDetached())
		{
			_overlayWidget->raise();
			event->accept();
			return true;
		}
		else if(isViewForBlock(_draggedBlock) && event->source() != _viewWidget)
		{
			event->accept();
			return true;
		}
		else
		{
			_draggedBlock = K_NULL;
			event->ignore();
			return true;
		}
	}
	return dragEnterEvent(event);
}

bool View::privateDragMoveEvent(QDragMoveEvent* event)
{
	// A Block is being dragged on this view
	if(_draggedBlock)
	{
		// The drag is a view !
		if(_draggedBlock->fastInherits<View>())
		{
			// Early escape !
			if(_draggedBlock == this)
			{
				event->setDropAction(Qt::IgnoreAction);
				event->ignore();
				return true;
			}

			QRect regionRect = _viewWidget->rect();

			switch(dropRegion(event->pos()))
			{
			case InvalidRegion:
				_overlayWidget->hide();
				event->setDropAction(Qt::IgnoreAction);
				event->ignore();
				return true;
			case LeftRegion:
				regionRect.setRight(regionRect.width()/2);
				break;
			case TopRegion:
				regionRect.setBottom(regionRect.height()/2);
				break;
			case RightRegion:
				regionRect.setLeft(regionRect.width()/2);
				break;
			case BottomRegion:
				regionRect.setTop(regionRect.height()/2);
				break;
			case CenterRegion: // Nothing to do !
			default:
				break;
			}

			_overlayWidget->move(regionRect.topLeft());
			_overlayWidget->resize(regionRect.size());
			_overlayWidget->show();

			event->acceptProposedAction();
			event->accept();

			return true;
		}
		else if(isViewForBlock(_draggedBlock))
		{
			event->acceptProposedAction();
			event->accept();

			return true;
		}
	}

	return dragMoveEvent(event);
}

bool View::privateDragLeaveEvent(QDragLeaveEvent* event)
{
	if(_draggedBlock)
	{
		_draggedBlock = K_NULL; // Reset the dragged view.
		_overlayWidget->hide(); // Hide the overlay anyway !
		event->accept();
		return true;
	}
	return dragLeaveEvent(event);
}

bool View::privateDropEvent(QDropEvent* event)
{
	if(_draggedBlock)
	{
		// Drag is a view !
		if(_draggedBlock->fastInherits<View>())
		{
			View* draggedView = static_cast<View*>(_draggedBlock);
			if(draggedView == this)
			{
				event->setDropAction(Qt::IgnoreAction);
				event->ignore();
				return true;
			}

			//qDebug("View dropped @ %s", qPrintable(blockName()));

			switch(dropRegion(event->pos()))
			{
			case InvalidRegion: // Do nothing !
				event->setDropAction(Qt::IgnoreAction);
				event->ignore();
				break;
			case CenterRegion: // Replace
				event->acceptProposedAction();
				event->accept();
				area()->replace(this, draggedView);
				break;
			case LeftRegion: // Split
				event->acceptProposedAction();
				event->accept();
				area()->split(this, draggedView, Qt::Horizontal, true);
				break;
			case TopRegion: // Split
				event->acceptProposedAction();
				event->accept();
				area()->split(this, draggedView, Qt::Vertical, true);
				break;
			case RightRegion: // Split
				event->acceptProposedAction();
				event->accept();
				area()->split(this, draggedView, Qt::Horizontal, false);
				break;
			case BottomRegion: // Split
				event->acceptProposedAction();
				event->accept();
				area()->split(this, draggedView, Qt::Vertical, false);
				break;
			}

			_draggedBlock = K_NULL; // Reset the dragged view
			_overlayWidget->hide(); // Hide the overlay !

			return true;
		}
		else if(isViewForBlock(_draggedBlock))
		{
			event->acceptProposedAction();
			event->accept();
			activeBlock( _draggedBlock );
			_draggedBlock = K_NULL;
			return true;
		}
	}
	return dropEvent(event);
}

bool View::dragEnterEvent(QDragEnterEvent* event)
{
	event->setDropAction(Qt::IgnoreAction);
	event->accept();
	return true;
}

bool View::dragMoveEvent(QDragMoveEvent* event)
{
	event->ignore();
	return true;
}

bool View::dragLeaveEvent(QDragLeaveEvent* event)
{
	event->ignore();
	return true;
}

bool View::dropEvent(QDropEvent* event)
{
	event->ignore();
	return true;
}

View::DropRegion View::dropRegion(const QPoint& mousePos) const
{
	int x1 = _viewWidget->width() / 3;
	int x2 = x1*2;

	int y1 = _viewWidget->height() / 3;
	int y2 = y1*2;

	QRect rects[5];
	rects[CenterRegion] =	QRect(	x1,	y1,	x1,	y1	);
	rects[LeftRegion] =		QRect(	0,	y1,	x1,	y1	);
	rects[TopRegion] = 		QRect(	x1,	0,	x1,	y1	);
	rects[RightRegion] =	QRect(	x2,	y1,	x1,	y1	);
	rects[BottomRegion] =	QRect(	x1,	y2,	x1,	y1	);

	for(int i = 0; i < 5; i++)
	{
		if(rects[i].contains(mousePos))
		{
			return static_cast<View::DropRegion>(i);
		}
	}

	return InvalidRegion;
}

void View::completeMenu(QMenu& menu)
{
	Q_UNUSED(menu)
	// Nothing
}

void View::menu()
{
	QMenu menu;

	QAction* action;
	QMenu* submenu;

	submenu = menu.addMenu(tr("Switch"));
	submenu->setEnabled(!isDetached()); // Only allow switch on non-detached views.
	const QList<const Kore::data::MetaBlock*> views = GooeyEngine::Views();
	for(kint i = 0; i < views.size() && submenu->isEnabled(); i++)
	{
		const Kore::data::MetaBlock* viewMetaBlock = views.at(i);
		if(viewMetaBlock == metaBlock())
		{
			// We cannot switch to our own type !
			continue;
		}

		action = submenu->addAction(
				GooeyEngine::GetIcon(viewMetaBlock->blockIconPath()),
				viewMetaBlock->blockProperty(Kore::data::Block::BlockTypeName).toString(),
				this,
				SLOT(replace())
			);
		action->setToolTip(viewMetaBlock->blockProperty(View::ViewDescription).toString());
		action->setObjectName(viewMetaBlock->blockClassName());
	}

	submenu = menu.addMenu(tr("Split"));
	submenu->setEnabled(!isDetached());

	action = submenu->addAction(tr("Horizontal"), this, SLOT(splitHorizontal()));
	action->setIcon(GooeyEngine::GetIcon("gooey/images/icons/area.split.horizontal.png"));

	action = submenu->addAction(tr("Vertical"), this, SLOT(splitVertical()));
	action->setIcon(GooeyEngine::GetIcon("gooey/images/icons/area.split.vertical.png"));

	if(isDetached())
	{
		action = menu.addAction(tr("Attach"), this, SLOT(attach()));
	}
	else
	{
		action = menu.addAction(tr("Detach"), this, SLOT(detach()));
		action->setIcon(GooeyEngine::GetIcon("gooey/images/icons/area.detach.png"));
		action->setEnabled(area()->canDetach(this));
	}

	menu.addSeparator();

	action = menu.addAction(tr("Close"), this, SLOT(close()));
	action->setIcon(GooeyEngine::GetIcon("gooey/images/icons/area.close.png"));
	action->setEnabled(area()->canClose(this));

	// Let the child implementation add elements to the menu.
	completeMenu(menu);

	menu.exec(QCursor::pos(), action); // Align with close element.
}

void View::attach()
{
	area()->attach(this);
}

void View::detach()
{
	K_ASSERT( area()->canDetach(this) )
	area()->detach(this);
}

void View::close()
{
	area()->close(this);
}

void View::splitVertical()
{
	split(Qt::Vertical);
}

void View::splitHorizontal()
{
	split(Qt::Horizontal);
}

void View::split(Qt::Orientation orientation)
{
	View* view = K_BLOCK_CREATE_INSTANCE(Gooey::layout::View);
	if(this->activeBlock())
	{
		view->activeBlock(this->activeBlock());
	}
	area()->split(this, view, orientation);
}

void View::setupComboBox()
{
	QStandardItemModel* model = new QStandardItemModel(_toolBarComboBox); // For proper Qt deletion.

	QStandardItem* currentViewItem = K_NULL;

	const QList<const Kore::data::MetaBlock*> views = GooeyEngine::Views();
	for(kint i = 0; i < views.size(); i++)
	{
		const Kore::data::MetaBlock* viewMetaBlock = views.at(i);

		QIcon icon = GooeyEngine::GetIcon(viewMetaBlock->blockIconPath());
		QString text = viewMetaBlock->blockProperty(Kore::data::Block::BlockTypeName).toString();

		// Update the minimum contents length !
		_toolBarComboBox->setMinimumContentsLength( K_MAX( _toolBarComboBox->minimumContentsLength(), text.length() ) );

		QStandardItem* item = new QStandardItem(icon, text);
		item->setData((int)Replace, Qt::UserRole + 1);
		item->setData(viewMetaBlock->blockClassName(), Qt::UserRole + 2);
		item->setToolTip(viewMetaBlock->blockProperty(View::ViewDescription).toString());

		if(viewMetaBlock == metaBlock())
		{
			currentViewItem = item;
		}

		model->appendRow(item);
	}

	// Default font for description entries
	QFont font;
	font.setBold(true);

	// Sort the model !
	model->sort(0);

	// Views
	QStandardItem* item = new QStandardItem(tr("Available views"));
	item->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
	// Tweak the font !
	item->setData(font, Qt::FontRole);
	item->setFlags(Qt::ItemIsEnabled);
	model->insertRow(0,item);

	// Actions
	item = new QStandardItem(tr("Actions"));
	item->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
	// Tweak the font !
	item->setData(font, Qt::FontRole);
	item->setFlags(Qt::ItemIsEnabled);
	model->appendRow(item);

	item = new QStandardItem(
			GooeyEngine::GetIcon("gooey/images/icons/area.split.horizontal.png"),
			tr("Horizontal split")
		);
	item->setData((int)SplitHorizontal, Qt::UserRole + 1);
	model->appendRow(item);

	item = new QStandardItem(
			GooeyEngine::GetIcon("gooey/images/icons/area.split.vertical.png"),
			tr("Vertical split")
		);
	item->setData((int)SplitVertical, Qt::UserRole + 1);
	model->appendRow(item);

	item = new QStandardItem(
			GooeyEngine::GetIcon("gooey/images/icons/area.close.png"),
			tr("Close")
		);
	item->setData((int)Close, Qt::UserRole + 1);
	model->appendRow(item);

	// Store the view's index
	_viewComboIndex = model->indexFromItem(currentViewItem).row();
	_previousComboIndex = _viewComboIndex;

	// Set the model and current index
	_toolBarComboBox->setModel(model);
	_toolBarComboBox->setCurrentIndex(_viewComboIndex);

	// Listen to the combo box events !
	connect(_toolBarComboBox, SIGNAL(activated(int)), SLOT(viewComboIndexChanged(int)));
}

void View::replace()
{
	replace(QString(sender()->objectName()));
}

void View::replace(const QString& viewType)
{
	View* view = KoreEngine::CreateBlockT<View>(viewType);
	K_ASSERT( view )
	area()->replace(this, view);
}

void View::viewComboIndexChanged(int index)
{
	if(index == -1 || index == _viewComboIndex || index == _previousComboIndex)
	{
		// Nothing to do !
		return;
	}

	// Update the previous index !
	_previousComboIndex = index;

	// Retrieve the model
	QStandardItemModel* model = static_cast<QStandardItemModel*>(_toolBarComboBox->model());
	QStandardItem* item = model->item(index);
	// Retrieve the item's user data
	int verb = item->data(Qt::UserRole + 1).toInt();

	switch(verb)
	{
	case Replace:
		{
			QString viewType = item->data(Qt::UserRole + 2).toString();
			if(viewType == metaBlock()->blockClassName())
			{
				// No need to change if it's already that kind of view
				return;
			}
			// Disconnect from the combo box events -> it triggers multiple times and causes crashes !
			_toolBarComboBox->disconnect(this);
			// Replace the view
			replace(viewType);
		}
		break;
	case SplitHorizontal:
		_toolBarComboBox->setCurrentIndex(_viewComboIndex);
		splitHorizontal();
		break;
	case SplitVertical:
		_toolBarComboBox->setCurrentIndex(_viewComboIndex);
		splitVertical();
		break;
	case Close:
		if(area() && area()->canClose(this))
		{
			_toolBarComboBox->disconnect(this);
			close();
		}
		break;
	}
}

QVariant View::ViewProperties(kint property)
{
	switch(property)
	{
	case Block::BlockTypeName:
		return tr("View");
	default:
		return QVariant();
	}
}

ViewOverlay::ViewOverlay(QWidget* widget)
:	QFrame(widget)
{
	setAcceptDrops(false);
}
