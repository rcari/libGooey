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

#include <widgets/GroupForm.hpp>
using namespace Gooey::widgets;

#include <Types.hpp>

#include <QtCore/QEvent>
#include <QtGui/QLabel>
#include <QtGui/QStyleOption>
#include <QtGui/QStylePainter>

GroupForm::GroupForm(QWidget* parent)
:	QFrame(parent)
{
	_group.setExclusive(false);
	QFormLayout* fLayout = new QFormLayout;
	fLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	fLayout->setMargin(0);
	fLayout->setSpacing(1);
	setLayout(fLayout);
}

int GroupForm::addGroup(const QString& title)
{
	// Setup the group
	GroupFormPrivate* group = new GroupFormPrivate(this);
	group->setText(title);
	// Connect the dots
	connect(this, SIGNAL(expand(bool)), group, SLOT(setChecked(bool)));
	// Append the group.
	_groups.append(group);

	// The private group adds itself to the FormLayout of this group form.

	return _groups.size() - 1; // Last inserted index.
}

void GroupForm::setGroupTitle(int group, const QString& title)
{
	_groups.at(group)->setText(title);
}

void GroupForm::setGroupIcon(int group, const QIcon& icon)
{
	_groups.at(group)->setIcon(icon);
}

void GroupForm::addRow(int group, QWidget* label, QWidget* field)
{
	label->setObjectName("LABEL"); // For CSS !
	_groups.at(group)->addRow(label, field);
}

void GroupForm::addRow(int group, QWidget* field)
{
	addRow(group, K_NULL, field);
}

void GroupForm::addRow(int group, const QString& labelText, QWidget* field)
{
	QLabel* label = new QLabel(labelText);
	label->setBuddy(field);
	addRow(group, label, field);
}

void GroupForm::setExclusive(bool exclusive)
{
	_group.setExclusive(exclusive);
}

void GroupForm::expandAll()
{
	emit expand(true);
}
void GroupForm::collapseAll()
{
	emit expand(false);
}

GroupFormPrivate::GroupFormPrivate(GroupForm* form)
:	_form(form)
{
	// Add to the layout
	static_cast<QFormLayout*>(form->layout())->addRow(this);
	setCheckable(true);
	setChecked(true);
	// Toggle information
	connect(this, SIGNAL(toggled(bool)), SLOT(buttonToggled()));
}

void GroupFormPrivate::addRow(QWidget* label, QWidget* field)
{
	K_ASSERT( field != K_NULL )

	QFormLayout* fLayout = static_cast<QFormLayout*>(_form->layout());

	_labels.append(label);
	_fields.append(field);

	if(isChecked())
	{
		int row;
		QFormLayout::ItemRole role;
		fLayout->getWidgetPosition(this, &row, &role);

		K_ASSERT( row != -1 )
		K_ASSERT( role == QFormLayout::SpanningRole )

		addRowToLayout(row + _labels.size(), label, field);
	}
}

void GroupFormPrivate::addRowToLayout(int row, QWidget* label, QWidget* field)
{
	QFormLayout* fLayout = static_cast<QFormLayout*>(_form->layout());

	// Show the field
	field->show();

	if(label)
	{
		// Show the label
		label->show();
		fLayout->insertRow(row, label, field); // Label + field
	}
	else
	{
		fLayout->insertRow(row, field); // Spanning field
	}
}

void GroupFormPrivate::buttonToggled()
{
	if(isChecked())
	{
		showGroup();
	}
	else
	{
		hideGroup();
	}
}

void GroupFormPrivate::showGroup()
{
	QFormLayout* fLayout = static_cast<QFormLayout*>(_form->layout());

	int row;
	QFormLayout::ItemRole role;
	fLayout->getWidgetPosition(this, &row, &role);

	K_ASSERT( row != -1 )
	K_ASSERT( role == QFormLayout::SpanningRole )

	for(kint i = 0; i < _labels.size(); i++)
	{
		addRowToLayout(row + i + 1, _labels.at(i), _fields.at(i));
	}
}

void GroupFormPrivate::hideGroup()
{
	QFormLayout* fLayout = static_cast<QFormLayout*>(_form->layout());
	for(kint i = 0; i < _labels.size(); i++)
	{
		// Deal with the label
		QWidget* label = _labels.at(i);
		if(label)
		{
			// Remove the item from the layout
			fLayout->removeWidget(label);
			label->hide();
		}
		// Deal with the field
		QWidget* field = _fields.at(i);
		fLayout->removeWidget(field);
		field->hide();
	}
}

void GroupFormPrivate::paintEvent(QPaintEvent* e)
{
	QStylePainter p(this);
	QStyleOptionButton option;
	initStyleOption(&option);
	p.drawControl(QStyle::CE_PushButton, option);
	p.translate(QPoint(12-width()/2, 0));
	p.drawPrimitive(
			isChecked()
			? QStyle::PE_IndicatorArrowDown
			: QStyle::PE_IndicatorArrowRight,
			option
		);
}
