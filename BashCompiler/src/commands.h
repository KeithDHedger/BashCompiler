/*
 *
 * ©K. D. Hedger. Tue 22 Apr 14:49:51 BST 2025 keithdhedger@gmail.com

 * This file (commands.h) is part of BashCompiler.

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

#ifndef _COMMANDS_
#define _COMMANDS_

#include "globals.h"

class commandsClass
{
	public:
		commandsClass();
		~commandsClass();

		QString	makeEcho(QString line);
		QString	makeIf(QString line);
		QString	makeThen(QString line);
		QString	makeFi(QString line);
		QString	makeExternalCommand(QString line);

	private:
		lineData	parseStr(QString line);
		QString	makeIntCompare(QVector<lineData> da);
		QString	makeStrCompare(QVector<lineData> da);
};

#endif
