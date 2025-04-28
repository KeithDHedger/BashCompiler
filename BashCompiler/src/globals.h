/*
 *
 * ©K. D. Hedger. Sat 19 Apr 14:08:13 BST 2025 keithdhedger@gmail.com

 * This file (globals.h) is part of BashCompiler.

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

#ifndef _GLOBALS_
#define _GLOBALS_

#include <QApplication>
#include <QFile>
#include <QRegularExpression>
#include <QVector>

class parseFileClass;
class commandsClass;


enum dataType {INVALID=0,CONSTSTR,STRVAR,PROCSUB,NEGATE,CONSTDATA};
//static const char *typeAsText[]={"INVALID","CONSTSTR","STRVAR","PROCSUB","NEGATE","CONSTDATA"};

struct lineData
{
	QString		data="";
	dataType		dtype=INVALID;
	qsizetype	argLen=0;
};

#include "commands.h"
#include "parseFile.h"

extern parseFileClass	*mainParseClass;
extern commandsClass		*mainCommandsClass;

#endif