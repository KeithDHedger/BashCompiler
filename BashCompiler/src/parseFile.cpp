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
}

parseFileClass::parseFileClass()
{
}

bool parseFileClass::parseLine(QString qline)
{
	QString					tstr;
	QString					line;
	QRegularExpression		re;
	QRegularExpressionMatch	match;

	line=qline;

	this->linePosition=0;
	this->lineParts.clear();
	this->currentPart="";

	this->bashCommand=EXTERNALCOMMAND;
//check for assign
	re.setPattern("^([[:alpha:]][[:alnum:]_\\[\\]]*)=(.*)");
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
			if(line.at(this->linePosition).toLatin1()=='\'')
				{
					this->parseSingleQuoteString(line);
					continue;
				}

		//skip escaped quote
			if((line.at(this->linePosition).toLatin1()=='\\') && (line.at(this->linePosition+1).toLatin1()=='\\') && (line.at(this->linePosition+2).toLatin1()=='"'))
				{
					this->currentPart+="\\\"";
					this->linePosition+=3;
					continue;
				}
		//skip escaped dollar
			if((line.at(this->linePosition).toLatin1()=='\\') && (line.at(this->linePosition+1).toLatin1()=='$'))
				{
					this->currentPart+="$";
					this->linePosition+=2;
					continue;
				}
		//unescape other escaped chars
			if(line.at(this->linePosition).toLatin1()=='\\')
				{
					this->currentPart+="\\\\"+QString(line.at(this->linePosition+1).toLatin1());
					this->linePosition+=2;
					continue;
				}
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

			this->currentPart+=line.at(this->linePosition);
			this->linePosition++;
		}

	if(this->currentPart.isEmpty()==false)
		this->lineParts.append({this->currentPart,STRINGDATA});

	if(this->bashCommand==BASHASSIGN || this->bashCommand==BASHECHO)
		return(true);
	else
		return(false);
}

QString parseFileClass::cleanVar(QString line)
{
	QRegularExpression		re("([[:alnum:] \"]*)|\\$\\{*([[:alnum:]]*)}*");
	QRegularExpressionMatch	match;
	QString					retval=line;

	re.setPattern("\\$\\{?([[:alpha:]][[:alnum:]#_]*)}?");
	match=re.match(line);
	if(match.hasMatch())
		{
			retval="variables[\""+match.captured(1).trimmed()+"\"]";
			return(retval);
	}

	re.setPattern("^(\\\"[^\"]*\\\")");
	match=re.match(line);
	if(match.hasMatch())
		{
			retval=match.captured(1).trimmed();
			return(retval);
		}

	if(retval.contains(QRegularExpression("^\"(.*)\"$"))==false)
		retval.replace(QRegularExpression("^\"*(.*)\"*$"),"\"\\1\"");
	return(retval);
}

QString parseFileClass::globToRX(QString glob,bool greedy)
{
	QString tstr=glob;
	tstr.replace(".","[.]");
	tstr.replace("?",".");
	if(greedy==true)
		tstr.replace("*",".*");
	else
		tstr.replace("*",".*?");
	return(tstr);
}

