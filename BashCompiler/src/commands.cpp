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

QString commandsClass::makeExternalCommand(QString qline)
{
	QString	formatstr="";
	QString	datastr="";
	QString	final;
	QString	tstr;
	QString retstr=qline;

	if(qline.isEmpty()==true)
		return("");

	tstr=qline;
	tstr=tstr.replace("\"","\\\"");
	tstr=tstr.replace(QRegularExpression("\\${*([[:alpha:]][[:alnum:]_]*)}*"),"\"+variables[\"\\1\"]+\"");

	if(isInFunction==true)
		tstr=tstr.replace(QRegularExpression("\\${*([[0-9]]*)}*"),"\"+QString(fv[\"\\1\"])+\"");
	else
		tstr=tstr.replace(QRegularExpression("\\${*([[0-9]]*)}*"),"\"+QString(gargv[\\1])+\"");

	retstr="exitstatus=QString::number(WEXITSTATUS(system(QString(\""+tstr+"\").toStdString().c_str())));\nfflush(NULL)";
	return(retstr);
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
			retstr="QString "+match.captured(1).trimmed()+"(bool capture,QHash<QString, QString> fv)\n{\nQString retstr;";
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
			retstr="if("+leftstr+midstr+ritestr+")\n";
		}
	else
		{
			re.setPattern("^[[:space:]]*if[[:space:]]*\\[+[[:space:]]*(!)?[[:space:]]*-([efdhxrw])[[:space:]](.*)[[:space:]]*[^]]+");
			match=re.match(qline);
			if(match.hasMatch())
				{
					QString testwhat="true";
					if(match.captured(1).trimmed()=="!")
						testwhat="false";
					ritestr=pfl.parseOutputString(match.captured(3).trimmed());
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
						retstr+="while(inpop.readLineInto(&variables[\"REPLY\"]))\n";
					else
						retstr+="while(inpop.readLineInto(&variables[\""+match.captured(2).trimmed()+"\"]))\n";
				}
			else
				{
					retstr+="FILE *fp;\n";
					cCode+="{\n";
					whileReadLine.append(cCode.size());
					retstr+="fp=popen(proc.toStdString().c_str(),\"r\");\n";
					retstr+="QTextStream	inpop(fp);\n";
					if(match.captured(2).trimmed().isEmpty()==true)
						retstr+="while(inpop.readLineInto(&variables[\"REPLY\"]))\n";
					else
						retstr+="while(inpop.readLineInto(&variables[\""+match.captured(2).trimmed()+"\"]))\n";
				}
			return(retstr);
		}
	return(retstr);
}

QString commandsClass::makeWhile(QString qline)
{
	QString					retstr="";
	QString					tstr;
	QString					line;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	QString					leftstr;
	QString					midstr;
	QString					ritestr;
	int						cnt=0;
	bool						donumexpr=true;
	parseFileClass			pfl;
	parseFileClass			pfr;

	isInFor.push_back(false);
	forVariable.push_back("");
	line=qline;

	while(bashmath[cnt]+=NULL)
		tstr+=bashmath[cnt++]+QString("|");
	tstr=tstr.left(tstr.length()-1);

	re.setPattern("^[[:space:]]*(while)[[:space:]]*\\[+[[:space:]]*(.*)[[:space:]]*("+tstr+")[[:space:]]*([^]]+)[^;]\\]*;*(.*)");
	match=re.match(line);
	if((match.hasMatch()) && (match.captured(1).trimmed().compare("while")==0))
		{
			midstr=match.captured(3);
			if(midstr.at(0)=='-')
				donumexpr=true;
			else
				donumexpr=false;
	
			tstr=match.captured(2).trimmed();
			pfl.parseLine(tstr);
			leftstr=pfl.parseExprString(donumexpr);
			tstr=match.captured(4).trimmed();
			pfr.parseLine(tstr);
			ritestr=pfr.parseExprString(donumexpr);

			cnt=0;
			while(bashmath[cnt]+=NULL)
				{
					if(midstr.compare(bashmath[cnt])==0)
						midstr=cmath[cnt];
					cnt++;		
				}
			retstr="while("+leftstr+midstr+ritestr+")\n";
		}

	return(retstr);
}

