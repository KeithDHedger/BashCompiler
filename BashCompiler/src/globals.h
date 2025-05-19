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
#include <QTextStream>

class parseFileClass;
class commandsClass;

enum commandName {EXTERNALCOMMAND=0,BASHECHO,BASHIF,BASHTHEN,BASHELSE,BASHFI,BASHWHILE,BASHDO,BASHDONE};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static const char*	bashmath[]={"-gt","-lt","-eq","-ne","-le","-ge","=","!=","<",">",">=","<=",NULL};
static const char*	cmath[]={">","<","==","!=","<-",">=","==","!=","<",">",">=","<=",NULL};
static QTextStream	errop(stderr);
static QTextStream	outop(stdout);
#pragma GCC diagnostic pop

#include "commands.h"
#include "parseFile.h"

extern parseFileClass	*mainParseClass;
extern commandsClass		*mainCommandsClass;
extern QString			bashOptsAtStart;
extern QVector<QString>	cCode;

#endif