QString parseFileClass::parseVar(QString line)
{
	QString varname;
	QString haystack;
	QString needle;
	QString retcode="";
	QRegularExpression re;
	QRegularExpressionMatch match;
//N.B.
//You must use echo ${REPLY%*.*} and NOT echo ${REPLY%.*} For now no default *	//TODO//
//check for special vars 1st
	//retcode=setSpecialDollars(line.at(1));
	retcode=setSpecialDollars(line);
	if(retcode.isEmpty()==false)
		return(retcode);

	re.setPattern("\\$\\{\\#?([[:alpha:]][[:alnum:]_\\[\\]]*)(:-|##?|%%?|:|//?|,,?|\\^\\^?)(.*)}");
	match=re.match(line);
	if(match.hasMatch())
		{
//${string:-default}
			if(match.captured(2).trimmed()==":-")
				{
					varname=match.captured(1).trimmed();
					haystack=this->parseOutputString(match.captured(3).trimmed());
					retcode="(variables[\""+varname+"\"].isEmpty()==true) ? (variables[\""+varname+"\"]="+haystack+") : (variables[\""+varname+"\"])";
					return(retcode);
				}

//${string,}
			if(match.captured(2).trimmed()==",")
				{
					varname=match.captured(1).trimmed();
					retcode="(variables[\""+varname+"\"].length()>0) ? (variables[\""+varname+"\"].at(0).toLower()+variables[\""+varname+"\"].mid(1)):(0)";
					return(retcode);
				}
//${string,,}
			if(match.captured(2).trimmed()==",,")
				{
					varname=match.captured(1).trimmed();
					retcode="variables[\""+varname+"\"].toLower()";
					return(retcode);
				}
//${string^}
			if(match.captured(2).trimmed()=="^")
				{
					varname=match.captured(1).trimmed();
					retcode="(variables[\""+varname+"\"].length()>0) ? (variables[\""+varname+"\"].at(0).toUpper()+variables[\""+varname+"\"].mid(1)):(0)";
					return(retcode);
				}
//${string^^}
			if(match.captured(2).trimmed()=="^^")
				{
					varname=match.captured(1).trimmed();
					retcode="variables[\""+varname+"\"].toUpper()";
					return(retcode);
				}

//${string##substring}
			if(match.captured(2).trimmed()=="##")
				{
					varname=match.captured(1).trimmed();
					needle=match.captured(3).trimmed();
					needle=this->globToRX(needle,true);
					needle="^"+needle+"(.*)$";
					retcode="QString(variables[\""+varname+"\"]).replace(QRegularExpression(QString(\""+needle+"\")) ,\"\\\\1\")";
					return(retcode);
				}
//${string#substring}
			else if(match.captured(2).trimmed()=="#")
				{
					varname=match.captured(1).trimmed();
					needle=match.captured(3).trimmed();
					needle=this->globToRX(needle,false);
					needle="^"+needle+"(.*)$";
					retcode="QString(variables[\""+varname+"\"]).replace(QRegularExpression(QString(\""+needle+"\")),\"\\\\1\")";
					return(retcode);
				}
//${string%substring}
			else if(match.captured(2).trimmed()=="%")
				{
					varname=match.captured(1).trimmed();
					needle=match.captured(3).trimmed();
					needle=this->globToRX(needle,true);
					needle="^(.*)"+needle+".*";
					retcode="QString(variables[\""+varname+"\"]).replace(QRegularExpression(QString(\""+needle+"\")),\"\\\\1\")";
					return(retcode);
				}
//${string%%substring}
			else if(match.captured(2).trimmed()=="%%")
				{
					varname=match.captured(1).trimmed();
					needle=match.captured(3).trimmed();
					needle=this->globToRX(needle,true);
					needle="^(.*?)"+needle;
					retcode="QString(variables[\""+varname+"\"]).replace(QRegularExpression(QString(\""+needle+"\")),\"\\\\1\")";
					return(retcode);
				}
			else if(match.captured(2).trimmed()==":")
//${string:position}/${string:position:length}
				{
					varname=match.captured(1).trimmed();
					if(match.captured(3).trimmed().contains(QRegularExpression("(.*):(.*)"))==true)
						{
//${string:position:length}
							re.setPattern("(.*):(.*)");
							match=re.match(match.captured(3));
							if(match.hasMatch())
								{
									haystack=this->cleanVar(match.captured(1).trimmed());
									needle=this->cleanVar(match.captured(2).trimmed());
									retcode="variables[\""+varname+"\"].mid(QString("+haystack+").toInt(nullptr,0),QString("+needle+").toInt(nullptr,0))";
									return(retcode);
								}
						}
					else
						{
//${string:position}
							needle=this->cleanVar(match.captured(3).trimmed());
							retcode="variables[\""+varname+"\"].mid(QString("+needle+").toInt(nullptr,0))";
							return(retcode);
						}
				}
			else if(match.captured(2).trimmed()=="//")
//${string//substring/replacement}
				{
					re.setPattern("\\$\\{([[:alnum:]_]+)//(.*)/(.*)}");
					match=re.match(line);
					if(match.hasMatch())
						{
							varname=match.captured(1).trimmed();
							haystack=this->cleanVar(match.captured(2).trimmed());
							needle=this->cleanVar(match.captured(3).trimmed());
							retcode="QString(variables[\""+varname+"\"]).replace("+haystack+","+needle+")";
							return(retcode);
						}
				}
//${string/substring/replacement}
			else if(match.captured(2).trimmed()=="/")
				{
					re.setPattern("\\$\\{([[:alnum:]_]+)/(.*)/(.*)}");
					match=re.match(line);
					if(match.hasMatch())
						{
							varname=match.captured(1).trimmed();
							haystack=this->cleanVar(match.captured(2).trimmed());
							needle=this->cleanVar(match.captured(3).trimmed());
							retcode="QString(variables[\""+varname+"\"]).replace(variables[\""+varname+"\"].indexOf(QString("+haystack+")),QString("+haystack+").size(), QString("+needle+"))";
							return(retcode);
						}
				}
		}

	//re.setPattern("\\$\\{#([[:alnum:]_]*)\\}");
	re.setPattern("\\$\\{#([[:alpha:]][[:alnum:]_\\[\\]]*)\\}");
	match=re.match(line);
	if(match.hasMatch())
		{
			varname=match.captured(1).trimmed();
			retcode="variables[\""+varname+"\"].length()";
			return(retcode);
		}
	
//	re.setPattern("\\$\\{([[:alnum:]_\\[\\]]*)\\}");
//	match=re.match(line);
//	if(match.hasMatch())
//		{
//			varname=match.captured(1).trimmed();
//			retcode="variables[\""+varname+"\"].length()";
//			return(retcode);
//		}


	retcode=line;
	retcode.replace(QRegularExpression("^\\$\\{*|\\}*$"),0);
//	retcode.replace(QRegularExpression("([[:alnum:]_]+)"),"variables[\"\\1\"]");
	retcode.replace(QRegularExpression("([[:alpha:]][[:alnum:]_\\[\\]]*)"),"variables[\"\\1\"]");
	return(retcode);
}