QString commandsClass::makeDone(QString qline)
{
	QString					retstr="";
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	parseFileClass			pfl;

	if((isInFor.isEmpty()==false) && (isInFor.back()==true))
		{
			retstr="}\n";
		}
	else
		{
			//re.setPattern("^[[:space:]]*(done)[[:space:]]*<[[:space:]]*.*<\\((.*)\\)");
			re.setPattern("^[[:space:]]*(done)[[:space:]]*<[[:space:]]*(<\\((.*)\\)|(.*))");
			match=re.match(qline);
			if(match.hasMatch())
				{
					int matchnum=3;
					QString doacat="";
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
		tstr="if(dirstack.isEmpty()==false) QDir::setCurrent(dirstack.takeLast())";
	return(tstr);
}

QString commandsClass::makeExport(QString qline)
{
	QString					tstr="";
	QString					line=qline;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	parseFileClass			pfl;

	re.setPattern("^[[:space:]]*?(export)[[:space:]]+(.*)");
	match=re.match(line);
	if(match.hasMatch())
		{
			tstr=QString("setenv(\"%1\",%2.toStdString().c_str(),1)").arg(match.captured(2).trimmed()).arg("variables[\""+match.captured(2).trimmed()+"\"]");
		}
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
			tstr="dirstack<<QDir::currentPath();\nQDir::setCurrent("+tstr+")";
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
							tstr="variables["+newvar+"]="+tstr2;
						}
					return(tstr);
				}
			else
				{
					tstr=pfl.parseOutputString(match.captured(3).trimmed());
					if(match.captured(2).trimmed().endsWith("\\n\"")==true)//TODO//horrible hack!
						tstr+="<<Qt::endl";
					tstr="outop<<"+tstr;
					return(tstr);
				}
		}
	return("");
}

QString commandsClass::makeAssign(QString qline)
{
	QString			pal;
	parseFileClass	pfl;

	pfl.parseLine(qline);
	pal=pfl.parseExprString(false);
	pal=pal.replace(QRegularExpression("\\\\([[:alpha:]])"),"\\1");
	return(pal);
}

QString commandsClass::makeEcho(QString qline)
{
	QString					tstr;
	QString					line=qline;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	parseFileClass			pfl;

	pfl.preserveWhitespace=true;

	re.setPattern("[[:space:]]*echo[[:space:]]*(-.\\s*)?(-.?\\s*)?[[:space:]]*(.*)[[:space:]](>>?|\\|)(.*)");
	match=re.match(line);
	if(match.hasMatch())
		{
			tstr=pfl.parseOutputString(match.captured(3).trimmed());
			QString rep=tstr;
			QString outit=pfl.parseOutputString(match.captured(5).trimmed());
			QString whatit=match.captured(4).trimmed();
			QString endit="";
			if(whatit=="|")
				return("");
			if((match.captured(1).trimmed()!="-n") && (match.captured(2).trimmed()!="-n"))
				endit="\\n";
			if(whatit==">")
				whatit="w";
			else
				whatit="a";

			re.setPattern("QString\\(\"(.*)\"\\)(.*)");
			match=re.match(rep);
			if(match.hasMatch())
				{
					rep=match.captured(1);
					//rep.replace("\"","\\\"");//TODO//
					QString final="{\nFILE *fp = fopen("+outit+".toStdString().c_str(),\""+whatit+"\");\nfprintf(fp, \"%s"+endit.toStdString().c_str()+"\",QString(\""+rep+"\")"+match.captured(2)+".toStdString().c_str());";
					final+="\nfclose(fp);\n}";
					return(final);
				}
		}
	else
		{
			re.setPattern("[[:space:]]*echo[[:space:]]*(-.\\s*)?(-.?\\s*)?[[:space:]]*(.*)");
			match=re.match(line);
			if(match.hasMatch())
				{
					tstr=pfl.parseOutputString(match.captured(3).trimmed());
					if(isInFunction==true)
						{
							QString pal=pfl.parseExprString(false);

							tstr="if(capture==false)\n";
							if((match.captured(1).trimmed()=="-n") || (match.captured(2).trimmed()=="-n"))
								tstr+=QString("outop<<%1<<Qt::flush;\n").arg(pal);
							else
								tstr+=QString("outop<<%1<<Qt::endl;\n").arg(pal);
							tstr+="else\n";
							if((match.captured(1).trimmed()=="-n") || (match.captured(2).trimmed()=="-n"))
								tstr+="retstr+="+pal;
							else
								tstr+="retstr+="+pal+"+\"\\n\"";
						}
					else
						{
							if((match.captured(1).trimmed()=="-n") || (match.captured(2).trimmed()=="-n"))
								tstr="outop<<"+tstr+"<<Qt::flush";
							else
								tstr="outop<<"+tstr+"<<Qt::endl";
						}
					return(tstr);
				}
		}
	return("");
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
			data.replace(QRegularExpression("([^\\\\])\\$\\{?([[:alpha:][:alnum:]_]*)}?"),"\\1\"+variables[\"\\2\"]+\"");
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

	re.setPattern("[[:space:]]*(.*)[[:space:]]*\\)[[:space:]]*");
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
	else
		{
			re.setPattern("[[:space:]]*;;[[:space:]]*");
			match=re.match(qline);
			if(match.hasMatch())
				{
					return("}\n");
				}
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

	retstr=QString("variables[\""+tstr+"\"]=procsub(\"%1;echo ${"+tstr+"}\")").arg(pfl.lineToBashCLIString(qline));
	return(retstr);
}