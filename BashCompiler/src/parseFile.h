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
		parseFileClass(QString filepath="");
		~parseFileClass();

//control
		bool					verboseCompile=false;
		bool					verboseCCode=false;
		bool					noCodeOP=false;
	
		QStringList			cFileDeclares;
		QVector<lineData>	lineParts;
		commandName			bashCommand=EXTERNALCOMMAND;
		QString				currentPart="";
		int					linePosition=0;

		void					parseFile(void);
		bool					parseLine(QString line);
		QString				parseExprString(bool isnumexpr);
		QString				lineToBashCLIString(QString qline);

	private:
		QFile				mainBashFile;
		bool					preserveWhitespace=false;
		int					currentLine=0;

		QString				setSpecialDollars(QChar dollar);
		//QVector<lineData>	parseString(QString qline,int *linePosition,QString currentPart);
		void 				parseString(QString qline);
		QString				parseVar(QString line);
		QString				cleanVar(QString line);
		void					createCommand(QString line);
		void					parseSquareBraces(QString line);
		void					parseDollar(QString line);

		void					parseWhitespace(QString line);
		//void					parseWhitespace(QString line,QString currentPart,int *linePosition,QVector<lineData> *lineParts);
		//void					parseQuotedString(QString line,QString currentPart,int *linePosition,QVector<lineData> *lineParts);

		void					parseQuotedString(QString line);
};

#endif
