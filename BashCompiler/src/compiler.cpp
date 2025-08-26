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

	QString globalvars="QTextStream outop(stdout);\nQHash<QString,QString> variables;\nQVector<QString> dirstack;\nQRegularExpression replaceWhite(\"[[:space:]]+\");\nQProcess pipeProc;\nQProcess comProc;\n\nint exitnum=-1;\nchar **gargv;\nwordexp_t commandsvar[MAXARGS];\nchar buffer[BUFFER_SIZE];\n";

	QString headers="#include <QTextStream>\n#include <QHash>\n#include <QRegularExpression>\n#include <QDir>\n#include <QProcessEnvironment>\n#include <QProcess>\n#include <wordexp.h>\n#include <sys/wait.h>\n#include <unistd.h>\n#include <fcntl.h>\n\n#define MAXARGS 50\n#define BUFFER_SIZE 4096\n\n";

	QString functions="\n\
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
\n\
QString procSubCheat(QString proc)\n\
{\n\
FILE *fp;\n\
int exitnum=-1;\n\
char *buffer=(char*)alloca(1024);\n\
QString retstr=\"\";\n\
\n\
//outop<<proc<<Qt::endl;\n\
\n\
retstr=variables[\"SHELL\"]+\" -c '\"+proc+\"'\";\n\
fp=popen(retstr.toStdString().c_str(),\"r\");\n\
retstr=\"\";\n\
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
};\n\
\n\
QString runExternalCommands(QString str,bool capture,QString tofile,bool append)\n\
{\n\
//outop<<str<<Qt::endl;\n\
QString dataout=\"\";\n\
int slen;\n\
char *data=NULL;\n\
int comnum=0;\n\
int wordexpret=0;\n\
bool inquote;\n\
char found;\n\
int wstatus;\n\
int filefd=-1;\n\
\n\
std::string ss=str.toStdString();\n\
slen=ss.length();\n\
data=(char*)alloca(slen*2);\n\
strcpy(data,ss.c_str());\n\
if(tofile.isEmpty()==false)\n\
{\n\
ss=tofile.toStdString();\n\
if(append==true)\n\
filefd=open(ss.c_str(), O_WRONLY|O_APPEND, 0666);\n\
else\n\
filefd=open(ss.c_str(), O_WRONLY|O_CREAT, 0666);\n\
}\n\
while(strlen(data)>0)\n\
{\n\
int pipe_position=-1;\n\
for(int i=0;data[i]!=\'\\0\';i++)\n\
{\n\
if(data[i]==\'\\\'\')\n\
i+=2;\n\
if(data[i]==\'|\')\n\
{\n\
inquote=false;\n\
found=0;\n\
for(int j=0;j<i;j++)\n\
{\n\
if(data[j]==\'\\\\\')\n\
j+=2;\n\
if((found==0) && ( (data[j]==\'\"\') || (data[j]==\'\\\'\')) )\n\
{\n\
found=data[j];\n\
inquote=true;\n\
}\n\
else if((found!=0) && (data[j]==found))\n\
{\n\
found=0;\n\
inquote=false;\n\
}\n\
}\n\
if(inquote==false)\n\
{\n\
pipe_position=i;\n\
break;\n\
}\n\
}\n\
}\n\
if(pipe_position != -1)\n\
data[pipe_position] = \'\\0\';\n\
if((wordexpret=wordexp(data,&commandsvar[comnum],0))==0)\n\
comnum++;\n\
else\n\
{\n\
fprintf(stderr,\"command parse fail error: %i\\n\",wordexpret);\n\
fprintf(stderr,\"%s\\n\",data);\n\
exit(wordexpret);\n\
}\n\
if(pipe_position!=-1)\n\
data+=pipe_position+1;\n\
else\n\
data+=strlen(data);\n\
}\n\
int i;\n\
int pipe_fds[2*(comnum)];\n\
int pipefdslast[2];\n\
pid_t pid[comnum];\n\
if(capture==true)\n\
{\n\
if(pipe(pipefdslast)==-1)\n\
{\n\
perror(\"pipe\");\n\
exit(EXIT_FAILURE);\n\
}\n\
}\n\
for(i=0;i<comnum;i++)\n\
{\n\
if(pipe(pipe_fds+i*2)==-1)\n\
{\n\
perror(\"pipe\");\n\
exit(EXIT_FAILURE);\n\
}\n\
}\n\
\n\
for(i=0;i<comnum;i++)\n\
{\n\
pid[comnum]=fork();\n\
if(pid[comnum]==-1)\n\
{\n\
perror(\"fork\");\n\
exit(EXIT_FAILURE);\n\
}\n\
if(pid[comnum]==0)\n\
{\n\
if(i>0)\n\
dup2(pipe_fds[(i-1)*2],0);\n\
if(i<comnum-1)\n\
dup2(pipe_fds[i*2+1],1);\n\
else if(capture==true)\n\
dup2(pipefdslast[1],STDOUT_FILENO);\n\
else if(tofile!=NULL)\n\
{\n\
close(STDOUT_FILENO);\n\
dup(filefd);\n\
}\n\
\n\
for(int j=0;j<2*(comnum);j++)\n\
close(pipe_fds[j]);\n\
\n\
execvp(commandsvar[i].we_wordv[0],commandsvar[i].we_wordv);\n\
perror(\"execlp\");\n\
exit(EXIT_FAILURE);\n\
}\n\
}\n\
\n\
for(i=0;i<2*(comnum);i++)\n\
close(pipe_fds[i]);\n\
\n\
if(capture==true)\n\
{\n\
close(pipefdslast[1]);\n\
\n\
dataout=\"\";\n\
FILE* file=fdopen(pipefdslast[0],\"r\");\n\
if(file!=NULL)\n\
{\n\
while(fgets(buffer,BUFFER_SIZE-1,file))\n\
{\n\
dataout.append(buffer);\n\
buffer[0]=0;\n\
}\n\
fclose(file);\n\
}\n\
close(pipefdslast[0]);\n\
}\n\
waitpid(pid[comnum],&wstatus,0);\n\
exitstatus=QString::number(WEXITSTATUS(wstatus));\n\
\n\
for(int j=0;j<comnum;j++)\n\
wordfree(&commandsvar[j]);\n\
if(filefd>0)\n\
close(filefd);\n\
if(capture==true)\n\
return(dataout.trimmed());\n\
return(\"\");\n\
}\n\
\n";

	for(int j=0;j<fCode.size();j++)
		functions+=fCode.at(j);

//write code
	cCode.prepend("\nint main(int argc, char **argv)\n{\ngargv=argv;\nloadEnvironment(false,{});\nvariables[\"SHELL\"]=(getenv(\"SHELL\")!=NULL && strlen(getenv(\"SHELL\"))>0) ? (getenv(\"SHELL\")) : (\"/bin/bash\");\n");

	cCode.prepend(functions);
	cCode.prepend(globalvars);
	cCode.prepend(specialvars);
	cCode.prepend(headers);
	cCode.prepend(QString("/*\nQt C++ file for %1\nCompile with:\ng++ -Wall $(pkg-config --cflags --libs "+useQTVersion+" ) -fPIC  -Ofast /PATH/TO/THIS/FILE -o APPNAME\nOptional:\nastyle -A7 --indent=tab /PATH/TO/THIS/FILE\nstrip ./a.out\nCreated on %2\n*/\n").arg(this->argv[1]).arg(QDate::currentDate().toString()));
	cCode<<"\nreturn(0);\n}\n";

	if((fullCompileHere.isEmpty()==true) && (compileHere.isEmpty()==true))
		{
			for(int j=0;j<cCode.size();j++)
				QTextStream(stdout)<<cCode.at(j);
		}
	else
		{
			if(compileHere.isEmpty()==false)
				{
					QFileInfo	fi(compileHere);
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
					command=QString("g++ -Wall $(pkg-config --cflags --libs "+useQTVersion+" ) -fPIC  -Ofast '%1' -o '%2'").arg(foldername+"/"+filename+".cpp").arg(foldername+"/"+filename);
					errop<<"Compiling using command:\n"<<command<<"\n..."<<Qt::endl;
					system(command.toStdString().c_str());
				}
			else
				{
					QString		outfilename;
					QFile		file;
					QString		fullpath;
					QString		command;
					QProcess		proc;
					QTextStream	out(&file);

					outfilename=QFileInfo(this->mainBashFile.fileName()).baseName();
					QDir().mkpath(fullCompileHere);
					fullpath=fullCompileHere+"/"+outfilename;
					file.setFileName(fullCompileHere+"/"+outfilename+".cpp");
					if(!file.open(QIODevice::WriteOnly))
						{
							qDebug()<<"Failed to open file";
							exit(100);
						}

					for(int j=0;j<cCode.size();j++)
						out<<cCode.at(j);
					file.close();
					proc.execute(QString("/bin/sh"),QStringList()<<"-c"<<prettyCommand+" "+fullpath+".cpp");
					proc.execute(QString("/bin/sh"),QStringList()<<"-c"<<"rm &>/dev/null "+fullpath+".cpp.orig");
					if(this->lowercaseName==true)
						command=QString("g++ -Wall $(pkg-config --cflags --libs "+useQTVersion+" ) -fPIC  -Ofast '%1' -o '%2'").arg(fullpath+".cpp").arg(fullpath.toLower());
					else
						command=QString("g++ -Wall $(pkg-config --cflags --libs "+useQTVersion+" ) -fPIC  -Ofast '%1' -o '%2'").arg(fullpath+".cpp").arg(fullpath);
					errop<<"Compiling using command:\n"<<command<<"\n..."<<Qt::endl;
					proc.execute(QString("/bin/sh"),QStringList()<<"-c"<<command);
				}
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

	line.replace(QRegularExpression("\\\\(.)"),"\\\\\\1");
	if(this->verboseCompile==true)
		errop<<"Processing line "<<this->currentLine<<" "<<line<<Qt::endl;

	if(line.startsWith('#'))
		return;

	re.setPattern("^.*<<([[:space:]]*[[:alpha:]][[:alnum:]_\\[\\]]*[[:space:]]*)$");
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
		{
			lines=this->splitLines(line);
		}

	for(int j=0;j<lines.count();j++)
		{
			doignore=false;
			if(lines.at(j).trimmed().startsWith('#'))
				return;

			if(lines.at(j).trimmed()==";;")
				{
					if(isInFunction==true)
						fCode<<"}\n";
					else
						cCode<<"}\n";
					return;
				}

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

					fCode<<"if(capture==true)\nreturn(retstr.trimmed());\n";
					fCode<<"else\n{\nif(file.isOpen()==true)\nfile.close();}\n\nreturn(0);\n}\n";
					isInFunction=false;
					return;
				}

			lineend=";\n";
			retstr="";

//check for case statement
			re.setPattern("^[[:space:]]*[^\\s]*[[:space:]]*\\)|^\".*\"[[:space:]]*\\)[[:space:]]*$");
			match=re.match(lines.at(j));
			if(match.hasMatch())
				{
					retstr=commands.makeCaseCompareStatement(lines.at(j));
					lineend="";	
					if(this->outToCode(retstr,lineend,lines.at(j),false,false)==true)
						continue;
				}
			else
				{
					re.setPattern("^[[:space:]]*([/\\.\\:]*[[:alpha:]][[:alnum:]_/\\.\\[\\]]*)[[:space:]]*(=)?");
					match=re.match(lines.at(j));
					if(match.hasMatch())
						{
							if(match.captured(2).trimmed()=="=")
								{
									retstr=commands.makeAssign(lines.at(j));
									if(retstr.isEmpty()==false)
										{
											retstr="variables[\""+match.captured(1).trimmed()+"\"]="+retstr;
											QString tstr=retstr;
											tstr=tstr.replace(QRegularExpression("\\\\\\\\\\\\\"\"\\+"),"\\\"\"+");
											tstr=tstr.replace(QRegularExpression("\\+\"\\\\\\\\\\\\\"\""),"+\"\\\"\"");								
											retstr=tstr;
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
									if(match.captured(1).trimmed()=="select")
										{
											lineend="";
											this->skipDo=true;
											retstr=commands.makeSelect(lines.at(j));
										}
									if(match.captured(1).trimmed()=="break")
										retstr="break";
		
									if(match.captured(1).trimmed()=="case")
										{
											retstr=commands.makeCase(lines.at(j));
											this->outToCode(retstr,lineend,lines.at(j),doignore,true);
											continue;
										}
									if(match.captured(1).trimmed()=="esac")
										{
											caseVariable.pop_back();
											this->outToCode(retstr,lineend,lines.at(j),doignore,true);
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
											this->outToCode(retstr,lineend,lines.at(j),doignore,true);
											retstr="{\n";
										}
									if(match.captured(1).trimmed()=="else") //TODO//elif
										{
											doignore=true;
											lineend="";
											this->outToCode(retstr,lineend,lines.at(j),doignore,true);
											retstr="}\nelse\n{\n";
										}
									if(match.captured(1).trimmed()=="fi")
										{
											doignore=true;
											lineend="";
											this->outToCode(retstr,lineend,lines.at(j),doignore,true);
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
											this->outToCode(retstr,lineend,lines.at(j),doignore,true);
											if(this->skipDo==true)
												{
													this->skipDo=false;
													continue;
												}
											else
												{
													if((isInFor.isEmpty()==false) && (isInFor.back()==true))
														retstr="{\nvariables[\""+forVariable.back()+"\"].setNum("+forVariable.back()+");\n";
													else
														retstr="{\n";
												}
										}
									if(match.captured(1).trimmed()=="done")
										{
											doignore=true;
											lineend="";
											this->outToCode(retstr,lineend,lines.at(j),doignore,true);
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
												retstr=commands.makeExternalCommand(lines.at(j));
										}
								}

							this->outToCode(retstr,lineend,lines.at(j),doignore,false);
						}
					else
						{
								retstr=commands.makeBASHCliLine(lines.at(j));
								retstr="exitstatus=QString::number(WEXITSTATUS(system("+retstr+".toStdString().c_str())))";
								this->outToCode(retstr,lineend,lines.at(j),doignore,false);
						}
				}
		}
}

bool compilerClass::outToCode(QString qline,QString lineend,QString brokenline,bool doignore,bool justsay)
{
	if(justsay==true)
		{
			if((this->verboseCCode==true) && (brokenline.length()>0))
				{
					if(isInFunction==true)
						fCode<<"//"+QString("Line %0: ").arg(this->currentLine)+brokenline+"\n";
					else
						cCode<<"//"+QString("Line %0: ").arg(this->currentLine)+brokenline+"\n";
				}
			return(true);
		}

	if(qline.isEmpty()==false)
		{
			if((this->verboseCCode==true) && (brokenline.length()>0) && (doignore==false))
				{
					if(isInFunction==true)
						fCode<<"//"+QString("Line %0: ").arg(this->currentLine)+brokenline+"\n";
					else
						cCode<<"//"+QString("Line %0: ").arg(this->currentLine)+brokenline+"\n";
				}

			if(isInFunction==true)
				fCode<<qline<<lineend;
			else
				cCode<<qline<<lineend;
			return(true);
		}
	return(false);
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

	//DB_showCode();
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
					continue;
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
					continue;
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
					lines<<tstr.left(pos);
					lines<<";;";
					tstr=tstr.mid(pos+2);
					pos=0;
					continue;
					pos+=2;
					continue;
				}

			if(tstr.at(pos).toLatin1()==';')
				{
					//if((tstr.length()>pos) && (tstr.at(pos+1).toLatin1()==';'))
//						{
//							pos+=2;
//							continue;
//						}
					lines<<tstr.left(pos);
					tstr=tstr.mid(pos+1);
					pos=0;
					continue;
				}

			for(int k=0;k<keywords.count();k++)
				{
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
	for(int j=0;j<lines.size();j++)
		if(lines.at(j).isEmpty()==true)
			lines.remove(j,1);
	//DB_printLines(lines);
	return(lines);
}