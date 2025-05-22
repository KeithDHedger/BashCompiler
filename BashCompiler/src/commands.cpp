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

QString commandsClass::makeExternalCommand(QString line)
{
	QString tstr=line;
	tstr.replace("\"","\\\"");
	return(QString("fflush(NULL);\nexitstatus=QString::number(WEXITSTATUS(system(\"%1\")) & 0xff);\n").arg(tstr));
}

#if 1
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

	line=qline;
	mainParseClass->lineParts.clear();
//^(if)[[:space:]]*\[+[[:space:]]*(.*)[[:space:]]*(-eq)[[:space:]]*([^\]]*).*(then)$
	while(bashmath[cnt]+=NULL)
		tstr+=bashmath[cnt++]+QString("|");
	tstr=tstr.left(tstr.length()-1);
	//errop<<"^(if)[[:space:]]*\\[+[[:space:]]*(.*)[[:space:]]*("+tstr+")[[:space:]]*([^\\]]*).*(then)$\n";
	re.setPattern("^(if)[[:space:]]*\\[+[[:space:]]*(.*)[[:space:]]*("+tstr+")[[:space:]]*([^\\]]*).*(then)$");
	match=re.match(line);
	if((match.hasMatch()) && (match.captured(1).trimmed().compare("if")==0))
		{
			if(match.captured(5).trimmed().compare("then")==0)
				singlelinethen=true;
			//QString	expr=match.captured(1).replace(QRegularExpression("\\[+|\\]+"),0).trimmed();
			//expr=match.captured(2).trimmed();
			//tstr="";

			//tstr=tstr.left(tstr.length()-1);
			//tstr="\\s*(.*)\\s+("+tstr+")\\s\\s*(.*)\\s*";
			
			//errop<<">>>>"<<match.captured(2).trimmed()<<"<<<<<\n";
			mainParseClass->bashCommand=BASHDONE;
			mainParseClass->parseLine(match.captured(2).trimmed());
			mainParseClass->bashCommand=BASHDONE;
			leftstr=mainParseClass->parseExprString(donumexpr);
			mainParseClass->parseLine(match.captured(4).trimmed());
			ritestr=mainParseClass->parseExprString(donumexpr);
qDebug()<<leftstr<<"----"<<ritestr;
			exit(100);
		}
	else
		return(false);
	return(true);	
}
#else
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

	line=qline;
	re.setPattern("(if)\\s+\\[+(.*)\\s+\\]+\\s*;*\\s*(.*)");
	match=re.match(line);
	if((match.hasMatch()) && (match.captured(1).trimmed().compare("if")==0))
		{
			if(match.captured(3).trimmed().compare("then")==0)
				singlelinethen=true;
			QString	expr=match.captured(1).replace(QRegularExpression("\\[+|\\]+"),0).trimmed();
			expr=match.captured(2).trimmed();
			tstr="";
			while(bashmath[cnt]+=NULL)
				tstr+=bashmath[cnt++]+QString("|");

			tstr=tstr.left(tstr.length()-1);
			tstr="\\s*(.*)\\s+("+tstr+")\\s\\s*(.*)\\s*";

			re.setPattern(tstr);
			match=re.match(expr);
			if(match.hasMatch())
				{
					leftstr=match.captured(1).trimmed();
					leftstr.replace(QRegularExpression("^\"|\"$"),0);
					leftstr.replace(QRegularExpression("^\"|\"$"),0);
					leftstr.replace(QRegularExpression("\\\""),"\\\"");
					midstr=match.captured(2).trimmed();
					ritestr=match.captured(3).trimmed();
					ritestr.replace(QRegularExpression("^\"|\"$"),0);
					ritestr.replace(QRegularExpression("\\\""),"\\\"");

					if(midstr.at(0)=='-')
						donumexpr=true;
					else
						donumexpr=false;
//parseExprString
					leftstr=mainParseClass->parseExprString(leftstr,donumexpr);
					ritestr=mainParseClass->parseExprString(ritestr,donumexpr);
					qDebug()<<"llllllllllllll="<<leftstr;
					qDebug()<<"rrrrrrrrrrrrrrrrr="<<ritestr;

					cnt=0;
					while(bashmath[cnt]+=NULL)
						{
							if(midstr.compare(bashmath[cnt])==0)
								midstr=cmath[cnt];
							cnt++;		
						}
					cCode<<"if("+leftstr+midstr+ritestr+")\n";
					if(singlelinethen==true)
						cCode<<"{\n";
					qDebug()<<">>>>>>>>>>>"<<"if("+leftstr+midstr+ritestr+")\n";
				}
			return(true);
		}
	return(false);
}
#endif
bool commandsClass::makeThen(QString line)
{
	QRegularExpression		re;
	QRegularExpressionMatch	match;

	re.setPattern("\\s*(then)");
	match=re.match(line);
	if(match.hasMatch())
		{
			cCode<<"{\n";
			return(true);
		}
	return(false);
}

