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

#include <windows/MainWindow.hpp>
#include <windows/SettingsDialog.hpp>
using namespace Gooey::windows;

#include <QtGui/QFormLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpinBox>
#include <QtGui/QDoubleSpinBox>

#include <GooeyEngine.hpp>
using namespace Gooey;

#include <KoreEngine.hpp>
using namespace Kore;

#include <data/BlockSettings.hpp>
using namespace Kore::data;

SettingsDialog::SettingsDialog()
:	QDialog(GooeyEngine::MainWindow())
{
	// Put the settings in the right sub group.
	settings.beginGroup(K_BLOCK_SETTINGS_GROUP);

	setWindowTitle(tr("Settings"));

	QHBoxLayout* hLayout = new QHBoxLayout;
	//hLayout->setMargin(0);
	hLayout->setSpacing(1);
	QListWidget* list = new QListWidget;

	hLayout->addWidget(list, 0);

	QStackedWidget* stack = new QStackedWidget;

	hLayout->addWidget(stack, 1);

	connect(list, SIGNAL(currentRowChanged(int)), stack, SLOT(setCurrentIndex(int)));

	loadSettings(list, stack);

	setLayout(hLayout);
}

void SettingsDialog::save(int i)
{
	save(sender()->objectName(), i);
}

void SettingsDialog::save(double d)
{
	save(sender()->objectName(), d);
}

void SettingsDialog::save(const QString& s)
{
	save(sender()->objectName(), s);
}

void SettingsDialog::save(const QString& name, const QVariant& v)
{
	qDebug(
			"Saving value %s = %s",
			qPrintable(name),
			qPrintable(v.toString())
		);
	settings.setValue(name, v);
}

void SettingsDialog::loadSettings(QListWidget* list, QStackedWidget* stack)
{
	const QList<MetaBlock*> metaBlocks = KoreEngine::MetaBlocks();

	for(kint i = 0; i < metaBlocks.size(); i++)
	{
		MetaBlock* mb = metaBlocks.at(i);
		QVariant variant = mb->blockProperty(Block::BlockSettings);
		if(!variant.isValid())
		{
			continue;
		}

		list->addItem(mb->blockProperty(Block::BlockTypeName).toString());

		QVariantMap settings = variant.toMap();
		QStringList settingsKeys = settings.keys();

		QFormLayout* layout = new QFormLayout;

		for(kint i = 0; i < settingsKeys.size(); i++)
		{
			QString settingName = settingsKeys.at(i);
			QVariantMap setting = settings.value(settingName).toMap();

			QString settingText = setting.value(K_BLOCK_SETTING_TEXT).toString();

			QWidget* settingWidget;
			QVariant defaultValue = setting.value(K_BLOCK_SETTING_DEFAULT);

			switch(defaultValue.type())
			{
			case QVariant::UInt:
				{
					QSpinBox* spinBox = new QSpinBox;
					spinBox->setValue(mb->blockSetting(settingName, defaultValue).toUInt());
					spinBox->setMinimum(setting.value(K_BLOCK_SETTING_MINIMUM, 0).toInt());
					spinBox->setMaximum(setting.value(K_BLOCK_SETTING_MAXIMUM, UINT_MAX).toInt());
					spinBox->setSuffix(setting.value(K_BLOCK_SETTING_SUFFIX, QString()).toString());
					connect(spinBox, SIGNAL(valueChanged(int)), SLOT(save(int)));
					settingWidget = spinBox;
				}
				break;
			case QVariant::Int:
				{
					QSpinBox* spinBox = new QSpinBox;
					spinBox->setMinimum(setting.value(K_BLOCK_SETTING_MINIMUM, INT_MIN).toInt());
					spinBox->setMaximum(setting.value(K_BLOCK_SETTING_MAXIMUM, INT_MAX).toInt());
					spinBox->setSuffix(setting.value(K_BLOCK_SETTING_SUFFIX, QString()).toString());
					spinBox->setValue(mb->blockSetting(settingName, defaultValue).toInt());
					connect(spinBox, SIGNAL(valueChanged(int)), SLOT(save(int)));
					settingWidget = spinBox;
				}
				break;
			case QVariant::Double:
				{
					QDoubleSpinBox* spinBox = new QDoubleSpinBox;
					spinBox->setMinimum(setting.value(K_BLOCK_SETTING_MINIMUM, INT_MIN).toDouble());
					spinBox->setMaximum(setting.value(K_BLOCK_SETTING_MAXIMUM, INT_MAX).toDouble());
					spinBox->setSuffix(setting.value(K_BLOCK_SETTING_SUFFIX, QString()).toString());
					spinBox->setValue(mb->blockSetting(settingName, defaultValue).toDouble());
					connect(spinBox, SIGNAL(valueChanged(double)), SLOT(save(double)));
					settingWidget = spinBox;
				}
				break;
			case QVariant::String:
				settingWidget = new QLineEdit(mb->blockSetting(settingName, defaultValue).toString());
				connect(settingWidget, SIGNAL(textEdited(const QString&)), SLOT(save(const QString&)));
				break;
			default:
				settingWidget = new QWidget;
				qDebug("No setting widget for type %s", defaultValue.typeName());
				break;
			}

			// Store the setting name for save operation !
			settingWidget->setObjectName(QString("%1.%2").arg(mb->blockClassName()).arg(settingName));

			layout->addRow(
					settingText.isNull()
						? settingName
						: settingText,
					settingWidget
				);
		}

		QWidget* settingsWidget = new QWidget;
		settingsWidget->setLayout(layout);
		stack->addWidget(settingsWidget);
	}
}