QString parseFileClass::lineToBashCLIString(QString qline)
{
	QString	newline=qline;
	QString	tstr="";
	QString	xx="";
	int		cnt=0;
	bool		flag=false;
	QString	varname="";

	tstr=qline;
	tstr.replace("\"","\\\"");
	tstr.replace("\\\\\"","\\\\\\\"");
	//tstr=tstr.replace(QRegularExpression("\\$\\{*([[:alpha:]][[:alnum:]_]*)\\}*"),"\"+variables[\"\\1\"].replace(replaceWhite, \" \")+\"");
	if(isInFunction==true)
		tstr=tstr.replace(QRegularExpression("\\${*([[0-9]]*)}*"),"\"+QString(fv[\"\\1\"])+\"");
	else
		tstr=tstr.replace(QRegularExpression("\\${*([[0-9]]*)}*"),"\"+QString(gargv[\\1])+\"");


	while(cnt<tstr.length())
		{
			if(cnt<tstr.length() && tstr.at(cnt).toLatin1()=='$')
				{
					cnt++;
					if(cnt>0 && cnt<tstr.length() && tstr.at(cnt-1).toLatin1()!='\\')
						{
							flag=true;
							varname="";
							if(tstr.at(cnt).toLatin1()=='{')
								cnt ++;
							while(flag==true && cnt<tstr.length())
								{
									if(cnt<tstr.length() && tstr.at(cnt).toLatin1()=='\\')
										{
											cnt++;
											continue;
										}
									if(cnt<tstr.length() && (tstr.at(cnt).toLatin1()=='}' || tstr.at(cnt).toLatin1()==' ' || tstr.at(cnt).toLatin1()=='"'))
										{
											if(tstr.at(cnt).toLatin1()=='}')
												cnt++;
											if(cnt<tstr.length() && tstr.at(cnt).toLatin1()=='"')
												cnt--;
											flag=false;
											continue;
										}
									else
										{
											varname+=tstr.at(cnt).toLatin1();
											cnt++;
										}
								}
							xx+="\"+variables[\""+varname+"\"]+\"";
							continue;
						}
					else
						{
							xx+=tstr.at(cnt).toLatin1();
							cnt++;
							continue;
						}
				}

			if(tstr.at(cnt).toLatin1()=='\'')
				{
					xx+="'";
					cnt++;
					while(tstr.at(cnt).toLatin1()!='\'')
						{
							xx+=tstr.at(cnt).toLatin1();
							cnt++;
						}
					xx+="'";
					cnt++;
					continue;
				}
			xx+=tstr.at(cnt).toLatin1();
			cnt++;
		}
	return(xx);
}