bool commandsClass::makeElse(QString line)
{
	QRegularExpression		re;
	QRegularExpressionMatch	match;

	re.setPattern("\\s*(else)");
	match=re.match(line);
	if(match.hasMatch())
		{
			cCode<<"}\nelse\n{\n";
			return(true);
		}
	return(false);
}
bool commandsClass::makeFi(QString line)
{
	QRegularExpression		re;
	QRegularExpressionMatch	match;

	re.setPattern("\\s*(fi)");
	match=re.match(line);
	if(match.hasMatch())
		{
			cCode<<"}\n";
			return(true);
		}
	return(false);
}

bool commandsClass::makeWhile(QString line)
{
	QString					tstr;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	QString					leftstr;
	QString					midstr;
	QString					ritestr;
	int						cnt=0;
	bool						donumexpr=true;
	bool						singlelinethen=false;

	re.setPattern("(while)\\s+\\[+(.*)\\s+\\]+\\s*;*\\s*(.*)");
	match=re.match(line);
	if((match.hasMatch()) && (match.captured(1).trimmed().compare("while")==0))
		{
			if(match.captured(3).trimmed().compare("do")==0)
				singlelinethen=true;
			QString	expr=match.captured(1).replace(QRegularExpression("\\[+|\\]+"),0).trimmed();
			expr=match.captured(2).trimmed();
			tstr="";
			while(bashmath[cnt]+=NULL)
				tstr+=bashmath[cnt++]+QString("|");

			tstr=tstr.left(tstr.length()-1);
			tstr="\\s*(.*)\\s+("+tstr+")\\s\\s*(.*)\\s*";

			re.setPattern(tstr);
			match=re.match(expr);
			if(match.hasMatch())
				{
					leftstr=match.captured(1).trimmed();
					leftstr.replace(QRegularExpression("^\"|\"$"),0);
					leftstr.replace(QRegularExpression("^\"|\"$"),0);
					leftstr.replace(QRegularExpression("\\\""),"\\\"");
					midstr=match.captured(2).trimmed();
					ritestr=match.captured(3).trimmed();
					ritestr.replace(QRegularExpression("^\"|\"$"),0);
					ritestr.replace(QRegularExpression("\\\""),"\\\"");

					if(midstr.at(0)=='-')
						donumexpr=true;
					else
						donumexpr=false;
//parseExprString
					//leftstr=mainParseClass->parseExprString(leftstr,donumexpr);
					leftstr=mainParseClass->parseExprString(donumexpr);
					//ritestr=mainParseClass->parseExprString(ritestr,donumexpr);
					ritestr=mainParseClass->parseExprString(donumexpr);

					cnt=0;
					while(bashmath[cnt]+=NULL)
						{
							if(midstr.compare(bashmath[cnt])==0)
								midstr=cmath[cnt];
							cnt++;		
						}
					cCode<<"while("+leftstr+midstr+ritestr+")\n";
					if(singlelinethen==true)
						cCode<<"{\n";
				}
			return(true);
		}
	return(false);
}