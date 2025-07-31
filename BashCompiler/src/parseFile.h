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
		parseFileClass();
		~parseFileClass();

//control
		QVector<lineData>	lineParts;
		QString				currentPart="";
		commandName			bashCommand=EXTERNALCOMMAND;
		int					linePosition=0;
		bool					preserveWhitespace=false;

		QString				parseExprString(bool isnumexpr);
		QString				lineToBashCLIString(QString qline);
		QString				parseVar(QString line);
		QString				cleanVar(QString line);
		QString				parseOutputString(QString qline);
		QString				globToRX(QString glob,bool greedy);
		bool					parseLine(QString line);

		QString				optimizeOP(QString qline,bool *succeed);

	private:
		int					currentLine=0;

		QString				setSpecialDollars(QString dollar);
		void 				parseString(QString qline);
		void					parseSquareBraces(QString line);
		void					parseDollar(QString line);
		void					parseWhitespace(QString line);
		void					parseQuotedString(QString line);
};

#endif
