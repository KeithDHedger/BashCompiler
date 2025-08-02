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
	QString headers="#include <QTextStream>\n#include <QHash>\n#include <QRegularExpression>\n#include <QDir>\n#include <QProcessEnvironment>\n\n";
	QString functions="\n\
QString procsub(QString proc)\n\
{\n\
FILE *fp;\n\
int exitnum=-1;\n\
char *buffer=(char*)alloca(1024);\n\
QString retstr=\"\";\n\
\n\
//outop<<proc<<Qt::endl;\n\
\n\
fp=popen(proc.toStdString().c_str(),\"r\");\n\
if(fp!=NULL)\n\
{\n\
buffer[0]=0;\n\
while(fgets(buffer,1024,fp))\n\
retstr+=buffer;\n\
if(retstr.isEmpty()==false)\n\
{\n\
if(retstr.at(retstr.length()-1)=='\\n')\n\
retstr.resize(retstr.length()-1);\n\
}\n\
exitnum=pclose(fp)/256;\n\
exitstatus=QString::number(exitnum);\n\
}\n\
return(retstr);\n\
};\n\n\
QString loadEnvironment(bool capture,QHash<QString, QString> fv)\n\
{\n\
QProcessEnvironment env=QProcessEnvironment::systemEnvironment();\n\
QStringList paths_list=env.toStringList();\n\
for(int j=0;j<paths_list.size();j++)\n\
{\n\
if(paths_list.at(j).startsWith(\"BASH_FUNC_\")==false)\n\
{\n\
QStringList parts=paths_list.at(j).split(\"=\");\n\
variables[parts.at(0)]=parts.at(1);\
}\n\
}\n\
return(\"\");\n\
}\n\
\n";

	for(int j=0;j<fCode.size();j++)
		functions+=fCode.at(j);

//write code
	cCode.prepend("int main(int argc, char **argv)\n{\ngargv=argv;\nloadEnvironment(false,{});\n");
	cCode.prepend(functions);
	cCode.prepend(globalvars);
	cCode.prepend(specialvars);
	cCode.prepend(headers);
	cCode.prepend(QString("/*\nQt C++ file for %1\nCompile with:\ng++ -Wall $(pkg-config --cflags --libs "+useQT+" ) -fPIC  -Ofast /PATH/TO/THIS/FILE -o APPNAME\nOptional:\nastyle -A7 --indent=tab /PATH/TO/THIS/FILE\nstrip ./a.out\nCreated on %2\n*/\n").arg(this->argv[1]).arg(QDate::currentDate().toString()));
	cCode<<"\nreturn(0);\n}\n";

	if(fullCompileHere.isEmpty()==true)
		{
			for(int j=0;j<cCode.size();j++)
				QTextStream(stdout)<<cCode.at(j);
		}
	else
		{
			QFileInfo	fi(fullCompileHere);
			QString		filename=fi.fileName();
			QString		foldername=fi.dir().absolutePath();
			QFile		file(foldername+"/"+filename+".cpp");
			QString		command;

			QDir().mkpath(foldername);
			if(!file.open(QIODevice::WriteOnly))
				{
					qDebug()<<"Failed to open file";
					exit(100);
				}

			QTextStream	out(&file);
			for(int j=0;j<cCode.size();j++)
				out<<cCode.at(j);
			file.close();
			command=QString("g++ -Wall $(pkg-config --cflags --libs "+useQT+" ) -fPIC  -Ofast '%1' -o '%2'").arg(foldername+"/"+filename+".cpp").arg(foldername+"/"+filename);
			errop<<"Compiling using command:\n"<<command<<"\n..."<<Qt::endl;
			system(command.toStdString().c_str());
		}
}

