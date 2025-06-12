/*
 *
 * ©K. D. Hedger. Mon  9 Jun 12:33:02 BST 2025 keithdhedger@gmail.com

 * This file (compiler.h) is part of BashCompiler.

 * BashCompiler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * BashCompiler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with BashCompiler.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _COMPILER_
#define _COMPILER_

#include "globals.h"
//class parseFileClass;
#include "parseFile.h"

class compilerClass
{
	public:
		compilerClass(int argc,char **argv);
		~compilerClass();


		QFile			mainBashFile;
		QVector<QString>	cCode;
		QVector<QString>	fCode;
		QVector<QString>	functionNames;

		bool				verboseCompile=false;
		bool				verboseCCode=false;
		bool				noCodeOP=false;
		bool				isInFunction=false;
		int				argc=0;
		char				**argv=NULL;

		bool				openBashFile(QString filepath);
		void				parseFile(void);
		void				writeCFile(void);

	private:
		QStringList		splitLines(QString qline);

		//parseFileClass	nparser();
		//parseFileClass	*pfl=new parseFileClass("apath");
		//commandsClass	commands;
		int				currentLine=0;
		QString			rawLine;
};

#endif
