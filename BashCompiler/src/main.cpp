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

int main(int argc,char **argv)
{
	QApplication	app(argc,argv);

	mainParseClass=new parseFileClass(argv[1]);
	mainCommandsClass=new commandsClass();

	mainParseClass->verboseCompile=true;
	mainParseClass->verboseCCode=false;
	mainParseClass->parseFile();

	mainParseClass->cFileDeclares.removeDuplicates();

	QString functions="std::string procsub(std::string proc)\n\
{\n\
FILE *fp;\n\
char *buffer=(char*)alloca(1024);\n\
std::string retstr=\"\";\n\
fp=popen(proc.c_str(),\"r\");\n\
if(fp!=NULL)\n\
{\n\
buffer[0]=0;\n\
while(fgets(buffer,1024,fp))\n\
retstr+=buffer;\n\
retstr.erase(retstr.length()-1,1);\n\
pclose(fp);\n\
}\n\
return(retstr);\n\
}\n";

	printf("//C File\n#include <string>\n#include <cstdio>\nbool inQuotes=false;\n%s\n%s\nint main(int argc,char **argv)\n{\n%s\n}\n",functions.toStdString().c_str(),mainParseClass->cFileDeclares.join("\n").toStdString().c_str(),mainParseClass->cFile.toStdString().c_str());

	delete mainParseClass;
	delete mainCommandsClass;
	return(0);
}