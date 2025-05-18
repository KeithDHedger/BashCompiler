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
	this->mainBashFile.close();
}

parseFileClass::parseFileClass(QString filepath)
{
	this->mainBashFile.setFileName(filepath);

	if(!this->mainBashFile.open(QIODevice::ReadOnly | QIODevice::Text))
         return;

	qDebug()<<"parseFileClass"<<this->mainBashFile.fileName();
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

bool parseFileClass::parseLine(QString line)
{
	QString					tstr;
	QRegularExpression		re;
	QRegularExpressionMatch	match;

	if(this->verboseCompile==true)
		qDebug()<<"processing "<<line;
	
	if(line.startsWith('#'))
		{
			if(this->verboseCCode==true)
				this->cFile+="//"+line.remove(0,1)+"\n";
			return(true);
		}
	if(line.length()==0)
		{
			return(true);
		}

	if(this->verboseCCode==true)
		this->cFile+=QString("//%1\n").arg(line);

//check for assign
	re.setPattern("^([[:alnum:]_]+)[[:space:]]*(=)(.*)");
	match=re.match(line);
	if(match.hasMatch())
		{
			this->cFileDeclares<<QString("QString %1;").arg(match.captured(1));
			tstr=match.captured(3).replace(QRegularExpression("^\"|\"$"),0);
			QString pal=this->parseExprString(tstr,false);
			this->cFile+=QString("%1=%2;\n").arg(match.captured(1)).arg(pal);
			return(true);
		}
	else
		{
			this->createCommand(line);
//external command
			if(this->bashCommand==EXTERNALCOMMAND)
				{
					this->cFile+=mainCommandsClass->makeExternalCommand(line);
					return(true);
				}
			else
				{
					return(true);
				}
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
	QString retcode;
	QRegularExpression re;
	QRegularExpressionMatch match;

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
//errop<<retcode<<"\n";
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

	return("");
}

QString parseFileClass::parseExprString(QString line,bool isnumexpr)
{
	QStringList	parts;
	QString		tstr;
	int			lastletter;
	QString		complete="";
	int			j=0;

	while(j<line.length())
		{
			if(line.at(j)!='$')
				{
					lastletter=line.indexOf('$',0);
					if(lastletter==-1)
						lastletter=line.length();
					if(isnumexpr==false)
						parts<<"\""+line.left(lastletter)+"\"";
					else
						{
							parts<<line.left(lastletter).remove(QRegularExpression("^\"|\"$"));
						}
					line=line.mid(lastletter);
					j=0;
					continue;
				}
			if(line.at(j)=="$")
				{
					if(this->setSpecialDollars(line.at(j+1)).isEmpty()==false)
						{
							parts<<"exitstatus";
							line=line.right(line.length()-2);
							j=0;
							continue;
						}

					if((line.at(j+1)=="(") && (line.at(j+2)=="("))
						{
							lastletter=line.indexOf("))");
							tstr=line.mid(j+3,lastletter-3);
							tstr.replace(QRegularExpression("\\$([[:alnum:]_{}]*)"),"\\1.toInt(nullptr,0)");
							tstr.remove(QRegularExpression("[{}]"));
							parts<<tstr;
							line=line.right(line.length()-lastletter-2);
							j=0;
							continue;
						}
					else if(line.at(j+1)=="(")
						{
							lastletter=line.indexOf(")");
							tstr=line.mid(j+2,lastletter-2);
							tstr.replace("\"","\\\"");
							line=line.right(line.length()-lastletter-1);

							if(isnumexpr==true)
								parts<<"procsub(\""+tstr+"\").toInt(nullptr,0)";
							else
								parts<<"procsub(\""+tstr+"\")";

							j=0;
							continue;
						}
					else if(line.at(j+1)=="{")
						{
							tstr=parseVar(line);
							if(tstr.isEmpty()==false)
								{
									//QRegularExpression	re("^(\\$\\{[[:alnum:]_\\\"]+/+[[:print:]]+/[^}]+\\}+)");
									//QRegularExpression	re("^(\\$\\{#*[[:alnum:]_\"]+/+[[:print:]]+/[^}]+\\}+)|(\\$\\{*#*[[:alnum:]_\"]+\\})");
									QRegularExpression	re("^(\\$\\{#*[[:alnum:]_\"]+/+[[:print:]]+/[^}]+\\}+)|(\\$\\{*#*[[:alnum:]_\"]+\\})|(\\$\\{([[:alnum:]_]*):.*\\})");
									line=line.remove(re);
									parts<<tstr;
									j=0;
									continue;
								}
							lastletter=line.indexOf("}");
							tstr=line.mid(j+2,lastletter-2);
							line=line.right(line.length()-lastletter-1);
							parts<<tstr;
							j=0;
							continue;
						}
					else
						{
							lastletter=line.indexOf(QRegularExpression("([^[:alnum:]_{}])"),1);
							if(lastletter==-1)
								lastletter=line.length();
							tstr=line.mid(1,lastletter);
							line=line.right(line.length()-lastletter);
							tstr.remove(QRegularExpression("[\\\\{\\s}]"));
							parts<<tstr;
							j=0;
							continue;
						}
				}
			
			j++;
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
	if(isnumexpr==true)
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

//check for echo
	re.setPattern("^echo[[:space:]]*(.*)");
	match=re.match(line);
	if(match.hasMatch())
		{
			this->bashCommand=BASHECHO;
			tstr=match.captured(1);
			tstr.replace(QRegularExpression("^\"|\"$"),0);
			QString pal=this->parseExprString(tstr,false);
			this->cFile+=QString("outop<<%1<<Qt::endl;\n").arg(pal);
			return;
		}

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
			this->cFile+="{\n";
			this->bashCommand=BASHDO;
			return;
		}
	re.setPattern("\\s*(done\\b)");
	match=re.match(line);
	if(match.hasMatch())
		{
			this->cFile+="}\n";
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

