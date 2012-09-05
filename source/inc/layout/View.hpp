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

#include "Element.hpp"
#include "ViewBar.hpp"

#include <QtCore/QPointer>

#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QMenu>

namespace Gooey { namespace layout {

class Area;

class GooeyExport View : public Element
{
	Q_OBJECT

	K_BLOCK

	friend class ViewBar;
	friend class ViewBarButton;

	enum DropRegion
	{
		CenterRegion = 0,
		LeftRegion = 1,
		TopRegion = 2,
		RightRegion = 3,
		BottomRegion = 4,
		InvalidRegion,
	};

	enum Action
	{
		Replace = 1,
		SplitHorizontal,
		SplitVertical,
		Detach,
		Close,
	};

public:
	enum Flags
	{
		Polished = Element::MAX_FLAG,
	};

	enum Properties
	{
		ViewDescription = Block::MAX_PROPERTY,
		MAX_PROPERTY,
	};

protected:
	View(QWidget* viewWidget);

	virtual void library(Kore::data::Library* lib);

public:
	virtual kbool isViewForBlock(Block* block);
	Block* activeBlock();
	virtual kbool activeBlock(Block* b);

	void setOverlayVisible(bool show);

	static QVariant ViewProperties(kint property);

private slots:
	void menu();

	void attach();
	void detach();
	void close();
	void splitVertical();
	void splitHorizontal();
	void replace();
	void replace(const QString& viewType);

	void viewComboIndexChanged(int index);

protected:
	template<typename T>
	T* viewWidget() { return static_cast<T*>(_viewWidget); }

	template<typename T>
	const T* viewWidget() const { return static_cast<const T*>(_viewWidget); }

	ViewBar* privateBar();

	virtual void completeMenu(QMenu& menu);

	virtual bool eventFilter(QObject* watched, QEvent* event);
	virtual bool dragEnterEvent(QDragEnterEvent* event);
	virtual bool dragMoveEvent(QDragMoveEvent* event);
	virtual bool dragLeaveEvent(QDragLeaveEvent* event);
	virtual bool dropEvent(QDropEvent* event);

private:
	void polishView();
	bool privateDragEnterEvent(QDragEnterEvent* event);
	bool privateDragMoveEvent(QDragMoveEvent* event);
	bool privateDragLeaveEvent(QDragLeaveEvent* event);
	bool privateDropEvent(QDropEvent* event);

	DropRegion dropRegion(const QPoint& mousePos) const;

private:
	void split(Qt::Orientation orientation);
	void setupComboBox();

private:
	Block*			_activeBlock;
	QWidget*		_viewWidget;

	// Drag n drop
	Kore::data::Block*		_draggedBlock;

	// For views
	QWidget*				_overlayWidget;
	Qt::KeyboardModifier	_dropModifier;

	ViewBar*		_toolBar;
	QComboBox*		_toolBarComboBox;
	kint			_viewComboIndex;
	kint			_previousComboIndex;
	ViewBarButton*	_toolBarButton;
};

class GooeyExport ViewOverlay : public QFrame
{
	Q_OBJECT

	friend class View;

private:
	ViewOverlay(QWidget* viewWidget);
};

}}

#define G_VIEW	private: static kbool __G_Registered;
