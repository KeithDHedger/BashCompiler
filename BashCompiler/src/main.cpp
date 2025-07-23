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

commandsClass	*mainCommandsClass=NULL;
compilerClass	*mainCompilerClass=NULL;

QString			bashOptsAtStart="";//TODO//for later
QVector<QString>	functionNames;
bool				isInFunction;
QVector<int>		whileReadLine;
QVector<QString>	cCode;
QVector<QString>	fCode;
QVector<QString>	forVariable;
QVector<bool>	isInFor;
QVector<QString>	caseVariable;
QString			fullCompileHere="";
QString			useQT="Qt6Core";
bool				firstCasecompare=false;

int main(int argc,char **argv)
{
	QApplication	app(argc,argv);

	LFSTK_prefsClass	prefs(PACKAGE,VERSION);
	option			long_options[]=
		{
			{"verbose-compile",optional_argument,NULL,'v'},
			{"verbose-ccode",optional_argument,NULL,'V'},
			{"full-compile",required_argument,NULL,'c'},
			{"use-qt5",no_argument,NULL,'5'},
			{"syntax-check",optional_argument,NULL,'s'},
			{0,0,0,0}
		};

	std::string	configfile=getenv("HOME");
	configfile+="/.config/bashcompiler.rc";

	prefs.prefsMap=
		{
			{prefs.LFSTK_hashFromKey("verbose-compile"),{TYPEBOOL,"verbose-compile","Verbose compile ( optional 1/true or 0/false )","",true,0}},
			{prefs.LFSTK_hashFromKey("verbose-ccode"),{TYPEBOOL,"verbose-ccode","Add BASH source lines to C Code ( optional 1/true or 0/false )","",false,0}},
			{prefs.LFSTK_hashFromKey("full-compile"),{TYPESTRING,"full-compile","Compile script then compile code to ARG ( where ARG is fullpath to final application )","",false,0}},
			{prefs.LFSTK_hashFromKey("use-qt5"),{TYPEBOOL,"use-qt5","Use qt5 for final compile instead of qt6","",false,0}},
			{prefs.LFSTK_hashFromKey("syntax-check"),{TYPEBOOL,"syntax-check","Just Check syntax ( use shellcheck if installed  else use bash -n )","",false,0}},
		};
	prefs.LFSTK_loadVarsFromFile(configfile);

	if(prefs.LFSTK_argsToPrefs(argc,argv,long_options,true)==false)
		return(0);

	if(prefs.cliArgs.size()==0)
		{
			qDebug()<<"No input file supplied ...";
			exit(1);
		}

	if(prefs.LFSTK_getBool("syntax-check")==true)
		{
			QString checkstr=QString("shellcheck \"%1\" 2>/dev/null||bash -n \"%1\"").arg(prefs.cliArgs.at(0).c_str());
			int exitstatus=WEXITSTATUS(system(checkstr.toStdString().c_str()));
			qDebug()<<"Error "<<exitstatus;
			exit(exitstatus);
		}

	if(prefs.LFSTK_getString("full-compile").length()>0)
		fullCompileHere=prefs.LFSTK_getString("full-compile").c_str();

	if(prefs.LFSTK_getBool("use-qt5")==true)
		useQT="Qt5Core";

	mainCommandsClass=new commandsClass();
	mainCompilerClass=new compilerClass(argc,argv);

	if(mainCompilerClass->openBashFile(prefs.cliArgs.at(0).c_str())==true)
		{
			mainCompilerClass->verboseCompile=prefs.LFSTK_getBool("verbose-compile");
			mainCompilerClass->verboseCCode=prefs.LFSTK_getBool("verbose-ccode");
			mainCompilerClass->parseFile();
			mainCompilerClass->writeCFile();
		}
	else
		errop<<"Can't open input file, aborting ...!"<<Qt::endl;
	
	delete mainCommandsClass;
	delete mainCompilerClass;

	return(0);
}