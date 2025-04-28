/*
 *
 * ©K. D. Hedger. Sat 19 Apr 14:07:25 BST 2025 keithdhedger@gmail.com

 * This file (parseFile.h) is part of BashCompiler.

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

#ifndef _PARSEFILE_
#define _PARSEFILE_

#include "globals.h"

class parseFileClass
{
	public:
		parseFileClass(QString filepath);
		~parseFileClass();

//control
		bool					verboseCompile=false;
		bool					verboseCCode=false;
		void					parseFile(void);

		QString				cFile;
		QStringList			cFileDeclares;
		QVector<lineData>	dataArray;
		QStringList			lineParts;

		void					parseLineAsParams(QString line);
	private:
		QFile				mainBashFile;

		bool					parseLine(QString line);
		QString				createCommand(QString line);
		QString				dataArrayToString(void);
};

#endif
