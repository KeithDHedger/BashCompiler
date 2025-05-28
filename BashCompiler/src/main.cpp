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
QString			bashOptsAtStart="";//TODO//for later
QVector<QString>	cCode;

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

	mainParseClass->verboseCompile=prefs.LFSTK_getBool("verbose-compile");
	mainParseClass->verboseCCode=prefs.LFSTK_getBool("verbose-ccode");
	mainParseClass->parseFile();

	mainParseClass->cFileDeclares.removeDuplicates();

	//QString specialvars="QString exitstatus;\nQString shelloptions("+bashOptsAtStart+");\n";
	QString specialvars="QString exitstatus;\nQMap<QString> variables\n";
	QString globalvars="QTextStream	outop(stdout);\n\n";
	QString headers="#include <QCoreApplication>\n#include <QTextStream>\n#include <QMap>\n\n";
	QString functions="\n\
QString procsub(QString proc)\n\
{\n\
FILE		*fp;\n\
char		*buffer=(char*)alloca(1024);\n\
QString	retstr=\"\";\n\
\n\
fp=popen(proc.toStdString().c_str(),\"r\");\n\
if(fp!=NULL)\n\
{\n\
buffer[0]=0;\n\
while(fgets(buffer,1024,fp))\n\
retstr+=buffer;\n\
retstr.resize(retstr.length()-1);\n\
exitstatus=QString::number(pclose(fp));\n\
}\n\
return(retstr);\n\
};\n\n";

//write code
	cCode.prepend("QCoreApplication myapp(argc,argv);\n");
	cCode.prepend("int main(int argc, char **argv)\n{\n");
	cCode.prepend(functions);
	cCode.prepend(QString("%1\n").arg(mainParseClass->cFileDeclares.join("\n")));
	cCode.prepend(globalvars);
	cCode.prepend(specialvars);
	cCode.prepend(headers);
	cCode.prepend(QString("/*\nQt C++ file for %1\nCompile with:\ng++ -Wall $(pkg-config --cflags --libs Qt5Core ) -fPIC /PATH/TO/THIS/FILE\nCreated on %2\n*/\n").arg(argv[1]).arg(QDate::currentDate().toString()));
	cCode<<"\nreturn(0);\n}\n";

	for(int j=0;j<cCode.size();j++)
		QTextStream(stdout)<<cCode.at(j);

	delete mainParseClass;
	delete mainCommandsClass;

	return(0);
}