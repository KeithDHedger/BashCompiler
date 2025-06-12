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

#if 0
QString commandsClass::makeExternalCommand(QString line)
{
	QString tstr;
	if(line.isEmpty()==true)
		return("");
	tstr=line;
	parseFileClass			pfl;
QString xx;
	pfl.noCodeOP=true;
	pfl.preserveWhitespace=true;
	pfl.quotevars=true;
tstr=pfl.parseOutputString(line);
	errop<<">>"<<tstr<<"\n"<<line<<"<<"<<Qt::endl;

	xx="exitstatus=QString::number(WEXITSTATUS(system("+tstr+".toStdString().c_str())) & 0xff);";
	errop<<xx<<Qt::endl;
	return(xx);

	tstr=line;

//QString xx="dirname $(dirname $(dirname '"+variables["APATH"]+"'))";
//	//exitstatus=QString::number(WEXITSTATUS(system("dirname $(dirname $(dirname '"variables[$ {APATH}].toStdString().c_str()"'))")) & 0xff);
//	exitstatus=QString::number(WEXITSTATUS(system(xx.toStdString().c_str())) & 0xff);

//	exitstatus=QString::number(WEXITSTATUS(system(QString("basename \"/tmp/a \\\"test\\\" file\" .txt").toStdString().c_str())) & 0xff);


	tstr.replace(QRegularExpression("\\$\\{(.*)\\}"),"'\"+variables[\"\\1\"]+\"'");
	//tstr=
	errop<<tstr<<Qt::endl;
	errop<<tstr.toStdString().c_str()<<Qt::endl;
//QString xx;
	xx="exitstatus=QString::number(WEXITSTATUS(system(QString(\""+tstr+"\").toStdString().c_str())) & 0xff);";
	//exit(100);
//tstr=QString("fflush(NULL);\nexitstatus=QString::number(WEXITSTATUS(system("+xx+".toStdString().c_str()\")) & 0xff);\n");
//	errop<<tstr<<Qt::endl;
	errop<<xx<<Qt::endl;
	return(xx);
}
#else
QString commandsClass::makeExternalCommand(QString line)
{
	QString tstr;
	if(line.isEmpty()==true)
		return("");
	//tstr=mainParseClass->lineToBashCLIString(line);
	//errop<<">>"<<tstr<<"<<"<<Qt::endl;

	parseFileClass			pfl;

	pfl.noCodeOP=true;
	pfl.preserveWhitespace=true;
	pfl.quotevars=true;
tstr=pfl.parseOutputString(line);
	errop<<">>"<<tstr<<"<<"<<line<<"<<"<<Qt::endl;

//exit(100);
//tstr.replace(QRegularExpression(".arg\\((QString\\(variables\\[\".*\"\\]\\).*)"),"\"'\"+.arg(\\1+\"'\"");

errop<<">>>>>>>>>>"<<tstr<<"<<<<<<<<<<<<"<<Qt::endl;

//exitstatus=QString::number(WEXITSTATUS(system(QString(QString("./photopagemaker -i %1").arg(QString("%1").arg(QString(variables["input_folder"]).replace(QRegularExpression(QString("%1").arg("^(.*)/.*")),"\\1")))).toStdString().c_str())));

//exitstatus=QString::number(WEXITSTATUS(system(QString(QString("./photopagemaker -i %1").arg(QString("%1").arg(QString(variables["input_folder"]).replace(QRegularExpression(QString("%1").arg("^(.*)/.*")),"\\1")))).toStdString().c_str())));

QString xx=tstr;
tstr=QString("fflush(NULL);\nexitstatus=QString::number(WEXITSTATUS(system("+xx+".toStdString().c_str())) & 0xff);\n");

//exitstatus=QString::number(WEXITSTATUS(system(QString(\"%1\").toStdString().c_str())) & 0xff);\n").arg(xx))
	if(tstr.isEmpty()==false)
//		return(QString("fflush(NULL);\nexitstatus=QString::number(WEXITSTATUS(system(QString(\"%1\").toStdString().c_str())) & 0xff);\n").arg(tstr));
		return(tstr);
	return("");
}
#endif
bool commandsClass::makeFunction(QString qline)
{
	QString					line=qline;
	QRegularExpression		re;
	QRegularExpressionMatch	match;

	re.setPattern("^[[:space:]]*([[:alnum:]_]+)[[:space:]]*\\([[:space:]]*\\)[[:space:]]*$");
	match=re.match(line);
	if(match.hasMatch())
		{
			isInFunction=true;
			fCode<<"QString "<<match.captured(1).trimmed()<<"(bool capture,QHash<QString, QString> fv)\n{\nQString retstr;\n";
			functionNames<<match.captured(1).trimmed();
		}
	else
		return(false);
	return(true);

}

