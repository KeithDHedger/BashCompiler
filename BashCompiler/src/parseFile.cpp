/*
 *
 * ©K. D. Hedger. Sat 19 Apr 14:07:25 BST 2025 keithdhedger@gmail.com

 * This file (parseFile.cpp) is part of BashCompiler.

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

#include "parseFile.h"

parseFileClass::~parseFileClass()
{
	if(this->mainBashFile.isOpen()==true)
		this->mainBashFile.close();
}

parseFileClass::parseFileClass(QString filepath)
{
	if(filepath.isEmpty()==false)
		{
			this->mainBashFile.setFileName(filepath);

			if(!this->mainBashFile.open(QIODevice::ReadOnly | QIODevice::Text))
		         return;
		}
}

void parseFileClass::parseFile(void)
{
	QString line;

	while (!this->mainBashFile.atEnd())
		{
			line=this->mainBashFile.readLine().trimmed();
			if(this->parseLine(line)==false)
				{
					qDebug()<<"Error parsing line:\n"<<line;
					return;
				}
		}
}

bool parseFileClass::parseLine(QString qline)
{
	QString					tstr;
	QString					line;
	QRegularExpression		re;
	QRegularExpressionMatch	match;

	line=qline;
	currentLine++;

	if(this->verboseCompile==true)
		qDebug()<<"processing line "<<this->currentLine<<line;
	
	if(line.startsWith('#'))
		{
			if(this->verboseCCode==true)
				cCode<<"//"+line.remove(0,1)+"\n";
			return(true);
		}
	if(line.length()==0)
		{
			return(true);
		}

	if(this->verboseCCode==true)
		cCode<<QString("//%1\n").arg(line);

	this->linePosition=0;
	this->lineParts.clear();
	this->currentPart="";

	this->preserveWhitespace=false;
	this->bashCommand=EXTERNALCOMMAND;

//check for assign
	re.setPattern("^([[:alnum:]_]+)=(.*)");
	match=re.match(line);
	if(match.hasMatch())
		{
			this->bashCommand=BASHASSIGN;
			this->preserveWhitespace=true;
			line=match.captured(2).replace(QRegularExpression("^\"|\"$"),0);
		}
	else
		{
//check for echo
			re.setPattern("^([[:alnum:]]+)[[:space:]]+(.*)");
			match=re.match(line);
			if(match.hasMatch())
				{
					if(match.captured(1)=="echo")
						{
							this->bashCommand=BASHECHO;
							this->preserveWhitespace=true;
							line=match.captured(2).replace(QRegularExpression("^\"|\"$"),0);
						}
				}
		}

	while(this->linePosition<line.length())
		{
			if(line.at(this->linePosition).toLatin1()=='"')
				{
					this->parseString(line);
					continue;
				}
			if(line.at(this->linePosition).toLatin1()=='$')
				{
					this->parseDollar(line);
					continue;
				}
					
			if((line.at(this->linePosition).isSpace()==true) && (this->preserveWhitespace==false))
				{
					this->parseWhitespace(line);
					continue;
				}

			if(line.at(this->linePosition).toLatin1()=='[')
				{
					this->parseSquareBraces(line);
					continue;
				}

			this->currentPart+=line.at(this->linePosition);
			this->linePosition++;
		}

	if(this->currentPart.isEmpty()==false)
		this->lineParts.append({this->currentPart,STRINGDATA});

	//for(int j=0;j<lineParts.size();j++)
	//	QTextStream(stderr)<<"-->>"<<lineParts.at(j).data<<"<< "<<typeText[lineParts.at(j).typeHint]<<"<<--\n";

	switch(this->bashCommand)
		{
			case BASHASSIGN:
				{
					this->cFileDeclares<<QString("QString %1;").arg(match.captured(1));
					QString pal=this->parseExprString(false);
					cCode<<QString("%1=%2;\n").arg(match.captured(1)).arg(pal);
					return(true);
				}
				break;
			case BASHECHO:
				{
					QString pal=this->parseExprString(false);
					cCode<<QString("outop<<%1<<Qt::endl;\n").arg(pal);
					return(true);
				}
				break;
//			case BASHIF:
//			case BASHELSE:
//			case BASHFI:
//			qDebug()<<"this->createCommand(line)";
//				return(true);
//				break;

			case BASHDONE:
				return(true);
				break;

			default:
				this->createCommand(line);
//external command
				if(this->bashCommand==EXTERNALCOMMAND)
					{
						if(this->noCodeOP==false)
							cCode<<mainCommandsClass->makeExternalCommand(line);
						return(true);
					}
				else
					return(true);
				break;
		}
//should never get here!
	return(false);
}

QString parseFileClass::cleanVar(QString line)
{
	QRegularExpression		re("([[:alnum:] \"]*)|\\$\\{*([[:alnum:]]*)}*");
	QRegularExpressionMatch	match;
	QString					retval=line;

	re.setPattern("^\\$\\{*([[:alnum:]#_]*)}*");
	match=re.match(line);
	if(match.hasMatch())
		{
			retval=match.captured(1).trimmed();
			return(retval);
	}

	re.setPattern("^(\\\"[^\"]*\\\")");
	match=re.match(line);
	if(match.hasMatch())
		{
			retval=match.captured(1).trimmed();
			return(retval);
		}
	return(retval);
}

QString parseFileClass::parseVar(QString line)
{
	QString varname;
	QString haystack;
	QString needle;
	QString retcode="";
	QRegularExpression re;
	QRegularExpressionMatch match;

//check for special vars 1st
	retcode=setSpecialDollars(line.at(1));
	if(retcode.isEmpty()==false)
		return(retcode);
//${string#substring}
//${string##substring}
//${string%substring}
//${string%%substring}

	re.setPattern("^\\$\\{[[:alnum:]_\"]+([/#%:]{1,2}).*}");
	match=re.match(line);
	if(match.hasMatch())
		{
//${string:position}/${string:position:length}
			if(match.captured(1).trimmed()==":")
				{//{
//${string:position:length}
					re.setPattern("\\${([[:alnum:]_]+):([[:alnum:]_\\$\"\\{\\}]+):([[:alnum:]_\\$\"\\{\\}]+)}");
					match=re.match(line);
					if(match.hasMatch())
						{
							varname=match.captured(1).trimmed();
							haystack=this->cleanVar(match.captured(2).trimmed());
							needle=this->cleanVar(match.captured(3).trimmed());
							retcode="QString(\"%1\").arg(QString("+varname+").mid(QString(\"%1\").arg("+haystack+").toInt(nullptr,0),QString(\"%1\").arg("+needle+").toInt(nullptr,0)))";
							return(retcode);
						}
					else
						{//{
//${string:position}
							re.setPattern("([[:alnum:]_]*):(.*)}");
							match=re.match(line);
							if(match.hasMatch())
								{
									varname=match.captured(1).trimmed();
									needle=this->cleanVar(match.captured(2).trimmed());
									retcode="QString(\"%1\").arg(QString("+varname+").mid(QString(\"%1\").arg("+needle+").toInt(nullptr,0)))";
									return(retcode);
								}
						}
				}

//${string/substring/replacement}
//${string//substring/replacement}
			if(match.captured(1).trimmed()=="//")
				{//{
					re.setPattern("([[:alnum:]_]*)/+(.*)/(.*)\\}");
					match=re.match(line);
					if(match.hasMatch())
						{
							varname=match.captured(1).trimmed();
							needle=this->cleanVar(match.captured(3).trimmed());
							haystack=this->cleanVar(match.captured(2).trimmed());
							retcode="QString("+varname+").replace("+haystack+","+needle+")";
							return(retcode);
						}
				}

			if(match.captured(1).trimmed()=="/")
				{//{
					re.setPattern("([[:alnum:]_]*)/(.*)/(.*)\\}");
					match=re.match(line);
					if(match.hasMatch())
						{
							varname=match.captured(1).trimmed();
							needle=this->cleanVar(match.captured(3).trimmed());
							haystack=this->cleanVar(match.captured(2).trimmed());

							retcode=QString("QString(%1).remove(%1.indexOf(%2,0),QString(%2).length()).insert(%1.indexOf(%2,0),%3)").arg(varname).arg(haystack).arg(needle); 
							return(retcode);
						}
				}
		}
	else
		{
			re.setPattern("\\$\\{*#([[:alnum:]_]*)");//}
			match=re.match(line);
			if(match.hasMatch())
				{
					varname=match.captured(1).trimmed();
					retcode=varname+".length()";
					return(retcode);
				}
		}

	retcode=line;
	retcode.replace(QRegularExpression("^\\$\\{*|\\}*$"),0);
	return(retcode);
}

QString parseFileClass::lineToBashCLIString(QString qline)
{
	QString	newline=qline;
	QString	tstr="";

	newline.replace("\\\"","\\\\\\\"");
	for(int j=0;j<newline.length();j++)
		{
			if((newline.at(j).toLatin1()=='"')&& (newline.at(j-1).toLatin1()!='\\'))
				tstr+="\\";	
			tstr+=newline.at(j).toLatin1();
		}
	tstr.replace(QRegularExpression("\\$\\{*([[:alnum:]_]+)\\}"),"\"+\\1+\"");

	return(tstr);
}

#if 0
QString parseFileClass::lineToBashCLIString(QString qline)
{
	QString	newline=qline;
	QString	tstr="";

	newline.replace("\\\"","\\\\\\\"");
	newline.replace(QRegularExpression("\\$\\{*([[:alnum:]_]+)\\}"),"\"+\\1+\"");//BAD HACK//TODO//
	for(int j=0;j<newline.length();j++)
		{
			if((newline.at(j).toLatin1()=='"')&& (newline.at(j-1).toLatin1()!='\\'))
				tstr+="\\";	
			tstr+=newline.at(j).toLatin1();
		}

	tstr.replace("IN/"," '\"");
	tstr.replace("OUT/","\"' ");

	return(tstr);
}
QString parseFileClass::lineToBashCLIString(QString qline)
{
	QString	newline=qline;
	QString	tstr="";

	newline.replace("\\\"","\\\\\\\"");
	//newline.replace(QRegularExpression("\"*\\$\\{*([[:alnum:]_]+)\\}\"*"),"IN/+\\1+OUT/");//BAD HACK//TODO//
	//newline.replace(QRegularExpression("\\$\\{*([[:alnum:]_]+)\\}"),"\"+\\1+\"");//BAD HACK//TODO//
	for(int j=0;j<newline.length();j++)
		{
			if((newline.at(j).toLatin1()=='"')&& (newline.at(j-1).toLatin1()!='\\'))
				tstr+="\\";	
			tstr+=newline.at(j).toLatin1();
		}

	tstr.replace("IN/"," \"");
	tstr.replace("OUT/","\" ");
	tstr.replace(QRegularExpression("\\$\\{*([[:alnum:]_]+)\\}"),"\"+\\1+\"");//BAD HACK//TODO//

	return(tstr);
}
#endif

QString parseFileClass::parseExprString(bool isnumexpr)
{
	QStringList	parts;
	QString		complete="";
	QString		tstr;
	bool			isnumber=isnumexpr;

	for(int j=0;j<this->lineParts.count();j++)
		{
			switch(this->lineParts.at(j).typeHint)
				{
					case STRINGDATA:
						parts<<QString("\"%1\"").arg(this->lineParts.at(j).data);
						break;
					case DOUBLEQUOTESTRING:
						parts<<this->lineParts.at(j).data;
						break;
					case VARIABLEINCURLYS:
						parts<<this->parseVar(this->lineParts.at(j).data);
						break;
					case BRACKETS:
						if((this->lineParts.at(j).data.at(2)=='('))
							{
								tstr=this->lineParts.at(j).data;
								tstr.replace(QRegularExpression("^\\$\\(*|\\)*$"),"");
								tstr.replace(QRegularExpression("\\$\\{*|\\}*"),"");
								tstr.replace(QRegularExpression("\\b([[:alpha:]][[:alnum:]_]*)\\b"),"\\1.toInt(nullptr,0)");
								tstr=tstr.trimmed();
								parts<<tstr;
								break;
							}
						tstr=this->lineParts.at(j).data;
						tstr.replace(QRegularExpression("^\\$\\(*|\\)*$"),"");
						tstr=this->lineToBashCLIString(tstr);
						if(isnumber==true)
							parts<<"procsub(\""+tstr+"\").toInt(nullptr,0)";
						else
							parts<<"procsub(\""+tstr+"\")";
						break;
					case VARIABLE:
						parts<<this->parseVar(this->lineParts.at(j).data);
						break;
					default:
						parts<<QString("\"%1\"").arg(this->lineParts.at(j).data);
						continue;
				}
		}

	QString outstr="";
	QString outfmt="QString(\"";
	for(int j=0;j<parts.count();j++)
		{
			outfmt+="%"+ QString::number(j+1);
			outstr+=".arg("+parts.at(j)+")";
		}
	outfmt+="\")";
	complete=outfmt+outstr;
	if(isnumber==true)
		complete="("+complete+").toInt(nullptr,0)";
	return(complete);
}

void parseFileClass::createCommand(QString line)
{
	QString					tstr;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	QString					leftstr;
	QString					midstr;
	QString					ritestr;

	this->bashCommand=EXTERNALCOMMAND;


//if then else fi
	if(mainCommandsClass->makeIf(line)==true)
		{
			this->bashCommand=BASHIF;
			return;
		}

	if(mainCommandsClass->makeThen(line)==true)
		{
			this->bashCommand=BASHTHEN;
			return;
		}
	if(mainCommandsClass->makeElse(line)==true)
		{
			this->bashCommand=BASHELSE;
			return;
		}
	if(mainCommandsClass->makeFi(line)==true)
		{
			this->bashCommand=BASHFI;
			return;
		}

//while do done
	if(mainCommandsClass->makeWhile(line)==true)
		{
			this->bashCommand=BASHWHILE;
			return;
		}
	re.setPattern("\\s*(do\\b)");
	match=re.match(line);
	if(match.hasMatch())
		{
			cCode<<"{\n";
			this->bashCommand=BASHDO;
			return;
		}
	re.setPattern("\\s*(done\\b)");
	match=re.match(line);
	if(match.hasMatch())
		{
			cCode<<"}\n";
			this->bashCommand=BASHDONE;
			return;
		}
}

QString parseFileClass::setSpecialDollars(QChar dollar)
{
	switch(dollar.toLatin1())
		{
			case '?':
				return("exitstatus");
				break;
			default:
				break;
		}
	return("");
}

void parseFileClass::parseString(QString line)
{
	QString	strdata="\"";

	this->linePosition++;

	if(this->currentPart.isEmpty()==false)
		{
			this->lineParts.append({this->currentPart,STRINGDATA});
			this->currentPart="";
		}

	while(this->linePosition<line.length())
		{
			if((line.at(this->linePosition).toLatin1()=='"') && (line.at(this->linePosition-1).toLatin1()=='\\'))
				{
					strdata+=line.at(this->linePosition++);
				}
			else if((line.at(this->linePosition).toLatin1()=='"') && (line.at(this->linePosition-1).toLatin1()!='\\'))
				break;

			strdata+=line.at(this->linePosition++);
		}
	strdata+="\"";
	this->lineParts.append({strdata,DOUBLEQUOTESTRING});
	this->linePosition++;
}

void parseFileClass::parseDollar(QString line)
{
	int		opencnt=1;
	QString	dollardata="";
	QChar	braceopen;
	QChar	braceclose;
	parseDataType	dtype=VARIABLE;

	if(this->currentPart.isEmpty()==false)
		{
			this->lineParts.append({this->currentPart,STRINGDATA});
			this->currentPart="";
		}

	if(line.at(this->linePosition+1).toLatin1()=='{')
		{
			braceopen='{';
			braceclose='}';
			dollardata="${";//}
			dtype=VARIABLEINCURLYS;
		}
	if(line.at(this->linePosition+1).toLatin1()=='(')
		{
			braceopen='(';
			braceclose=')';
			dollardata="$(";//)
			dtype=BRACKETS;
		}

//do curly braces
	if(line.at(this->linePosition+1).toLatin1()==braceopen)
		{
			this->linePosition+=2;
			while(this->linePosition<line.length())
				{
					if(line.at(this->linePosition).toLatin1()==braceopen)
						opencnt++;
					if(line.at(this->linePosition).toLatin1()==braceclose)
						opencnt--;
					if(opencnt==0)
						{
							dollardata+=line.at(this->linePosition);
							this->lineParts.append({dollardata,dtype});
							this->linePosition++;
							return;
						}
					dollardata+=line.at(this->linePosition);
					this->linePosition++;
				}
			this->lineParts.append({dollardata,dtype});
			this->linePosition++;
			return;
		}

	while(this->linePosition<line.length() && (line.at(this->linePosition).isSpace()==false))
		dollardata+=line.at(this->linePosition++);
	this->lineParts.append({dollardata,dtype});
}

void parseFileClass::parseWhitespace(QString line)
{
	if(this->currentPart.isEmpty()==false)
		{
			lineParts.append({this->currentPart,STRINGDATA});
			this->currentPart="";
		}
	while((this->linePosition<line.length()) && (line.at(this->linePosition).isSpace()==true))
		this->linePosition++;
}

void parseFileClass::parseSquareBraces(QString line)
{
	int		opencnt=1;
	QString	bracedata="[";

	this->linePosition++;
	if(this->currentPart.isEmpty()==false)
		{
			this->lineParts.append({this->currentPart,STRINGDATA});
			this->currentPart="";
		}
	while(this->linePosition<line.length())
		{
			if(line.at(this->linePosition).toLatin1()=='[')
				opencnt++;
			if(line.at(this->linePosition).toLatin1()==']')
				opencnt--;
			if(opencnt==0)
				{
					bracedata+=line.at(this->linePosition);
					this->lineParts.append({bracedata,SQUAREBRACKETS});
					this->linePosition++;
					return;
				}
			bracedata+=line.at(this->linePosition);
			this->linePosition++;
		}
}
