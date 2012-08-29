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

#include <views/BlockEditor.hpp>
using namespace Gooey::views;
using namespace Gooey::layout;

#include <common/BlockModel.hpp>
#include <common/GooeyItemDelegate.hpp>
using namespace Gooey::common;

#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QPushButton>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QVBoxLayout>

#include <KoreApplication.hpp>
using namespace Kore::data;

#include <GooeyModule.hpp>

#define K_BLOCK_TYPE Gooey::views::BlockEditor
#include <data/BlockMacros.hpp>
K_BLOCK_BEGIN
	K_BLOCK_ICON_DEFAULT
	K_BLOCK_ALLOCABLE
	K_BLOCK_PROPERTY_METHOD( Gooey::views::BlockEditor::ViewProperties )
K_BLOCK_END

#include <GooeyEngine.hpp>
G_VIEW_I( Gooey::views::BlockEditor )

/* TRANSLATOR Gooey::views::BlockEditor */

class FilterProxyModel : public QSortFilterProxyModel
{
public:
	FilterProxyModel(QObject* parent)
	:	QSortFilterProxyModel(parent)
	{}

	bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
	{
		// This will only filter leave nodes!
		QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);
		return	sourceModel()->hasChildren(sourceIndex)
				?	true
				:	QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
	}
};

BlockEditor::BlockEditor()
:	View(new QWidget)
{
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setMargin(4);
	hLayout->setSpacing(4);

	hLayout->addWidget(_iconLabel = new QLabel, 0);
	hLayout->addWidget(_nameLabel = new QLabel, 1);
	hLayout->addStretch();
	hLayout->addWidget(_searchEdit = new QLineEdit);

	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->setMargin(0);
	vLayout->setSpacing(0);
	vLayout->addLayout(hLayout);
	vLayout->addWidget(_treeView = new QTreeView);
	_treeView->setItemDelegate(new GooeyItemDelegate(_treeView));

	viewWidget<QWidget>()->setLayout(vLayout);

	blockName(tr("Block editor"));

	privateBar()->addAction(
			GooeyEngine::GetIcon("gooey/images/icons/global.expand.png"), tr("Expand all"),
			_treeView, SLOT(expandAll())
		);

	privateBar()->addAction(
			GooeyEngine::GetIcon("gooey/images/icons/global.collapse.png"), tr("Collapse all"),
			_treeView, SLOT(collapseAll())
		);
}

kbool BlockEditor::isViewForBlock(Block* b)
{
	return b != K_NULL;
}

kbool BlockEditor::activeBlock(Block* b)
{
	if(!View::activeBlock(b))
	{
		return false;
	}

	// This is for the blockNameChanged signal !
	if(View::activeBlock())
	{
		View::activeBlock()->disconnect(this);
	}

	if(_treeView->model())
	{
		_treeView->model()->deleteLater();
	}

	_treeView->setModel(createBlockModel(b));

	if(_treeView->model()->rowCount(_treeView->rootIndex()) == 1)
	{
		// No need to display the inheritance tree as there is only one level of properties.
		QModelIndex index = _treeView->model()->index( 0, 0, _treeView->rootIndex() );
		// Move root to the first and only inheritance block
		_treeView->setRootIndex(index);
		// Open editors
		openPersistentEditors(index);
	}
	else if(_treeView->model()->rowCount(_treeView->rootIndex()) != 0)
	{
		// We have multiple inheritance levels...
		QModelIndex index = _treeView->rootIndex();
		openPersistentEditors(index);
	}

	// Icon + text.
	_iconLabel->setPixmap(GooeyEngine::GetBlockIcon(b).pixmap(32));

	_nameLabel->setText(b->metaObject()->userProperty().read(b).toString());

	// To change properly the name of the block !
	connect(b, SIGNAL(blockNameChanged(const QString&)), SLOT(blockNameChanged(const QString&)));

	// Expand all entries
	treeView()->expandAll();

	// Update label column width.
	treeView()->resizeColumnToContents(0);

	return true;
}

QTreeView* BlockEditor::treeView()
{
	return _treeView;
}

QAbstractItemModel* BlockEditor::createBlockModel(Block* b)
{
	QSortFilterProxyModel* proxy = new FilterProxyModel(_treeView);
	connect(_searchEdit, SIGNAL(textChanged(const QString&)), proxy, SLOT(setFilterWildcard(const QString&)));

	BlockModel* model = new BlockModel(b, proxy);
	proxy->setSourceModel(model);
	return proxy;
}

void BlockEditor::openPersistentEditors(const QModelIndex& index)
{
	// Leaf element in the Block model.
	if(_treeView->model()->rowCount(index) == 0)
	{
		K_ASSERT( index.column() == 0 )

		// The element directly on the right
		QModelIndex valueIndex = _treeView->model()->index(index.row(), 1, index.parent());

		K_ASSERT( valueIndex.isValid() )

		// Only display the editor if the field is edit-able.
		if(valueIndex.model()->flags(valueIndex) & Qt::ItemIsEditable)
		{
			_treeView->openPersistentEditor(valueIndex);
		}
	}
	else
	{
		const int rowCount = _treeView->model()->rowCount(index);
		for(int i = 0; i < rowCount; i++)
		{
			openPersistentEditors(_treeView->model()->index(i,0,index));
		}
	}
	/*
	const int rowCount = index.model()->rowCount(index);
	for(kint j = 0; j < rowCount; j++)
	{
		QModelIndex valueIndex = index.model()->index(j, 1, index);

		K_ASSERT( valueIndex.isValid() )

		// Only display the editor if the field is edit-able.
		if(valueIndex.model()->flags(valueIndex) & Qt::ItemIsEditable)
		{
			_treeView->openPersistentEditor(valueIndex);
		}
	}
	*/
}

void BlockEditor::blockNameChanged(const QString& name)
{
	_nameLabel->setText(name);
}

QVariant BlockEditor::ViewProperties(kint property)
{
	switch(property)
	{
	case Block::BlockTypeName:
		return tr("Block Editor");
	case View::ViewDescription:
		return tr("Editor for Block properties.");
	default:
		return QVariant();
	}
}