bool commandsClass::makeIf(QString qline)
{
	QString					tstr;
	QString					line;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	QString					leftstr;
	QString					midstr;
	QString					ritestr;
	int						cnt=0;
	bool						donumexpr=true;
	bool						singlelinethen=false;
	parseFileClass			pfl;
	parseFileClass			pfr;

	pfl.noCodeOP=true;
	pfr.noCodeOP=true;
	line=qline;
	while(bashmath[cnt]+=NULL)
		tstr+=bashmath[cnt++]+QString("|");
	tstr=tstr.left(tstr.length()-1);

	re.setPattern("^[[:space:]]*(if)[[:space:]]*\\[+[[:space:]]*(.*)[[:space:]]*("+tstr+")[[:space:]]*([^]]+)[^;]\\]*;*(.*)");

	match=re.match(line);
	if((match.hasMatch()) && (match.captured(1).trimmed().compare("if")==0))
		{
			mainParseClass->bashCommand=BASHIF;
			if(match.captured(5).trimmed().compare("then")==0)
				singlelinethen=true;

			midstr=match.captured(3).trimmed();
			if(midstr.at(0)=='-')
				donumexpr=true;
			else
				donumexpr=false;
	
			tstr=match.captured(2).trimmed();
			pfl.parseLine(tstr);
			//leftstr=pfl.parseExprString(donumexpr);
			leftstr=pfl.parseExprStringNew(donumexpr);
			tstr=match.captured(4).trimmed();
			pfr.parseLine(tstr);
			//ritestr=pfr.parseExprString(donumexpr);
			ritestr=pfr.parseExprStringNew(donumexpr);

			cnt=0;
			while(bashmath[cnt]+=NULL)
				{
					if(midstr.compare(bashmath[cnt])==0)
						midstr=cmath[cnt];
					cnt++;		
				}
			if(isInFunction==true)
				fCode<<"if("+leftstr+midstr+ritestr+")\n";
			else
				cCode<<"if("+leftstr+midstr+ritestr+")\n";
			if(singlelinethen==true)
				{
					if(isInFunction==true)
						fCode<<"{\n";
					else
						cCode<<"{\n";
				}
			return(true);
		}
	else
		return(false);
	return(true);
}

QString commandsClass::makeIfNew(QString qline)
{
	QString					tstr;
	//QString					line;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	QString					leftstr;
	QString					midstr;
	QString					ritestr;
	QString					retstr="";
	int						cnt=0;
	bool						donumexpr=true;
	parseFileClass			pfl;
	parseFileClass			pfr;

	pfl.noCodeOP=true;
	pfr.noCodeOP=true;
	//line=qline;
	while(bashmath[cnt]+=NULL)
		tstr+=bashmath[cnt++]+QString("|");
	tstr=tstr.left(tstr.length()-1);

	re.setPattern("^[[:space:]]*if[[:space:]]*\\[+[[:space:]]*(.*)[[:space:]]*("+tstr+")[[:space:]]*([^]]+)[^;]\\]*");
	match=re.match(qline);
	if(match.hasMatch())
		{
			midstr=match.captured(2).trimmed();
			if(midstr.at(0)=='-')
				donumexpr=true;
			else
				donumexpr=false;
	
			tstr=match.captured(1).trimmed();
			pfl.parseLine(tstr);
			leftstr=pfl.parseExprStringNew(donumexpr);
			tstr=match.captured(3).trimmed();
			pfr.parseLine(tstr);
			ritestr=pfr.parseExprStringNew(donumexpr);
			cnt=0;
			while(bashmath[cnt]+=NULL)
				{
					if(midstr.compare(bashmath[cnt])==0)
						midstr=cmath[cnt];
					cnt++;		
				}
			retstr="if("+leftstr+midstr+ritestr+")\n";
		}

	return(retstr);
}

bool commandsClass::makeThen(QString line)
{
	QRegularExpression		re;
	QRegularExpressionMatch	match;

	re.setPattern("^[[:space:]]*(then)[[:space:]]*$");
	match=re.match(line);
	if(match.hasMatch())
		{
			if(isInFunction==true)
				fCode<<"{\n";
			else
				cCode<<"{\n";
			return(true);
		}
	return(false);
}

