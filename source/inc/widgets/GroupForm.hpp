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

#include <QtGui/QButtonGroup>
#include <QtGui/QFormLayout>
#include <QtGui/QFrame>
#include <QtGui/QPushButton>

namespace Gooey { namespace widgets {

class GroupFormPrivate;

class GooeyExport GroupForm : public QFrame
{
	Q_OBJECT

public:
	GroupForm(QWidget* parent = NULL);

	int addGroup(const QString& title);
	void setGroupTitle(int group, const QString& title);
	void setGroupIcon(int group, const QIcon& icon);

	void addRow(int group, QWidget* label, QWidget* field);
	void addRow(int group, QWidget* field);
	void addRow(int group, const QString& labelText, QWidget* field);

	//void addRow(const QString& labelText, QLayout* field);

	void setExclusive(bool exclusive);

public slots:
	void expandAll();
	void collapseAll();

signals:
	void expand(bool);

private:
	QList<GroupFormPrivate*> _groups;
	QButtonGroup _group;
	int _lastGroupId;
};

class GooeyExport GroupFormPrivate : public QPushButton
{
	Q_OBJECT

	friend class GroupForm;

private:
	GroupFormPrivate(GroupForm* form);

	void addRow(QWidget* label, QWidget* field);
	void addRowToLayout(int row, QWidget* label, QWidget* field);
	void showGroup();
	void hideGroup();

private slots:
	void buttonToggled();

protected:
	virtual void paintEvent(QPaintEvent* e);
private:
	GroupForm*	_form;
	QWidgetList	_labels;
	QWidgetList	_fields;
};

}}
