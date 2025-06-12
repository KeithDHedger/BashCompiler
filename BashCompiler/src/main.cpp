/*
 *
 * ©K. D. Hedger. Fri 18 Apr 20:31:32 BST 2025 keithdhedger@gmail.com

 * This file (main.cpp) is part of BashCompiler.

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

#include "globals.h"

parseFileClass	*mainParseClass=NULL;
commandsClass	*mainCommandsClass=NULL;
compilerClass	*mainCompilerClass=NULL;

QString			bashOptsAtStart="";//TODO//for later
QVector<QString>	cCode;
QVector<QString>	fCode;
QVector<QString>	functionNames;
bool				isInFunction;

int main(int argc,char **argv)
{
	QApplication	app(argc,argv);

	LFSTK_prefsClass	prefs(PACKAGE,VERSION);
	option			long_options[]=
		{
			{"verbose-compile",optional_argument,NULL,'v'},
			{"verbose-ccode",optional_argument,NULL,'V'},
			{0,0,0,0}
		};

	std::string	configfile=getenv("HOME");
	configfile+="/.config/bashcompiler.rc";

	prefs.prefsMap=
		{
			{prefs.LFSTK_hashFromKey("verbose-compile"),{TYPEBOOL,"verbose-compile","Verbose compile ( optional 1/true or 0/false )","",false,0}},
			{prefs.LFSTK_hashFromKey("verbose-ccode"),{TYPEBOOL,"verbose-ccode","Add BASH source lines to C Code ( optional 1/true or 0/false )","",false,0}},
		};
	prefs.LFSTK_loadVarsFromFile(configfile);

	if(prefs.LFSTK_argsToPrefs(argc,argv,long_options,true)==false)
		return(0);

	if(prefs.cliArgs.size()==0)
		{
			qDebug()<<"No input file supplied ...";
			exit(1);
		}
	mainParseClass=new parseFileClass(prefs.cliArgs.at(0).c_str());
	mainCommandsClass=new commandsClass();
	mainCompilerClass=new compilerClass(argc,argv);



	mainParseClass->verboseCompile=prefs.LFSTK_getBool("verbose-compile");
	mainParseClass->verboseCCode=prefs.LFSTK_getBool("verbose-ccode");

	if(mainCompilerClass->openBashFile(prefs.cliArgs.at(0).c_str())==true)
		{
			mainCompilerClass->verboseCompile=prefs.LFSTK_getBool("verbose-compile");
			mainCompilerClass->verboseCCode=prefs.LFSTK_getBool("verbose-ccode");
			mainCompilerClass->parseFile();

	//mainParseClass->parseFile();

			//mainCompilerClass->cCode=cCode;
			//mainCompilerClass->fCode=fCode;
			mainCompilerClass->functionNames=functionNames;
			mainCompilerClass->writeCFile();
		}
	else
		errop<<"Can't open input file, aborting ...!"<<Qt::endl;
	
	delete mainParseClass;
	delete mainCommandsClass;
	delete mainCompilerClass;

	return(0);
}