bool commandsClass::makeElse(QString line)
{
	QRegularExpression		re;
	QRegularExpressionMatch	match;

	re.setPattern("^[[:space:]]*(else)[[:space:]]*$");
	match=re.match(line);
	if(match.hasMatch())
		{
			if(isInFunction==true)
				fCode<<"}\nelse\n{\n";
			else
				cCode<<"}\nelse\n{\n";
			return(true);
		}
	return(false);
}
bool commandsClass::makeFi(QString line)
{
	QRegularExpression		re;
	QRegularExpressionMatch	match;

	re.setPattern("^[[:space:]]*(fi)[[:space:]]*$");
	match=re.match(line);
	if(match.hasMatch())
		{
			if(isInFunction==true)
				fCode<<"}\n";
			else
				cCode<<"}\n";
			return(true);
		}
	return(false);
}

bool commandsClass::makeWhileRead(QString qline)
{
	QString					line=qline;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	QString					commstr;
	QString					midstr;
	QString					ritestr;

	re.setPattern("^[[:space:]]*(while[[:space:]]*read)[[:space:]]*(.*)$");
	match=re.match(line);
	if((match.hasMatch()) && (match.captured(1).simplified().compare("while read")==0))
		{
			if(isInFunction==true)
				{
					fCode<<"{\nFILE *fp;\n";
					mainParseClass->whileReadLine.append(fCode.size());
					fCode<<"fp=popen(proc.toStdString().c_str(),\"r\");\n";
					fCode<<"QTextStream	inpop(fp);\n";
					if(match.captured(2).trimmed().isEmpty()==true)
						fCode<<"while(inpop.readLineInto(&variables[\"REPLY\"]))\n";
					else
						fCode<<"while(inpop.readLineInto(&variables[\""+match.captured(2).trimmed()+"\"]))\n";
				}
			else
				{
					cCode<<"{\nFILE *fp;\n";
					mainParseClass->whileReadLine.append(cCode.size());
					cCode<<"fp=popen(proc.toStdString().c_str(),\"r\");\n";
					cCode<<"QTextStream	inpop(fp);\n";
					if(match.captured(2).trimmed().isEmpty()==true)
						cCode<<"while(inpop.readLineInto(&variables[\"REPLY\"]))\n";
					else
						cCode<<"while(inpop.readLineInto(&variables[\""+match.captured(2).trimmed()+"\"]))\n";
				}
			return(true);
		}
	else
		return(false);
	return(true);
}

bool commandsClass::makeWhile(QString qline)
{
	QString					tstr;
	QString					line;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	QString					leftstr;
	QString					midstr;
	QString					ritestr;
	int						cnt=0;
	bool						donumexpr=true;
	bool						singlelinethen=false;
	parseFileClass			pfl;
	parseFileClass			pfr;

	pfl.noCodeOP=true;
	pfr.noCodeOP=true;
	line=qline;
	while(bashmath[cnt]+=NULL)
		tstr+=bashmath[cnt++]+QString("|");
	tstr=tstr.left(tstr.length()-1);

	re.setPattern("^[[:space:]]*(while)[[:space:]]*\\[+[[:space:]]*(.*)[[:space:]]*("+tstr+")[[:space:]]*([^]]+)[^;]\\]*;*(.*)");
	match=re.match(line);
	if((match.hasMatch()) && (match.captured(1).trimmed().compare("while")==0))
		{
			mainParseClass->bashCommand=BASHWHILE;
			if(match.captured(5).trimmed().compare("do")==0)
				singlelinethen=true;

			midstr=match.captured(3).trimmed();
			if(midstr.at(0)=='-')
				donumexpr=true;
			else
				donumexpr=false;
	
			tstr=match.captured(2).trimmed();
			pfl.parseLine(tstr);
			//leftstr=pfl.parseExprString(donumexpr);
			leftstr=pfl.parseExprStringNew(donumexpr);
			tstr=match.captured(4).trimmed();
			pfr.parseLine(tstr);
			//ritestr=pfr.parseExprString(donumexpr);
			ritestr=pfr.parseExprStringNew(donumexpr);

			cnt=0;
			while(bashmath[cnt]+=NULL)
				{
					if(midstr.compare(bashmath[cnt])==0)
						midstr=cmath[cnt];
					cnt++;		
				}
			if(isInFunction==true)
				fCode<<"while("+leftstr+midstr+ritestr+")\n";
			else
				cCode<<"while("+leftstr+midstr+ritestr+")\n";
			if(singlelinethen==true)
				{
					if(isInFunction==true)
						fCode<<"{\n";//}
					else
						cCode<<"{\n";//}
				}
			return(true);
		}
	else
		return(false);
	return(true);
}

bool commandsClass::makePopd(QString qline)
{
	QString					tstr;
	QString					line=qline;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	parseFileClass			pfl;
	re.setPattern("^[[:space:]]*?(popd)[[:space:]]*.*");
	match=re.match(line);
	if(match.hasMatch())
		{
			if(isInFunction==true)
				{
					fCode<<"if(dirstack.isEmpty()==false) QDir::setCurrent(dirstack.takeLast());\n";
				}
			else
				{
					cCode<<"if(dirstack.isEmpty()==false) QDir::setCurrent(dirstack.takeLast());\n";
				}			
		}
	else
		return(false);
	return(true);}

