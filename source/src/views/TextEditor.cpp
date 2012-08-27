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

#include <views/TextEditor.hpp>
using namespace Gooey::views;
#include <widgets/TabWidget.hpp>
using namespace Gooey::widgets;
using namespace Gooey::layout;

#include <KoreApplication.hpp>

#include <GooeyModule.hpp>

#define K_BLOCK_TYPE Gooey::views::TextEditor
#include <data/BlockMacros.hpp>
K_BLOCK_BEGIN
	K_BLOCK_ICON("gooey/images/icons/textEditor.png")
	K_BLOCK_ALLOCABLE
	K_BLOCK_PROPERTY_METHOD( Gooey::views::TextEditor::ViewProperties )
K_BLOCK_END

#include <data/BlockSettings.hpp>
using namespace Kore::data;

#include <GooeyEngine.hpp>
G_VIEW_I( Gooey::views::TextEditor )

#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QTextEdit>

TextEditor::TextEditor()
:	View(new TabWidget)
{
	blockName(tr("Text Editor"));

	TabWidget* tabs = viewWidget<TabWidget>();
	tabs->setTabsClosable(true);
	connect(tabs, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));

	QAction* closeAction = new QAction(tabs);
	closeAction->setShortcut(QKeySequence::Close);
	connect(closeAction, SIGNAL(triggered()), SLOT(closeActiveTab()));
	tabs->addAction(closeAction);

	_selector = new QComboBox;
	privateBar()->addWidget(_selector);

	_saveTimer.setInterval(metaBlock()->blockSetting("autoSaveDelay", 500).toInt()); // Save every 500 msec
	connect(&_saveTimer, SIGNAL(timeout()), SLOT(saveAll()));

	// Temp !
	activeBlock(K_NULL);
	activeBlock(K_NULL);
}

kbool TextEditor::activeBlock(Block* b)
{
	addTab(b);
	return false;
}

void TextEditor::closeActiveTab()
{
	TabWidget* tabs = viewWidget<TabWidget>();
	closeTab(tabs->currentIndex());
}

void TextEditor::closeTab(int tabIndex)
{
	TabWidget* tabs = viewWidget<TabWidget>();

	Tab tab = _tabs.at(tabIndex);

	if(tab.editor->document()->isModified())
	{
		saveTab(tabIndex);
	}

	// Remove the tab struct from the list
	_tabs.removeAt(tabIndex);

	// Delete the widget !
	tab.editor->deleteLater();

	// Update the global editor.
	tabs->setTabsVisible(_tabs.size() > 1);
}

void TextEditor::addTab(Block* b)
{
	TabWidget* tabs = viewWidget<TabWidget>();

	Tab newTab;
	newTab.block = b;
	newTab.editor = new QTextEdit;
	newTab.editor->setTabStopWidth(metaBlock()->blockSetting("tabSize", 16).toInt());

	_tabs.append(newTab);

	if(newTab.block != K_NULL)
	{
		tabs->addTab(
				newTab.editor,
				GooeyEngine::GetBlockIcon(b),
				b->blockName()
			); // TODO: Complete !!
	}
	else
	{
		tabs->addTab(newTab.editor, tr("Unknown"));
		newTab.editor->setPlainText(qApp->styleSheet());
	}
	connect(newTab.editor, SIGNAL(textChanged()), &_saveTimer, SLOT(start())); // Restart the timer every time the text changed.

	tabs->setTabsVisible(tabs->count() > 1);
}

void TextEditor::saveTab(int tabIndex)
{
	// Only save if necessary !
	if(_tabs.at(tabIndex).editor->document()->isModified())
	{
		_tabs.at(tabIndex).editor->document()->setModified(false);
		qApp->setStyleSheet(_tabs.at(tabIndex).editor->toPlainText());
	}
}

void TextEditor::saveAll()
{
	for(kint i = 0; i < _tabs.size(); i++)
	{
		saveTab(i);
	}
}

QVariant TextEditor::ViewProperties(kint property)
{
	switch(property)
	{
	case Block::BlockTypeName:
		return tr("Text Editor");
	case Block::BlockSettings:
		{
			// Autosave delay
			QVariantMap autoSaveDelay;
			autoSaveDelay.insert(K_BLOCK_SETTING_DEFAULT,	500);
			autoSaveDelay.insert(K_BLOCK_SETTING_TEXT,		tr("Autosave delay"));
			autoSaveDelay.insert(K_BLOCK_SETTING_SUFFIX,	tr(" ms"));
			autoSaveDelay.insert(K_BLOCK_SETTING_MINIMUM,	100);

			// Tab size
			QVariantMap tabSize;
			tabSize.insert(K_BLOCK_SETTING_DEFAULT, 	16);
			tabSize.insert(K_BLOCK_SETTING_TEXT,		tr("Tab size"));
			tabSize.insert(K_BLOCK_SETTING_SUFFIX,		tr(" pixels"));
			tabSize.insert(K_BLOCK_SETTING_MINIMUM,		16);

			// Global settings structure
			QVariantMap settings;
			settings.insert("autoSaveDelay",	autoSaveDelay);
			settings.insert("tabSize",			tabSize);

			return settings;
		}
	case View::ViewDescription:
		return tr("Editor for text.");
	default:
		return QVariant();
	}
}