void compilerClass::parseSingleLine(QString qline)
{
	QString					line;
	QStringList				lines;
	QString					retstr;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	QString					lineend;
	commandsClass			commands;
bool doignore=false;

	line=this->rawLine;

	if(line.length()==0)
		return;

	if(this->verboseCompile==true)
		errop<<"Processing line "<<this->currentLine<<" "<<line<<Qt::endl;

	if(line.startsWith('#'))
		return;

	re.setPattern("^.*<<([[:space:]]*[[:alpha:][:alnum:]]*[[:space:]]*)$");
	match=re.match(line);
	if(match.hasMatch())
		{
			retstr=commands.makeHereDoc(line);
			if(retstr.isEmpty()==false)
				{
					if(isInFunction==true)
						fCode<<retstr<<";";
					else
						cCode<<retstr<<";";
				}
			return;
		}
	else
		lines=this->splitLines(this->rawLine);

	for(int j=0;j<lines.count();j++)
		{
			doignore=false;
			if(lines.at(j).trimmed().startsWith('#'))
				return;

			if(lines.at(j).trimmed()=="{")
				{
					if(this->verboseCCode==true)
						{
							if(isInFunction==true)
								fCode<<"//"+QString("Line %0: {\n").arg(currentLine);
							else
								cCode<<"//"+QString("Line %0: {\n").arg(currentLine);
						}
					return;
				}

			if(lines.at(j).trimmed()=="}")
				{
					if((this->verboseCCode==true) && (line.length()>0))
						{
							if(isInFunction==true)
								fCode<<"//"+QString("Line %0: }\n").arg(currentLine);
							else
								cCode<<"//"+QString("Line %0: }\n").arg(currentLine);
						}

					fCode<<"	if(capture==true)\nreturn(retstr.trimmed());\n";
					fCode<<"else\n{\nif(file.isOpen()==true)\nfile.close();}\n\nreturn(0);\n}\n";
					isInFunction=false;
					return;
				}

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
						//builtins
							if(match.captured(1).trimmed()=="printf")
								retstr=commands.makePrintf(lines.at(j));
							if(match.captured(1).trimmed()=="echo")
								retstr=commands.makeEcho(lines.at(j));
							if(match.captured(1).trimmed()=="pushd")
								retstr=commands.makePushd(lines.at(j));
							if(match.captured(1).trimmed()=="popd")
								retstr=commands.makePopd(lines.at(j));
							if(match.captured(1).trimmed()=="exit")
								retstr=commands.makeExit(lines.at(j));
							if(match.captured(1).trimmed()=="export")
								retstr=commands.makeExport(lines.at(j));
							if(match.captured(1).trimmed()=="cd")
								retstr=commands.makeCD(lines.at(j));
							if(match.captured(1).trimmed()=="read")
								retstr=commands.makeRead(lines.at(j));

							if(match.captured(1).trimmed()=="case")
								{
									retstr=commands.makeCase(lines.at(j));
									continue;
								}
							if(match.captured(1).trimmed()=="esac")
								{
									caseVariable.pop_back();
									continue;
								}

							//conditionals
							if(match.captured(1).trimmed()=="for")
								{
									lineend="";
									retstr=commands.makeFor(lines.at(j));
								}

							if(match.captured(1).trimmed()=="if")
								{
									lineend="";
									retstr=commands.makeIf(lines.at(j));
								}
							if(match.captured(1).trimmed()=="then")
								{
									doignore=true;
									lineend="";
									retstr="{\n";
								}
							if(match.captured(1).trimmed()=="else") //TODO//elif
								{
									doignore=true;
									lineend="";
									retstr="}\nelse\n{\n";
								}
							if(match.captured(1).trimmed()=="fi")
								{
									doignore=true;
									lineend="";
									retstr="}\n";
								}
							if(match.captured(1).trimmed()=="while")
								{
									lineend="";
									retstr=commands.makeWhileRead(lines.at(j));
									if(retstr.isEmpty()==true)
										retstr=commands.makeWhile(lines.at(j));
								}
							if(match.captured(1).trimmed()=="do")
								{
									doignore=true;
									lineend="";
									if((isInFor.isEmpty()==false) && (isInFor.back()==true))
										retstr="{\nvariables[\""+forVariable.back()+"\"].setNum("+forVariable.back()+");\n";
									else
										retstr="{\n";
								}
							if(match.captured(1).trimmed()=="done")
								{
									doignore=true;
									lineend="";
									retstr=commands.makeDone(lines.at(j));
									if(retstr.isEmpty()==true)
										retstr="}\n";
								}

							//functions/external commands	 etc
							if(retstr.isEmpty()==true)
								{
									if(retstr.isEmpty()==true)
										retstr=commands.makeFunction(lines.at(j));

									if(retstr.isEmpty()==true)
										{
											retstr=commands.makeCaseCompareStatement(lines.at(j));
											if(retstr.isEmpty()==false)
												{
													lineend="";
												}
										}
									if(retstr.isEmpty()==true)
										retstr=commands.makeExternalCommand(lines.at(j));
								}
						}

					if(retstr.isEmpty()==false)
						{
							if((this->verboseCCode==true) && (line.length()>0) && (doignore==false))
								{
									if(isInFunction==true)
										//fCode<<"//"+QString("Line %0: ").arg(currentLine)+line+"\n";
										fCode<<"//"+QString("Line %0: ").arg(currentLine)+lines.at(j)+"\n";
									else
										//cCode<<"//"+QString("Line %0: ").arg(currentLine)+line+"\n";
										cCode<<"//"+QString("Line %0: ").arg(currentLine)+lines.at(j)+"\n";
								}
										
							if(isInFunction==true)
								fCode<<retstr<<lineend;
							else
								cCode<<retstr<<lineend;
						}
				}
		}
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
	functionNames<<"loadEnvironment";

	while(!this->mainBashFile.atEnd())
		{
			this->currentLine++;
			this->rawLine=this->mainBashFile.readLine().trimmed();
			this->parseSingleLine(this->rawLine);
		}
}

QStringList compilerClass::splitLines(QString qline)
{
	QStringList	lines;
	QString		tstr;
	QStringList	keywords;
	QStringList	ss;
	bool			inquotes;
	int			inquotespos;
	int			pos=0;

	lines.clear();
	tstr=qline;
	inquotes=false;
	inquotespos=0;
	keywords<<"then "<<"else "<<"do ";

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

			if((pos<tstr.length()-1) && (tstr.at(pos).toLatin1()=='(') && (tstr.at(pos+1).toLatin1()=='('))
				{
					pos+=2;
					inquotes=true;
					inquotespos=pos;
					while((inquotespos<tstr.length()) && (inquotes==true))
						{
							if((tstr.at(inquotespos).toLatin1()==')') && (tstr.at(inquotespos+1).toLatin1()==')'))
								inquotes=false;
							inquotespos++;
						}
					pos=inquotespos+1;
				}


			if(pos>=tstr.length())
				continue;

			if(pos+1<tstr.length() && tstr.at(pos).toLatin1()==';' && tstr.at(pos+1).toLatin1()==';')
				{
					pos+=2;
					continue;
				}

			if(tstr.at(pos).toLatin1()==';')
				{
					lines<<tstr.left(pos);
					tstr=tstr.mid(pos+1);
					pos=0;
					continue;
				}

			for(int k=0;k<keywords.count();k++)
				{
					//if(keywords.at(k)==tstr.trimmed().mid(pos,keywords.at(k).length()))
					if(tstr.left(keywords.at(k).length())==keywords.at(k))
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
	return(lines);
}