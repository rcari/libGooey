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

#include "../GooeyExport.hpp"

#include <data/BlockExtension.hpp>

#include <QtGui/QAction>

#define GOOEY_BLOCK_ACTION_NAME "__gooey_block_action"

namespace Gooey { namespace common {

class BlockMenu;

class GooeyExport BlockAction : public QAction, public Kore::data::BlockExtension
{
	Q_OBJECT

	friend class BlockMenu;

public:
	enum Priority
	{
		LowPriority = QAction::LowPriority,

		SystemActions, // Copy, paste...

		NormalPriority = QAction::NormalPriority,

		HighPriority = QAction::HighPriority,
	};

public:
	BlockAction(QObject* parent);

	void setPriority(Priority priority);

	virtual QStringList categories() const;

	/*!
	 * Prepare the block action for the given block and return its state.
	 * @param Block The block to prepare the action for.
	 * @return true if it is a valid action for the given block, false if it is not.
	 */
	virtual kbool prepareForBlock(Kore::data::Block* block);

	virtual void performAction(Kore::data::Block* block) const = K_NULL;

protected:
	virtual void notifyHoveredIn();
	virtual void notifyHoveredOut();

signals:
	void hoveredIn();
	void hoveredOut();
};

}}

#define G_STATIC_BLOCK_ACTION private: static BlockAction* _gooey_block_action_instance;