QString parseFileClass::parseExprString(bool isnumexpr)
{
	QStringList				parts;
	QString					complete="";
	QString					tstr;
	bool						isnumber=isnumexpr;
	bool						doabreak;
	QRegularExpression		re;
	QRegularExpressionMatch	match;
	int						argcnt=1;
	QString					outfmt="QString(\"";


	for(int j=0;j<this->lineParts.count();j++)
		{
			doabreak=false;
			switch(this->lineParts.at(j).typeHint)
				{
					case STRINGDATA:
						outfmt+=this->lineParts.at(j).data;
						break;
					case SINGLEQUOTSTRING:
						tstr=this->lineParts.at(j).data;
						outfmt+=tstr;
						break;
					case DOUBLEQUOTESTRING:
						tstr=this->lineParts.at(j).data;
						//tstr.replace(QRegularExpression("\\$\\{*([[:alnum:]_]+)\\}*"),"\"+variables[\"\\1\"]+\"");
						tstr.replace(QRegularExpression("\\$\\{*([[:alpha:][:alnum:]_\\[\\]]+)\\}*"),"\"+variables[\"\\1\"]+\"");
						outfmt+=tstr;
						break;
					case VARIABLEINCURLYS:
						parts<<this->parseVar(this->lineParts.at(j).data);
						if(argcnt<10)
							outfmt+="%0"+ QString::number(argcnt++);
						else
							outfmt+="%"+ QString::number(argcnt++);
						break;
//					case SQUAREBRACKETS:
//						errop<<"SQUAREBRACKETS"<<Qt::endl;
//						break;
					case BRACKETS:	
						if((this->lineParts.at(j).data.at(2)=='('))
							{
								tstr=this->lineParts.at(j).data;
								tstr.replace(QRegularExpression("^\\$\\(*|\\)*$"),"");
								tstr.replace(QRegularExpression("\\$\\{*|\\}*"),"");
								tstr.replace(QRegularExpression("\\b([[:alpha:]][[:alnum:]_\\[\\]]*)\\b"),"variables[\"\\1\"].toInt(nullptr,0)");
								tstr=tstr.trimmed();
								parts<<tstr;
								if(argcnt<10)
									outfmt+="%0"+ QString::number(argcnt++);
								else
									outfmt+="%"+ QString::number(argcnt++);
								break;
							}
						tstr=this->lineParts.at(j).data;
						tstr.replace(QRegularExpression("^\\$\\(*|\\)*$"),"");
						tstr=this->lineToBashCLIString(tstr);
						tstr.replace("\\\\\"","\\\"");
//call a bash function and assign
						re.setPattern("^[[:space:]]*([[:alpha:][:alnum:]_\\[\\]]+)[[:space:]]*(.*)$");
						match=re.match(tstr);
						if(match.hasMatch())
							{
								for(int jj=0;jj<functionNames.count();jj++)
									{
										if(match.captured(1).trimmed().compare(functionNames.at(jj).trimmed())==0)
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
														if(pfl.lineParts.at(kk).data.compare("\"+variables[\"")==0)
															{
																varstr+=QString("{\"%1\",variables[\"%2\"]},").arg(argcnt++).arg(pfl.lineParts.at(kk+1).data);
																kk+=3;
															}
														else
															varstr+=QString("{\"%1\",").arg(argcnt++)+tstr+"},";
													}
												varstr+="}";
												parts<<functionNames.at(jj)+"(true,"+varstr+")";
												if(argcnt<10)
													outfmt+="%0"+ QString::number(argcnt++);
												else
													outfmt+="%"+ QString::number(argcnt++);
												doabreak=true;
											}
									}
							}

						if(doabreak==true)
							break;
						if(isnumber==true)
							parts<<"runExternalCommands(\""+tstr+"\",true,\"\",false).toInt(nullptr,0)";
						else
							parts<<"runExternalCommands(\""+tstr+"\",true,\"\",false)";
						if(argcnt<10)
							outfmt+="%0"+ QString::number(argcnt++);
						else
							outfmt+="%"+ QString::number(argcnt++);
						break;

					case VARIABLE:
						parts<<this->parseVar(this->lineParts.at(j).data);
						if(argcnt<10)
							outfmt+="%0"+ QString::number(argcnt++);
						else
							outfmt+="%"+ QString::number(argcnt++);
						break;
					default:
						outfmt+=this->lineParts.at(j).data;
						continue;
				}
		}

	QString outstr="";

	for(int j=0;j<parts.count();j++)
		outstr+=".arg("+parts.at(j)+")";
	outfmt+="\")";
	complete=outfmt+outstr;
	if(isnumber==true)
		complete="("+complete+").toInt(nullptr,0)";

	return(complete);
}

