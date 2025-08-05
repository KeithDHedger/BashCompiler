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

//external command
		QString	makeExternalCommand(QString qline);

//assign
		QString	makeAssign(QString qline);

//printf/echo/for
		QString	makePrintf(QString qline);
		QString	makeEcho(QString qline);
		QString	makeFor(QString qline);

//if/then/else/fi
		QString	makeIf(QString qline);

//while/do/done
		QString	makeWhile(QString line);
		QString	makeDone(QString qline);

//while read	
		QString	makeWhileRead(QString qline);

//builtins
		QString	makePushd(QString qline);
		QString	makePopd(QString qline);
		QString	makeExit(QString qline);
		QString	makeExport(QString qline);
		QString	makeCD(QString qline);
		QString	makeRead(QString qline);
		QString	makeCase(QString qline);
		QString	makeCaseCompareStatement(QString qline);

//functions
		QString	makeFunction(QString qline);
		QString	makeFunctionDefine(QString qline);

//here docs
		QString	makeHereDoc(QString qline);
	private:
		QString	optEchoLine(QString qline,bool preserve,bool escapes,bool force);
};

#endif
