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

int main(int argc,char **argv)
{
	QApplication	app(argc,argv);

	mainParseClass=new parseFileClass(argv[1]);
	mainCommandsClass=new commandsClass();

	mainParseClass->verboseCompile=true;
	mainParseClass->verboseCCode=false;
	mainParseClass->parseFile();

	mainParseClass->cFileDeclares.removeDuplicates();

	//QString specialvars="QString exitstatus;\nQString shelloptions("+bashOptsAtStart+");\n";
	QString specialvars="QString exitstatus;\n";
	QString headers="#include <QTextStream>\n#include <QCoreApplication>\n#include <QDebug>\n\n";
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

QTextStream(stdout)<<"//C file for "<<argv[1]<<"\n\n"<<headers<<specialvars<<functions<<mainParseClass->cFileDeclares.join("\n")<<"\n"<<"int main(int argc, char **argv)\n{\n"<<"QCoreApplication myapp(argc,argv);\n";
QTextStream(stdout)<<mainParseClass->cFile<<"\n";
QTextStream(stdout)<<"return(0);\n}\n\n";


	delete mainParseClass;
	delete mainCommandsClass;
	return(0);
}