QString parseFileClass::setSpecialDollars(QString dollar)
{
	QString tstr=dollar;
	QChar	num;
	bool		needlength=false;

	tstr.replace(QRegularExpression("[[:space:]]*\\$\\{?(#[[:digit:]]|#|\\?|[[:digit:]]|#[[:alnum:]]*)}?"),"\\1");
	if(tstr.isEmpty()==false)
		{
			if(tstr.length()>2)
				return("");
			if((tstr.length()>1) && (tstr.at(0)=="#"))
				{
					num=tstr.at(1);
					needlength=true;
				}
			else
				num=tstr.at(0);

			switch(num.toLatin1())
				{
					case '?':
						return("exitstatus");
						break;
					case '0':
						if(needlength==true)
							return("strlen(gargv[0])");
						else
							return("gargv[0]");
						break;
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						if(isInFunction==false)
							{
								if(needlength==true)
									return("strlen(gargv["+QString(num)+"])");
								else
									return("gargv["+QString(num)+"]");
							}
						else
							{
								if(needlength==true)
									return("strlen(fv[\""+QString(num)+"\"].toStdString().c_str())");
								else
									return("fv[\""+QString(num)+"\"]");
							}
						break;
					case '#':
						if(isInFunction==false)
							return("QString(\"%1\").arg(argc-1)");

						else
							return("QString(\"%1\").arg(fv.size())");
						break;
					
					default:
						break;
				}
		}
	return("");
}

