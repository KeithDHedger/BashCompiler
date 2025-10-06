/*
 *
 * ©K. D. Hedger. Tue 22 Apr 14:49:51 BST 2025 keithdhedger@gmail.com

 * This file (commands.cpp) is part of BashCompiler.

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

#include "commands.h"

commandsClass::~commandsClass()
{
}

commandsClass::commandsClass()
{
}

QString commandsClass::makeFunction(QString qline)
{
	QString					tstr;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	QString					leftstr;
	QString					midstr;
	QString					ritestr;
	QString					retstr="";

//call no return val
	re.setPattern("^[[:space:]]*([[:alnum:]_]+)[[:space:]]*(.*)$");
	match=re.match(qline);
	if(match.hasMatch())
		{
			for(int j=0;j<functionNames.count();j++)
				{
					if(match.captured(1).trimmed().compare(functionNames.at(j).trimmed())==0)
						{
							parseFileClass	pfl;
							parseFileClass	pfl1;
							QString varstr="{";
							tstr=match.captured(2).trimmed();
							pfl.parseLine(tstr);
							int argcnt=1;
							for(int kk=0;kk<pfl.lineParts.count();kk++)
								{
									pfl1.parseLine(pfl.lineParts.at(kk).data);
									tstr=pfl1.parseExprString(false);
									tstr.replace("\"\"\"+","");
									tstr.replace("+\"\"\"","");
									varstr+=QString("{\"%1\",").arg(argcnt++)+tstr+"},";
								}
							varstr+="}";
							retstr=match.captured(1).trimmed()+"(false,"+varstr+")";
							return(retstr);
						}
				}
		}
//define
	retstr=this->makeFunctionDefine(qline);
	return(retstr);
}

QString commandsClass::makeBASHCliLine(QString qline)
{
	QString	data=qline;
	QString	varname="";
	QString	whatin;
	QString	formatstr="";
	QString	argstr="";
	int		j=0;
	int		formatcnt=1;

	if(isInFunction==false)
		whatin="_BC_gargv";
	else
		whatin="fv";

	while(j<data.length())
		{
			if(data.at(j).toLatin1()=='\'')
				{
					formatstr+="'";
					j++;
					while(data.at(j).toLatin1()!='\'')
						{
							if(data.at(j).toLatin1()=='"')
								{
									formatstr+="\\\"";
								}
							else
								formatstr+=data.at(j);
							j++;
						}
					formatstr+="'";
					j++;
					continue;
				}

			if(data.at(j).toLatin1()=='"')
				{
					formatstr+="\\\"";
					j++;
					continue;
				}

			if(data.at(j).toLatin1()=='$')
				{
					if((data.at(j+1).isNumber()) || (data.at(j+1).toLatin1()=='{' && data.at(j+2).isNumber()))
						{
							if(data.at(j+1).toLatin1()=='{')
								j++;
							if(whatin=="_BC_gargv")
								argstr+=".arg("+whatin+"["+data.at(j+1)+"])";
							else
								argstr+=".arg("+whatin+"[\""+data.at(j+1)+"\"])";
							formatstr+="%"+QString::number(formatcnt++);

							j+=2;
							if(data.at(j).toLatin1()=='}')
								j++;
							continue;
						}
					varname="_BC_variables[\"";
					j++;

					if(data.at(j).toLatin1()=='{')
						{
							j++;
							while(data.at(j).toLatin1()!='}')
								{
									varname+=data.at(j).toLatin1();
									j++;
								}
							varname+="\"]";
							formatstr+="%"+QString::number(formatcnt++);
							argstr+=".arg("+varname+")";
							varname="";
							j++;
							continue;
						}
					else
						{
						//	while(j<data.length() && data.at(j).toLatin1()!=' ' && data.at(j).toLatin1()!='"' && data.at(j).toLatin1()!='\t' && data.at(j).toLatin1()!='|' && data.at(j).toLatin1()!='$' && data.at(j).toLatin1()!='\'')
							while(j<data.length() && data.at(j).toLatin1()!=' ' && data.at(j).toLatin1()!='"' && data.at(j).toLatin1()!='\t' && data.at(j).toLatin1()!='|' && data.at(j).toLatin1()!='$')
								{
									varname+=data.at(j).toLatin1();
									j++;
								}
							varname+="\"]";
							formatstr+="%"+QString::number(formatcnt++);
							argstr+=".arg("+varname+")";
							continue;
						}
				}
			formatstr+=data.at(j);
			j++;
		}

	return("QString(\""+formatstr+"\")"+argstr);
}

QString commandsClass::simpleSub(QString qline)
{
	QString	data=qline;
	QString	varname="";
	QString	whatin;
	QString	formatstr="";
	QString	argstr="";
	int		j=0;

	QString	retstr="";
	parseFileClass pfl;

	while(j<data.length())
		{
			if(data.at(j).toLatin1()=='$')
				{
					varname="";
					j++;

					if(data.at(j).toLatin1()=='{')
						{
							j++;
							while(data.at(j).toLatin1()!='}')
								{
									varname+=data.at(j).toLatin1();
									j++;
								}
							
							retstr+=pfl.parseVar("${"+varname+"}");
							j++;
							continue;
						}
				}
			retstr+=data.at(j).toLatin1();
			j++;
		}

	return(retstr);
}

QString commandsClass::makeExternalCommand(QString qline)
{
	QString	formatstr="";
	QString	datastr="";
	QString	final;
	QString	tstr;
	QString retstr=qline;
	QString outto="\"\"";
	QString line=qline;
	QString	append="false";

	if(qline.isEmpty()==true)
		return("");

	QRegularExpression		re;
	QRegularExpressionMatch	match;
	re.setPattern("(.*)([[:space:]]+>+[[:space:]]+)(.*)[[:space:]]*$");
	match=re.match(qline);
	if(match.hasMatch())
		{
			outto=match.captured(3).trimmed();
			line=match.captured(1).trimmed();
			if(match.captured(2).trimmed()==">>")
				append="true";
		}

	tstr=this->makeBASHCliLine(line);
	outto.replace(QRegularExpression(R"RX(^"|"$)RX"),"");
	outto="\""+outto+"\"";

	if(isInFunction==true)
		return("ss<<_BC_runExternalCommands("+tstr+",true,"+outto+","+append+")<<Qt::endl");
	else
		return("_BC_runExternalCommands("+tstr+",false,"+outto+","+append+")");

	return("");
}

QString commandsClass::makeFunctionDefine(QString qline)
{
	QString					line=qline;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	QString					retstr="";

	re.setPattern("^[[:space:]]*([[:alnum:]_]+)[[:space:]]*\\([[:space:]]*\\)[[:space:]]*$");
	match=re.match(line);
	if(match.hasMatch())
		{
			isInFunction=true;
			retstr="QString "+match.captured(1).trimmed()+"(bool capture,QHash<QString, QString> fv)\n{\nQString retstr;\nQTextStream ss;\nQFile file;\n";
			retstr+="if(capture==true)\nss.setString(&retstr, QIODevice::WriteOnly);\nelse\n";
			retstr+="{\nfile.open(stdout, QIODevice::WriteOnly);\nss.setDevice(&file);\n}";
			functionNames<<match.captured(1).trimmed();
		}
	return(retstr);
}

QString commandsClass::makeIf(QString qline)
{
	QString					tstr;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	QString					leftstr;
	QString					midstr;
	QString					ritestr;
	QString					retstr="";
	int						cnt=0;
	QString					doint="";
	parseFileClass			pfl;
	parseFileClass			pfr;
	bool						nowrapper;

	while(bashmath[cnt]+=NULL)
		tstr+=bashmath[cnt++]+QString("|");
	tstr=tstr.left(tstr.length()-1);

	re.setPattern("^[[:space:]]*if[[:space:]]*\\[+[[:space:]]*(.*)[[:space:]]*("+tstr+")[[:space:]]*([^]]+)[^;]\\]*");
	match=re.match(qline);
	if(match.hasMatch())
		{
			midstr=match.captured(2);
			if(midstr.at(0)=='-')
				doint=".toInt()";

			tstr=match.captured(1).trimmed();
			pfl.parseLine(tstr);
			leftstr=pfl.parseOutputString(tstr)+doint;
			tstr=match.captured(3).trimmed();
			pfr.parseLine(tstr);
			ritestr=pfr.parseOutputString(tstr)+doint;
			cnt=0;
			while(bashmath[cnt]+=NULL)
				{
					if(midstr.compare(bashmath[cnt])==0)
						{
							midstr=cmath[cnt];
							break;
						}
					cnt++;		
				}

			leftstr=pfl.optimizeOP(leftstr,&nowrapper);
			ritestr=pfl.optimizeOP(ritestr,&nowrapper);

			retstr="if("+leftstr+midstr+ritestr+")\n";
		}
	else
		{
			re.setPattern("^[[:space:]]*if[[:space:]]*\\[+[[:space:]]*(!)?[[:space:]]*-([efdhxrwL])[[:space:]](.*)[[:space:]]*[^]]+");
			match=re.match(qline);
			if(match.hasMatch())
				{
					QString testwhat="true";
					if(match.captured(1).trimmed()=="!")
						testwhat="false";
					ritestr=pfl.parseOutputString(match.captured(3).trimmed());
					ritestr=pfl.optimizeOP(ritestr,&nowrapper);
					switch(match.captured(2).trimmed().at(0).toLatin1())
						{
							case 'e':
								retstr="if(QFileInfo::exists("+ritestr+")=="+testwhat+")\n";
								break;
							case 'd':
								retstr="if(QFileInfo("+ritestr+").isDir()=="+testwhat+")\n";
								break;
							case 'f':
								retstr="if(QFileInfo("+ritestr+").isFile()=="+testwhat+")\n";
								break;
							case 'h':
							case 'L':
								retstr="if(QFileInfo("+ritestr+").isSymLink()=="+testwhat+")\n";
								break;
							case 'x':
								retstr="if(QFileInfo("+ritestr+").isExecutable()=="+testwhat+")\n";
								break;
							case 'r':
								retstr="if(QFileInfo("+ritestr+").isReadable()=="+testwhat+")\n";
								break;
							case 'w':
								retstr="if(QFileInfo("+ritestr+").isWritable()=="+testwhat+")\n";
								break;
							default:
								retstr="if(false)\n";
								break;
						}
				}
		}
	return(retstr);
}

QString commandsClass::makeWhileRead(QString qline)
{
	QString					line=qline;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	QString					commstr;
	QString					midstr;
	QString					ritestr;
	QString					retstr="";

	isInFor.push_back(false);
	forVariable.push_back("");

	re.setPattern("^[[:space:]]*(while[[:space:]]*read)[[:space:]]*(.*)$");
	match=re.match(line);
	if((match.hasMatch()) && (match.captured(1).simplified().compare("while read")==0))
		{
			if(isInFunction==true)
				{
					retstr+="FILE *fp;\n";
					fCode+="{\n";
					whileReadLine.append(fCode.size());
					retstr+="fp=popen(proc.toStdString().c_str(),\"r\");\n";
					retstr+="QTextStream	inpop(fp);\n";
					if(match.captured(2).trimmed().isEmpty()==true)
						retstr+="while(inpop.readLineInto(&_BC_variables[\"REPLY\"]))\n";
					else
						retstr+="while(inpop.readLineInto(&_BC_variables[\""+match.captured(2).trimmed()+"\"]))\n";
				}
			else
				{
					retstr+="FILE *fp;\n";
					cCode+="{\n";
					whileReadLine.append(cCode.size());
					retstr+="fp=popen(proc.toStdString().c_str(),\"r\");\n";
					retstr+="QTextStream	inpop(fp);\n";
					if(match.captured(2).trimmed().isEmpty()==true)
						retstr+="while(inpop.readLineInto(&_BC_variables[\"REPLY\"]))\n";
					else
						retstr+="while(inpop.readLineInto(&_BC_variables[\""+match.captured(2).trimmed()+"\"]))\n";
				}
			return(retstr);
		}
	return(retstr);
}

QString commandsClass::makeWhile(QString qline)
{
//use makeIf co they're the same practically
	QString	retstr="";
	QString	line=qline;

	line.replace(QRegularExpression(R"foo(^[[:space:]]*while)foo"),"if");
	retstr=this->makeIf(line);
	retstr.replace(QRegularExpression(R"foo(^[[:space:]]*if)foo"),"while");

	return(retstr);
}

QString commandsClass::makeDone(QString qline)
{
	QString					retstr="";
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	QRegularExpressionMatch	match2;
	parseFileClass			pfl;

	if((isInFor.isEmpty()==false) && (isInFor.back()==true))
		{
			retstr="}\n";
		}
	else
		{
			re.setPattern("^[[:space:]]*(done)[[:space:]]*<[[:space:]]*(<\\((.*)\\)|(.*))");
			match=re.match(qline);
			if(match.hasMatch())
				{
					int matchnum=3;
					QString doacat="";

					if(match.captured(matchnum).trimmed().isEmpty()==false)
						{
							QString tstr="";
							re.setPattern("^[[:space:]]*([[:alnum:]_]+)[[:space:]]*(.*)$");
							match2=re.match(match.captured(matchnum).trimmed());

							if(match2.hasMatch())
								{
									for(int j=0;j<functionNames.count();j++)
										{
											if(match2.captured(1).trimmed().compare(functionNames.at(j).trimmed())==0)
												{
													parseFileClass	pfl;
													parseFileClass	pfl1;
													QString varstr="{";
													tstr=match2.captured(2).trimmed();
													pfl.parseLine(tstr);
													int argcnt=1;
													for(int kk=0;kk<pfl.lineParts.count();kk++)
														{
															pfl1.parseLine(pfl.lineParts.at(kk).data);
															tstr=pfl1.parseExprString(false);
															tstr.replace("\"\"\"+","");
															tstr.replace("+\"\"\"","");
															varstr+=QString("{\"%1\",").arg(argcnt++)+tstr+"},";
														}
													varstr+="}";
													retstr=match2.captured(1).trimmed()+"(true,"+varstr+")";
													if(isInFunction==true)
														fCode.insert(whileReadLine.last(),"QString proc=\"/usr/bin/echo \\\"\"+"+retstr+"+\"\\\"|cat -\";\n");
													else
														cCode.insert(whileReadLine.last(),"QString proc=\"/usr/bin/echo \\\"\"+"+retstr+"+\"\\\"|cat -\";\n");
													whileReadLine.pop_back();
													return("}\n}\n");
												}
										}
								}
						}

					if(match.captured(matchnum).trimmed().isEmpty()==true)
						{
							matchnum=4;
							doacat="cat ";
						}
					if(whileReadLine.size()>0)
						{
							QString tstr=pfl.lineToBashCLIString(match.captured(matchnum).trimmed());
							retstr="}\npclose(fp);\n}\n";
							if(isInFunction==true)
								fCode.insert(whileReadLine.last(),"QString proc=\""+doacat+tstr+"\";\n");
							else
								cCode.insert(whileReadLine.last(),"QString proc=\""+doacat+tstr+"\";\n");
							whileReadLine.pop_back();
						}
					else
						{
							retstr="}\n";
						}
				}
		}
	if(isInFor.isEmpty()==false)
		{
			isInFor.pop_back();
			forVariable.pop_back();
		}
	return(retstr);
}

QString commandsClass::makePopd(QString qline)
{
	QString					tstr;
	QString					line=qline;
	QRegularExpression		re;
	QRegularExpressionMatch	match;

	re.setPattern("^[[:space:]]*?(popd)[[:space:]]*.*");
	match=re.match(line);
	if(match.hasMatch())
		tstr="if(_BC_dirStack.isEmpty()==false) QDir::setCurrent(_BC_dirStack.takeLast())";
	return(tstr);
}

QString commandsClass::makeUnset(QString qline)
{
	QString					tstr="";
	QString					line=qline;
	QRegularExpression		re;
	QRegularExpressionMatch	match;

	re.setPattern(R"RX(^[[:space:]]*?(unset)[[:space:]]+(.*))RX");
	match=re.match(line);
	if(match.hasMatch())
		{
			QStringList ss=match.captured(2).trimmed().split(" ",Qt::SkipEmptyParts);
			for(int j=0;j<ss.size();j++)
				{
					tstr+="unsetenv(\""+ss.at(j)+"\");\n";
					tstr+="_BC_variables.remove(\""+ss.at(j)+"\");\n";
				}
		}
	tstr.remove(QRegularExpression(";\n$"));
	return(tstr);
}

QString commandsClass::makeExport(QString qline)
{
	QString					tstr="";
	QString					line=qline;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	parseFileClass			pfl;
	bool						success=false;
	re.setPattern(R"RX(^[[:space:]]*?(export)[[:space:]]+(.*)=(.*))RX");
	match=re.match(line);
	if(match.hasMatch())
		{
			tstr=pfl.reentrantParseVar(match.captured(3).trimmed());
			tstr=tstr.replace(QRegularExpression("^[\"${]*|[}\"]*$"),"");
			if(tstr.isEmpty()==true)
				{
					tstr=match.captured(3).trimmed();
					tstr=QString("setenv(\"%1\",\"%2\",1)").arg(match.captured(2).trimmed()).arg(tstr);
				}
			else
				{
					tstr=QString("setenv(\"%1\",%2.toStdString().c_str(),1)").arg(match.captured(2).trimmed()).arg(tstr);
				}
			return(tstr);
		}
	else
		{
			re.setPattern(R"RX(^[[:space:]]*?(export)[[:space:]]+(.*))RX");
			match=re.match(line);
			if(match.hasMatch())
				{
					QStringList ss=match.captured(2).trimmed().split(" ",Qt::SkipEmptyParts);
					for(int j=0;j<ss.size();j++)
						{
							tstr+=QString("setenv(\"%1\",%2.toStdString().c_str(),1)").arg(ss.at(j).trimmed()).arg("_BC_variables[\""+ss.at(j).trimmed()+"\"]");
							tstr+=";\n";
						}
				}
		}
	tstr.remove(QRegularExpression(";\n$"));
	return(tstr);
}

QString commandsClass::makeExit(QString qline)
{
	QString					tstr="";
	QString					line=qline;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	parseFileClass			pfl;

	re.setPattern("^[[:space:]]*?(exit)[[:space:]]*(.*)?[[:space:]]*");
	match=re.match(line);
	if(match.hasMatch())
		{
			if(match.captured(2).trimmed().isEmpty()==false)
				{
					tstr=match.captured(2).trimmed();
					pfl.parseLine(tstr);
					tstr=pfl.parseExprString(true);
					tstr="exit("+tstr+")";
				}
			else
				tstr="exit(0)";
		}
	return(tstr);
}

QString commandsClass::makePushd(QString qline)
{
	QString					tstr="";
	QString					line=qline;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	parseFileClass			pfl;

	re.setPattern("^[[:space:]]*?(pushd)[[:space:]]+(.*)");
	match=re.match(line);
	if(match.hasMatch())
		{
			tstr=match.captured(2).trimmed();
			tstr.remove(QRegularExpression("[[:space:]&123][[:space:]]*>[[:space:]]*.*$"));
			pfl.parseLine(tstr);
			tstr=pfl.parseExprString(false);
			tstr="_BC_dirStack<<QDir::currentPath();\nQDir::setCurrent("+tstr+")";
		}
	return(tstr);
}

QString commandsClass::makePrintf(QString qline)
{
	QString					tstr;
	QString					tstr2;
	QString					tstr3;
	QString					line=qline;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	parseFileClass			pfl;

	pfl.preserveWhitespace=true;
	re.setPattern("[[:space:]]*printf[[:space:]]*(-v)?[[:space:]]*(\"?[^\\s]*\"?)?[[:space:]]*(.*)");
	match=re.match(line);
	if(match.hasMatch())
		{
			if(match.captured(1).trimmed().compare("-v")==0)
				{
					QString newvar=match.captured(2).trimmed();
					newvar=pfl.parseOutputString(match.captured(2).trimmed());
					tstr=match.captured(3).trimmed();
					re.setPattern("[[:space:]]*\"?(%[[:alnum:]_]+)\"?[[:space:]]+(.*)");
					match=re.match(tstr);
					if(match.hasMatch())
						{
							tstr2=pfl.parseOutputString(match.captured(2).trimmed());
							tstr="_BC_variables["+newvar+"]="+tstr2;
						}
					return(tstr);
				}
			else
				{
					tstr=pfl.parseOutputString(match.captured(3).trimmed());
					if(match.captured(2).trimmed().endsWith("\\n\"")==true)//TODO//horrible hack!
						tstr+="<<Qt::endl";
					tstr="_BC_outOP<<"+tstr;
					return(tstr);
				}
		}
	return("");
}

QString commandsClass::makeAssign(QString qline)
{
	QString			pal;
	QString			retstr;
	QString			line=qline;
	parseFileClass	pfl;
	bool				ok;

	line.replace(QRegularExpression("(?<!\")~(?!\")"),"_BC_variables[\"HOME\"]");
	pfl.parseLine(line);

	pal=pfl.parseExprString(false);
	//pal=pfl.reentrantParseVar(pal);
	//DB_printParts(pfl.lineParts);
	pal.replace(QRegularExpression("\\\\([[:alpha:]])"),"\\1");
	pal.replace(QRegularExpression("\"(_BC_variables\\[\"[[:alpha:][:alnum:]_]*\"\\])\""),"\\1");
	retstr=pfl.optimizeOP(pal,&ok);

	return(retstr);
}

//TODO//More optimisations...
QString commandsClass::optEchoLine(QString qline,bool preserve,bool escapes,bool force)
{
	QString	tstr=qline;
	QString replaces=".replace(\"\\\\n\",\"\\n\").replace(\"\\\\t\",\"\\t\").replace(\"\\\\e\",\"\\e\").replace(\"\\\\r\",\"\\r\")";
	QString start="QString(";
	QString dne=")";

	if(qline.contains("_BC_variables[\"")==false)
		{
			if(preserve==false && escapes==true)
				tstr.replace(_BC_replaceWhiteSpace," ").replace("\\\\n","\\n").replace("\\\\t","\\t").replace("\\\\e","\\e").replace("\\\\r","\\r");
			else if(preserve==true && escapes==true)
				tstr.replace("\\\\n","\\n").replace("\\\\t","\\t").replace("\\\\e","\\e").replace("\\\\r","\\r");
			else if(preserve==false && escapes==false)
				tstr.replace(_BC_replaceWhiteSpace," ").replace("\\\\t","\\t").replace("\\\\e","\\e").replace("\\\\r","\\r");
			return(tstr);
		}

	if((preserve==true || escapes==true) && (force==false))
		{
			if(tstr.startsWith("QString("))
				{
					start="";
					dne="";
				}
		}

	if(preserve==false && escapes==true)
		tstr=start+tstr+dne+".replace(_BC_replaceWhite, \" \").trimmed()"+replaces;
	else if(preserve==true && escapes==true)
		tstr=start+tstr+dne+replaces;
	else if(preserve==false && escapes==false)
		tstr=tstr=start+tstr+dne+".replace(_BC_replaceWhite, \" \").trimmed()";
	return(tstr);
}

bool commandsClass::containsPipeOutsideQuotes(const QString &text)
{
	QRegularExpression regex(R"RX("([^"]*)"|([^"|]*)|(\|))RX");
	QRegularExpressionMatchIterator it=regex.globalMatch(text);

	while (it.hasNext())
		{
			QRegularExpressionMatch match = it.next();
			if (match.captured(3) == "|")
				return(true);
		}
	return(false);
}

QString commandsClass::makeEcho(QString qline)
{
	QString					tstr;
	QString					line=qline;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	QString					outwhat="";
	QString					outendl="Qt::endl";
	QString					whereto="";
	parseFileClass			pfl;
	bool						nowrapper;
	bool						preserve=false;
	bool						escapes=false;
	bool						nonl=false;
	bool						force=false;
	bool						emptyEcho=false;

	pfl.preserveWhitespace=true;

	if(this->containsPipeOutsideQuotes(line)==true)
		return("");

	re.setPattern("[[:space:]]*echo[[:space:]]*(-.\\s*)?(-.?\\s*)?[[:space:]]*(.*)([[:space:]]+>+[[:space:]]+)(.*)");
	match=re.match(line);
	if(match.hasMatch())
		{
			if(match.captured(1).trimmed()=="-e" || match.captured(2).trimmed()=="-e")
				escapes=true;
			if(match.captured(1).trimmed()=="-n" || match.captured(2).trimmed()=="-n")
				nonl=true;
			outwhat=match.captured(4).trimmed();
			whereto=match.captured(5).trimmed();
			force=true;
		}
	else
		{
			re.setPattern("[[:space:]]*echo[[:space:]]*(-.\\s*)?(-.?\\s*)?[[:space:]]*(.*)");
			match=re.match(line);
			if(match.hasMatch())
				{
					if(match.captured(1).trimmed()=="-e" || match.captured(2).trimmed()=="-e")
						escapes=true;
					if(match.captured(1).trimmed()=="-n" || match.captured(2).trimmed()=="-n")
						nonl=true;
				}
		}

	if(nonl==true)
		outendl="Qt::flush";

	nowrapper=true;
	emptyEcho=match.captured(3).isEmpty();
	if(emptyEcho==false)
		{
			tstr=pfl.parseOutputString(match.captured(3));
			if(match.captured(3).contains(QRegularExpression("^\\\".*\\\"$"))==true)
				preserve=true;

			tstr=pfl.optimizeOP(tstr,&nowrapper);
			tstr=this->optEchoLine(tstr,preserve,escapes,force);
		}

	whereto=pfl.parseOutputString(whereto);
	whereto=pfl.optimizeOP(whereto,&nowrapper);

	if(outwhat.isEmpty()==true)
		{
			if(emptyEcho==false)
				{
					if(isInFunction==false)
						tstr="_BC_outOP<<"+tstr+"<<"+outendl;
					else
						tstr="ss<<"+tstr+"<<"+outendl;
				}
			else
				{
					if(isInFunction==false)
						tstr="_BC_outOP<<"+outendl;
					else
						tstr="ss<<"+outendl;
				}
		}
	else
		{
			QString outofile;
			if(outwhat=="|")
				return("");
			if(outwhat==">")
				outwhat="QIODevice::WriteOnly";
			else
				outwhat="QIODevice::Append";
			if(emptyEcho==false)
				outofile="{\nQString f="+whereto+";\nQFile fh(f);\nfh.open("+outwhat+");\nQTextStream out(&fh);\nout<<"+tstr+"<<"+outendl+";\n}";
			else
				outofile="{\nQString f="+whereto+";\nQFile fh(f);\nfh.open("+outwhat+");\nQTextStream out(&fh);\nout<<"+outendl+";\n}";
			return(outofile);
		}

	return(tstr);
}

QString commandsClass::makeFor(QString qline)
{
	QString					tstr="";
	QString					line=qline;
	QRegularExpression		re;
	QRegularExpressionMatch	match;

	re.setPattern("^[[:space:]]*for[[:space:]]*\\(\\((.*)[[:space:]]*;[[:space:]]*(.*)[[:space:]]*;[[:space:]]*(.*)\\)\\)");
	match=re.match(line);
	if(match.hasMatch())
		{
			isInFor.push_back(true);
			tstr="for(int "+match.captured(1).trimmed()+";"+match.captured(2).trimmed()+";"+match.captured(3).trimmed()+")\n";
			re.setPattern("([[:alpha:]][[:alnum:]_]*)[[:space:]]*=[[:space:]]*");
			match=re.match(match.captured(1).trimmed());
			forVariable.push_back(match.captured(1));
		}
	return(tstr);	
}

QString commandsClass::makeHereDoc(QString qline)
{
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	QString					procstr;
	QString					op;
	QString					data="";
	QString					outto;
	QString					hl;
	bool						loop=true;
	parseFileClass			pfl;

	re.setPattern("^[[:space:]]*(.*)[[:space:]]+<<([[:space:]]*[[:alpha:][:alnum:]]*[[:space:]]*)$");
	match=re.match(qline);
	if(match.hasMatch())
		{
			outto=pfl.lineToBashCLIString(match.captured(1).trimmed());
			while(loop==true)
				{
					hl=mainCompilerClass->mainBashFile.readLine();
					mainCompilerClass->currentLine++;
					if(hl.trimmed()==match.captured(2).trimmed())
						{
							loop=false;
							continue;
						}
					data+=hl;
				}

			data.replace("\"","\\\"");
			data.replace("\\\\","\\");
			data.replace(";","\\\\;");
			data.replace("\n","\\n");
			data.replace("\t","\\t");
			data.replace(QRegularExpression("([^\\\\])\\$\\{?([[:alpha:][:alnum:]_]*)}?"),"\\1\"+_BC_variables[\"\\2\"]+\"");
			data.replace("\\$","\\\\$");

			procstr="echo -e \\\""+data+"\\\"";
			op="system(QString(\"%1|%2\").arg(\""+procstr+"\").arg(\""+outto+"\").toStdString().c_str())";
			return(op);
		}
	return("");
}

QString commandsClass::makeCD(QString qline)
{
	QString					tstr="";
	QString					line=qline;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	parseFileClass			pfl;

	re.setPattern("^[[:space:]]*?(cd)[[:space:]]+(.*)");
	match=re.match(line);
	if(match.hasMatch())
		{
			tstr=match.captured(2).trimmed();
			tstr.remove(QRegularExpression("[[:space:]&123][[:space:]]*>[[:space:]]*.*$"));
			pfl.parseLine(tstr);
			tstr=pfl.parseExprString(false);
			tstr.replace("\"\"\"+","");
			tstr.replace("+\"\"\"","");
			tstr="QDir::setCurrent("+tstr+")";
		}
	return(tstr);
}
QString commandsClass::makeCase(QString qline)
{
	QString					tstr;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	parseFileClass			pfl;

	re.setPattern("^[[:space:]]*?(case)[[:space:]]+(.*)[[:space:]]+in[[:space:]]*$");
	match=re.match(qline);
	if(match.hasMatch())
		{
			tstr=pfl.parseOutputString(match.captured(2).trimmed());
			caseVariable.push_back(tstr);
			firstCasecompare=true;
		}
	return("");
}

QString commandsClass::makeCaseCompareStatement(QString qline)
{
	QString					tstr;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	parseFileClass			pfl;

	re.setPattern("(.*)\\)[[:space:]]*$");
	match=re.match(qline);
	if(match.hasMatch())
		{
			tstr=match.captured(1).trimmed();
			tstr.remove(QRegularExpression("^\"|\"$"));
			tstr=pfl.globToRX(tstr,true);
			tstr=pfl.parseOutputString("^"+tstr+"\\$");
		
			if(firstCasecompare==true)
				tstr="if("+caseVariable.back()+".contains(QRegularExpression("+tstr+")))\n{\n";
			else
				tstr="else if("+caseVariable.back()+".contains(QRegularExpression("+tstr+")))\n{\n";
			firstCasecompare=false;
			return(tstr);
		}
	return("");
}

QString commandsClass::makeRead(QString qline)
{
	parseFileClass	pfl;
	QString			tstr;
	QString			retstr;

	tstr=qline;
	if(tstr.contains(QRegularExpression("^.*[[:space:]]+([[:alpha:]][[:alnum:]_]*)[[:space:]]*$")))
		tstr.replace(QRegularExpression("^.*[[:space:]]+([[:alpha:]][[:alnum:]_]*)[[:space:]]*$"),"\\1");
	else
		tstr="REPLY";

	retstr=QString("_BC_variables[\""+tstr+"\"]=_BC_procSubCheat(\"%1;echo ${"+tstr+"}\")").arg(pfl.lineToBashCLIString(qline));
	return(retstr);
}

QString commandsClass::makeSelect(QString qline)
{
	QString					retstr="";
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	QString					repstr="";

	re.setPattern("^[[:space:]]*select[[:space:]]*([[:alpha:][:alnum:]_]*)[[:space:]]*in[[:space:]]*(.*)");
	match=re.match(qline);
	if(match.hasMatch())
		{
			retstr=match.captured(2).trimmed().replace("~","${HOME}");
			retstr=this->makeAssign("data="+retstr);
			if(retstr.startsWith("QString("))
				repstr=".replace(\"\\n\",\" \")";
			else
				{
					retstr="QString("+retstr;
					repstr=").replace(\"\\n\",\" \")";
				}
			retstr="while(true)\n{\n_BC_variables[\""+match.captured(1).trimmed()+"\"]=_BC_procSubCheat(QString(\"select "+match.captured(1).trimmed()+" in \"+"+retstr+""+repstr+"+\"";
			retstr+=";do break;done;echo $"+match.captured(1).trimmed()+"\"));";
		}
	return(retstr);
}
