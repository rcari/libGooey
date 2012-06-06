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

#include <GooeyApplication.hpp>
#include <GooeyEngine.hpp>
using namespace Gooey;

#include <KoreApplication.hpp>
using namespace Kore;
using namespace Kore::data;

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtGui/QApplication>

GooeyApplication::GooeyApplication(kint argc, kchar** argv)
{
	K_ASSERT( KoreApplication::Instance() ) // KoreApplication must be instanciated first.

	Library* appLib = new Library(Block::System);
	appLib->blockName("Gooey Internals");

	// Apply Gooey stylesheet
	QFile file(":/gooey/text/stylesheet.css");
	file.open(QIODevice::ReadOnly);
	QTextStream stream(&file);
	qApp->setStyleSheet(stream.readAll());
	file.close();

	appLib->addBlock(GooeyEngine::Instance());

	const_cast<Library*>(kApp->rootLibrary())->insertBlock(appLib, kApp->dataLibrary()->index());
}

QString GooeyApplication::Version()
{
	return QString(_GOOEY_VERSION);
}
