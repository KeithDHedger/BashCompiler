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
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>
#include <QVector>
#include <QTextStream>
#include <QDate>
#include <QProcess>
#include <QDebug>

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <iostream>

#include "config.h"
#include "LFSTKPrefsClass.h"

class parseFileClass;
class commandsClass;
class compilerClass;

enum commandName {EXTERNALCOMMAND=0,BASHASSIGN,BASHECHO,BASHIF,BASHTHEN,BASHELSE,BASHFI,BASHWHILE,BASHWHILEREAD,BASHDO,BASHDONE,BASHFUNCTION,BASHFUNCTIONCALL,BASHBUILTIN,SKIPLINE};
enum parseDataType {UNKNOWN=0,WHITESPACE,DOUBLEQUOTESTRING,SINGLEQUOTSTRING,COMMAND,VARIABLE,VARIABLEINCURLYS,BRACKETS,SQUAREBRACKETS,STRINGDATA,VARNAME};

struct lineData
{
	QString		data;
	parseDataType		typeHint;
};


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

static const char	*NORMAL="\e[0m";
static const char	*RED="\e[1;31m";
static const char	*GREEN="\e[1;32m";
static const char	*CYAN="\e[1;36m";
static const char	*BLUE="\e[1;34m";
static const char* typeText[]={"UNKNOWN","WHITESPACE","DOUBLEQUOTESTRING","SINGLEQUOTSTRING","COMMAND","VARIABLE","VARIABLEINCURLYS","BRACKETS","SQUAREBRACKETS","STRINGDATA","VARNAME","SKIPLINE"};
static const char*	bashmath[]={"-gt ","-lt ","-eq ","-ne ","-le ","-ge "," != "," < "," > "," = ",NULL};
static const char*	cmath[]={">","<","==","!=","<=",">=","!=","<",">","==",NULL};
static QTextStream	errop(stderr);
static QTextStream	outop(stdout);
static QRegularExpression replaceWhiteSpace("[[:space:]]+");

#pragma GCC diagnostic pop

#include "commands.h"
#include "parseFile.h"
#include "compiler.h"

extern commandsClass		*mainCommandsClass;
extern compilerClass		*mainCompilerClass;

extern QString			bashOptsAtStart;
extern QVector<QString>	functionNames;
extern bool				isInFunction;

extern QVector<int>		whileReadLine;
extern QVector<QString>	cCode;
extern QVector<QString>	fCode;
extern QVector<QString>	forVariable;
extern QVector<bool>		isInFor;
extern QVector<QString>	caseVariable;
extern bool				firstCasecompare;
extern QString			fullCompileHere;
extern QString			compileHere;
extern QString			useQT;
extern QString			prettyCommand;

#endif