bool commandsClass::makePushd(QString qline)
{
	QString					tstr;
	QString					line=qline;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	parseFileClass			pfl;

	pfl.noCodeOP=true;
pfl.quotevars=true;

	re.setPattern("^[[:space:]]*?(pushd)[[:space:]]+(.*)");
	match=re.match(line);
	if(match.hasMatch())
		{
			tstr=match.captured(2).trimmed();
			tstr.remove(QRegularExpression("[[:space:]&123][[:space:]]*>[[:space:]]*.*$"));
			pfl.parseLine(tstr);
			//tstr=pfl.parseExprString(false);
			tstr=pfl.parseExprStringNew(false);
			if(isInFunction==true)
				{
					fCode<<"dirstack<<QDir::currentPath();\n";
					fCode<<"QDir::setCurrent("+tstr+");\n";
				}
			else
				{
					cCode<<"dirstack<<QDir::currentPath();\n";
					cCode<<"QDir::setCurrent("+tstr+");\n";
				}
		}
	else
		return(false);

	return(true);
}

bool commandsClass::makePrintf(QString qline)
{
	QString					tstr;
	QString					tstr2;
	QString					tstr3;
	QString					line=qline;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	parseFileClass			pfl;

	pfl.noCodeOP=true;
	pfl.preserveWhitespace=true;
	pfl.quotevars=true;
	re.setPattern("[[:space:]]*printf[[:space:]]*(-v)?[[:space:]]*(\\$*\\{*.*?)[[:space:]]+(\\$*\\{*.*\\}*)?");
	match=re.match(line);
	if(match.hasMatch())
		{
			if(match.captured(1).trimmed().compare("-v")==0)
				{
					tstr2=pfl.parseOutputString(match.captured(2).trimmed());
					tstr3=pfl.parseOutputString(match.captured(3).trimmed());
					tstr="variables["+tstr2+"]="+tstr3+";\n";
					
					if(isInFunction==true)
						fCode<<tstr;
					else
						cCode<<tstr;
					return(true);
				}
			else
				{
					errop<<"----"<<tstr<<Qt::endl;
					tstr=pfl.parseOutputString(match.captured(3).trimmed());
					errop<<"0000"<<tstr<<Qt::endl;
					tstr="outop<<"+tstr+";\n";
					if(isInFunction==true)
						fCode<<tstr;
					else
						cCode<<tstr;
					return(true);
				}
		}
	else
		return(false);

	return(true);
}


QString commandsClass::makePrintfNew(QString qline)
{
	QString					tstr;
	QString					tstr2;
	QString					tstr3;
	QString					line=qline;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	parseFileClass			pfl;

	pfl.noCodeOP=true;
	pfl.preserveWhitespace=true;
	pfl.quotevars=true;
	re.setPattern("[[:space:]]*printf[[:space:]]*(-v)?[[:space:]]*(\"[^\\\"]*\")?[[:space:]]*(.*)");
	match=re.match(line);
	if(match.hasMatch())
		{
			if(match.captured(1).trimmed().compare("-v")==0)
				{
					re.setPattern("[[:space:]]*([[:alnum:]_]+)[[:space:]]+(.*)");
					match=re.match(match.captured(3).trimmed());
					if(match.hasMatch())
						{
							tstr2=pfl.parseOutputStringNew(match.captured(1).trimmed());
							tstr3=pfl.parseOutputStringNew(match.captured(2).trimmed());
							tstr="variables["+tstr2+"]="+tstr3;
						}
					return(tstr);
				}
			else
				{
					tstr=pfl.parseOutputStringNew(match.captured(3).trimmed());
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
	QString pal;
	parseFileClass	pfl;
	pfl.parseLine(qline);
	pal=pfl.parseExprStringNew(false);
	return(pal);
}

QString commandsClass::makeEcho(QString qline)
{
	QString					tstr;
	QString					line=qline;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	parseFileClass			pfl;

	pfl.noCodeOP=true;
	pfl.preserveWhitespace=true;
	pfl.quotevars=true;
	re.setPattern("[[:space:]]*echo[[:space:]]*(-n)?[[:space:]]*(.*)");
	match=re.match(line);
	if(match.hasMatch())
		{
			tstr=pfl.parseOutputStringNew(match.captured(2).trimmed());
			if(match.captured(1).trimmed()=="-n")
				tstr="outop<<"+tstr;
			else
				tstr="outop<<"+tstr+"<<Qt::endl";
			return(tstr);
		}

	return("");
}