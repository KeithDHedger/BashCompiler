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
	QString tstr;
	tstr=mainParseClass->lineToBashCLIString(line);
	return(QString("fflush(NULL);\nexitstatus=QString::number(WEXITSTATUS(system(QString(\"%1\").toStdString().c_str())) & 0xff);\n").arg(tstr));
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

//^(if)[[:space:]]*\[+[[:space:]]*(.*)[[:space:]]*(-lt)[[:space:]]*([^]]+)[^;]\]*;*(.*)
	re.setPattern("^(if)[[:space:]]*\\[+[[:space:]]*(.*)[[:space:]]*("+tstr+")[[:space:]]*([^]]+)[^;]\\]*;*(.*)");

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
			cCode<<"if("+leftstr+midstr+ritestr+")\n";
			if(singlelinethen==true)
				cCode<<"{\n";
			return(true);
		}
	else
		return(false);
	return(true);
}

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

//^(while)[[:space:]]*\[+[[:space:]]*(.*)[[:space:]]*(-lt)[[:space:]]*([^]]+)[^;]\]*;*(.*)
	re.setPattern("^(while)[[:space:]]*\\[+[[:space:]]*(.*)[[:space:]]*("+tstr+")[[:space:]]*([^]]+)[^;]\\]*;*(.*)");
	match=re.match(line);
	if((match.hasMatch()) && (match.captured(1).trimmed().compare("while")==0))
		{
			mainParseClass->bashCommand=BASHIF;
			if(match.captured(5).trimmed().compare("do")==0)
				singlelinethen=true;

			midstr=match.captured(3).trimmed();
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
			cCode<<"while("+leftstr+midstr+ritestr+")\n";
			if(singlelinethen==true)
				cCode<<"{\n";
			return(true);
		}
	else
		return(false);
	return(true);
}