void parseFileClass::parseSingleQuoteString(QString line)
{
	QString	strdata="'";

	this->linePosition++;

	if(this->currentPart.isEmpty()==false)
		{
			this->lineParts.append({this->currentPart,STRINGDATA});
			this->currentPart="";
		}

	while(this->linePosition<line.length())
		{
			if((line.at(this->linePosition).toLatin1()=='\'') && (line.at(this->linePosition-1).toLatin1()=='\\'))
				{
					strdata+=line.at(this->linePosition++);
				}
			else if((line.at(this->linePosition).toLatin1()=='\'') && (line.at(this->linePosition-1).toLatin1()!='\\'))
				break;

			strdata+=line.at(this->linePosition++);
		}
	strdata+="'";
	this->lineParts.append({strdata,SINGLEQUOTSTRING});
	this->linePosition++;
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
			dollardata="${";
			dtype=VARIABLEINCURLYS;
		}
	if(line.at(this->linePosition+1).toLatin1()=='(')
		{
			braceopen='(';
			braceclose=')';
			dollardata="$(";
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

	while(this->linePosition<line.length() && ((line.at(this->linePosition).isSpace()==false) || (line.at(this->linePosition).toLatin1()=='[') || (line.at(this->linePosition).toLatin1()==']')))
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

	if(this->preserveWhitespace==true)
		{
			while((this->linePosition<line.length()) && (line.at(this->linePosition).isSpace()==true))
				{
					this->currentPart+=line.at(this->linePosition);
					this->linePosition++;
				}
			lineParts.append({this->currentPart,STRINGDATA});
			this->currentPart="";
			return;
		}
	else
		{	
			while((this->linePosition<line.length()) && (line.at(this->linePosition).isSpace()==true))
				this->linePosition++;
		}
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

QString parseFileClass::parseOutputString(QString qline)
{
	QString					line;
	QString					outstr;
	QRegularExpression		re;
	QRegularExpressionMatch	match;

	outstr="";
	this->linePosition=0;
	this->lineParts.clear();
	this->currentPart="";
	this->bashCommand=BASHECHO;
	this->preserveWhitespace=true;
	line=qline;
	line=line.replace(QRegularExpression("^\"|\"$"),"");

	while(this->linePosition<line.length())
		{
		//skip escaped quote
			if((line.at(this->linePosition).toLatin1()=='\\') && (line.at(this->linePosition+1).toLatin1()=='"'))
				{
					this->currentPart+="\"";
					this->linePosition+=2;
					continue;
				}
		//skip escaped dollar
			if((line.at(this->linePosition).toLatin1()=='\\') && (line.at(this->linePosition+1).toLatin1()=='$'))
				{
					this->currentPart+="$";
					this->linePosition+=2;
					continue;
				}
		//skip escaped e
			if((line.at(this->linePosition).toLatin1()=='\\') && (line.at(this->linePosition+1).toLatin1()=='e'))
				{
					this->currentPart+="\\e"+QString(line.at(this->linePosition+2));
					this->linePosition+=3;
					continue;
				}
//		//skip escaped return
//			if((line.at(this->linePosition).toLatin1()=='\\') && (line.at(this->linePosition+1).toLatin1()=='r'))
//				{
//		errop<<"skip escaped return"<<Qt::endl;
//					this->currentPart+="\\r";
//					this->linePosition+=2;
//					continue;
//				}
		//skip escaped char
//			if((line.at(this->linePosition).toLatin1()=='\\') && (line.at(this->linePosition+1).toLatin1()=='e') && (line.at(this->linePosition+2).toLatin1()=='['))
//				{
//		errop<<"skip escaped char"<<Qt::endl;
//					this->currentPart+="\\e[";
//					this->linePosition+=3;
//					continue;
//				}
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
					
			if((line.at(this->linePosition).isSpace()==true))
				{
					this->parseWhitespace(line);
					continue;
				}

//			if(line.at(this->linePosition).toLatin1()=='[')
//				{
//					this->parseSquareBraces(line);
//					continue;
//				}

			this->currentPart+=line.at(this->linePosition);
			this->linePosition++;
		}

	if(this->currentPart.isEmpty()==false)
		this->lineParts.append({this->currentPart,STRINGDATA});

	QString pal=this->parseExprString(false);
	pal.replace("\\$","$");
	return(pal);
}

QString parseFileClass::optimizeOP(QString qline,bool *succeed)
{
	QString	pal=qline;
	*succeed=false;


	//pal.replace("\\\\\"","\\\"");
	if(pal.contains(QRegularExpression("^QString\\(\"%01\"\\).arg\\(.*\\.(mid|length|toInt|toStdString).*$"))==false)
		{
			if(pal.contains(QRegularExpression("^QString\\(\"%01\"\\)"))==true)
				{
					pal.replace(QRegularExpression("^QString\\(\"%01\"\\).arg\\((.*)\\)"),"\\1");
					*succeed=true;
				}

			if(pal.contains(QRegularExpression("^QString\\(\".*\"\\)$"))==true)
				{
					pal.replace(QRegularExpression("^QString\\((\".*\")\\)$"),"\\1");
					*succeed=true;
				}
		}

	if(mainCompilerClass->verboseCompile==true && *succeed==true)
		errop<<RED<<"Optimized "<<BLUE<<qline<<RED<<" to "<<CYAN<<pal<<NORMAL<<Qt::endl;

	return(pal);
}


