/*
 *
 * ©K. D. Hedger. Mon  9 Jun 12:33:02 BST 2025 keithdhedger@gmail.com

 * This file (compiler.cpp) is part of BashCompiler.

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

#include "compiler.h"

compilerClass::~compilerClass()
{
	if(this->mainBashFile.isOpen()==true)
		this->mainBashFile.close();
}

compilerClass::compilerClass(int argc,char **argv)
{
	this->argc=argc;
	this->argv=argv;
}

bool compilerClass::openBashFile(QString filepath)
{
	if(filepath.isEmpty()==false)
		{
			this->mainBashFile.setFileName(filepath);

			if(!this->mainBashFile.open(QIODevice::ReadOnly | QIODevice::Text))
				return(false);
		}
	return(true);
}

void compilerClass::writeCFile(void)
{
	QString specialvars="QString exitstatus;\n";
	QString globalvars="QTextStream outop(stdout);\nQHash<QString,QString> variables;\nQVector<QString> dirstack;\nchar **gargv;\n";
	QString headers="#include <QTextStream>\n#include <QHash>\n#include <QRegularExpression>\n#include <QDir>\n\n";
	QString functions="\n\
QString procsub(QString proc)\n\
{\n\
FILE *fp;\n\
int exitnum=-1;\n\
char *buffer=(char*)alloca(1024);\n\
QString retstr=\"\";\n\
\n\
fp=popen(proc.toStdString().c_str(),\"r\");\n\
if(fp!=NULL)\n\
{\n\
buffer[0]=0;\n\
while(fgets(buffer,1024,fp))\n\
retstr+=buffer;\n\
retstr.resize(retstr.length()-1);\n\
exitnum=pclose(fp)/256;\n\
exitstatus=QString::number(exitnum);\n\
}\n\
return(retstr);\n\
};\n\n";

	for(int j=0;j<this->fCode.size();j++)
		functions+=this->fCode.at(j);

//write code
	this->cCode.prepend("int main(int argc, char **argv)\n{\ngargv=argv;\n");
	this->cCode.prepend(functions);
	this->cCode.prepend(globalvars);
	this->cCode.prepend(specialvars);
	this->cCode.prepend(headers);
	this->cCode.prepend(QString("/*\nQt C++ file for %1\nCompile with:\ng++ -Wall $(pkg-config --cflags --libs Qt5Core ) -fPIC  -Ofast /PATH/TO/THIS/FILE\nOptional:\nastyle -A7 --indent=tab /PATH/TO/THIS/FILE\nstrip ./a.out\nCreated on %2\n*/\n").arg(this->argv[1]).arg(QDate::currentDate().toString()));
	cCode<<"\nreturn(0);\n}\n";

	for(int j=0;j<this->cCode.size();j++)
		QTextStream(stdout)<<this->cCode.at(j);
}

void compilerClass::parseFile(void)
{
	QString					line;
	QStringList				lines;
	QString					retstr;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	QString					lineend;

	commandsClass			commands;
	while (!this->mainBashFile.atEnd())
		{
			this->rawLine=this->mainBashFile.readLine().trimmed();
			currentLine++;
			line=this->rawLine;

			if(line.length()==0)
				continue;
			if(this->verboseCompile==true)
				errop<<"processing line "<<this->currentLine<<" "<<line<<Qt::endl;

			if((this->verboseCCode==true) && (line.length()>0))
				{
					if(this->isInFunction==true)
						fCode<<"//"+line+"\n";
					else
						cCode<<"//"+line+"\n";
				}

			if(line.startsWith('#'))
				continue;

			lines=this->splitLines(this->rawLine);
			for(int j=0;j<lines.count();j++)
				{
					lineend=";\n";
					retstr="";
					re.setPattern("^[[:space:]]*([[:alpha:][:alnum:]_]*)[[:space:]]*(=)?");
					match=re.match(lines.at(j));
					if(match.hasMatch())
						{
							if(match.captured(2).trimmed()=="=")
								{
									retstr=commands.makeAssign(lines.at(j));
									if(retstr.isEmpty()==false)
										{
											retstr="variables[\""+match.captured(1).trimmed()+"\"]="+retstr;
										}
								}
							else
								{
									if(match.captured(1).trimmed()=="printf")
										retstr=commands.makePrintfNew(lines.at(j));
									if(match.captured(1).trimmed()=="echo")
										retstr=commands.makeEcho(lines.at(j));
									if(match.captured(1).trimmed()=="if")
										{
											lineend="";
											retstr=commands.makeIfNew(lines.at(j));
										}
									if(match.captured(1).trimmed()=="then")
										{
											lineend="";
											retstr="{\n";
										}
									if(match.captured(1).trimmed()=="else")
										{
											lineend="";
											retstr="}\nelse\n{\n";
										}
									if(match.captured(1).trimmed()=="fi")
										{
											lineend="";
											retstr="}\n";
										}
								}

							if(retstr.isEmpty()==false)
								{
									if(isInFunction==true)
										this->fCode<<retstr<<lineend;
									else
										this->cCode<<retstr<<lineend;
								}
						}
				}
		}
}

QStringList compilerClass::splitLines(QString qline)
{
	QStringList	lines;
	QString		tstr;
	QStringList	keywords;
	QStringList	ss;
	bool			inquotes;
	bool			quotes;
	int			inquotespos;
	int			pos=0;

	lines.clear();
	tstr=qline;
	inquotes=false;
	inquotespos=0;
	keywords<<"then "<<"else "<<"do "<<"#";
	//keywords<<"then ";

	while(pos<tstr.length())
		{
			if(tstr.at(pos).toLatin1()=='\'')
				{
					pos++;
					inquotes=true;
					inquotespos=pos;
					while((inquotespos<tstr.length()) && (inquotes==true))
						{
							if(tstr.at(inquotespos).toLatin1()=='\\')
								{
									inquotespos+=2;
									continue;
								}
							if(tstr.at(inquotespos).toLatin1()=='\'')
								inquotes=false;
							inquotespos++;
						}
					pos=inquotespos;
				}
			if(tstr.at(pos).toLatin1()=='"')
				{
					pos++;
					inquotes=true;
					inquotespos=pos;
					while((inquotespos<tstr.length()) && (inquotes==true))
						{
							if(tstr.at(inquotespos).toLatin1()=='\\')
								{
									inquotespos+=2;
									continue;
								}
							if(tstr.at(inquotespos).toLatin1()=='"')
								inquotes=false;
							inquotespos++;
						}
					pos=inquotespos;
				}
			if(pos>=tstr.length())
				continue;

			if(tstr.at(pos).toLatin1()==';')
				{
					lines<<tstr.left(pos);
					tstr=tstr.mid(pos+1);
					pos=0;
					continue;
				}

			for(int k=0;k<keywords.count();k++)
				{
					if(keywords.at(k)==tstr.trimmed().mid(pos,keywords.at(k).length()))
						{
							lines<<tstr.left(pos);
							lines<<keywords.at(k);
							tstr=tstr.mid(pos+keywords.at(k).length());
							pos=0;
							continue;
						}	
				}

			if(tstr.at(pos).toLatin1()=='\\')
				{
					pos+=2;
					continue;
				}

			pos++;
		}
	lines<<tstr;
//for(int j=0;j<lines.count();j++)
//	qDebug()<<lines.at(j);
//exit(100);
	return(lines);
}