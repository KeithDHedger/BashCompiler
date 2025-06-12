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
	QString		line;
	QStringList	lines;
	QString		tstr;
	QStringList	ss;
	QStringList	keywords;
	bool			quotes=false;

	keywords<<"then"<<"else"<<"do";
	while (!this->mainBashFile.atEnd())
		{
			line=this->mainBashFile.readLine().trimmed();
			currentLine++;

			if(this->verboseCompile==true)
				qDebug()<<"processing line "<<this->currentLine<<line;

			if(line.startsWith('#'))
				{
					if(this->verboseCCode==true)
						{
							if(isInFunction==true)
								fCode<<"//"+line+"\n";
							else
								cCode<<"//"+line+"\n";
						}
					continue;
				}
			if(line.length()==0)
				continue;

//			for(int k=0;k<keywords.count();k++)
//				{
//					ss=line.split(QRegularExpression("[[:space:]]*"+keywords.at(k)+"[[:space:]]"));
//					if(ss.count()>1)
//						line+=";";
//				}

			lines.clear();
			tstr="";
			quotes=false;
			for(int j=0;j<line.length();j++)
				{
				quotes=false;
					if(line.at(j).toLatin1()=='\\')
						{
							tstr+=line.at(j++).toLatin1();
							tstr+=line.at(j).toLatin1();
							continue;
						}
					if(line.at(j).toLatin1()=='"')
						{
						
							//lines<<tstr;
							j++;
							//QString sstr="'";
							QString sstr="";
							//QString sstr="\\\\\"";
							quotes=true;
							while(quotes==true)
								{
									if(line.at(j).toLatin1()=='\\')
										{
											sstr+=line.at(j).toLatin1();
											sstr+=line.at(j+1).toLatin1();
											j+=2;
											continue;
										}
									if(line.at(j).toLatin1()=='"')
										quotes=false;
									else
										sstr+=line.at(j).toLatin1();
									j++;
								}
							//tstr+=sstr+"'";
							tstr+=sstr;
							//tstr+=sstr+"\\\\\"";
							//lines<<tstr;
							//tstr="";
							quotes=false;
							if(j>line.length()-1)
								goto escape2;
						}
					if((line.at(j).toLatin1()==';') && (quotes==false))
						{
							for(int k=0;k<keywords.count();k++)
								{
									ss=tstr.split(QRegularExpression("^[[:space:]]*"+keywords.at(k)+"[[:space:]]"));
									if(ss.count()>1)
										{
											lines<<keywords.at(k);
											lines<<ss.at(1);
											tstr="";
											goto escape;
										}
								}
							lines<<tstr;
escape:
							tstr="";
escape2:
							continue;
						}
//				for(int k=0;k<keywords.count();k++)
//				{
//					ss=line.split(QRegularExpression("[[:space:]]*"+keywords.at(k)+"[[:space:]]"));
//					if(ss.count()>1)
//						line+=";";
//				}

					tstr+=line.at(j).toLatin1();
				}

			if(tstr.length()>0)
				lines<<tstr;
			for(int j=0;j<lines.count();j++)
				{
					if(this->parseLine(lines.at(j).trimmed())==false)
						{
							qDebug()<<"Error parsing line:\n"<<line;
							return;
						}
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

	if(this->verboseCCode==true)
		{
			if(isInFunction==true)
				fCode<<QString("//%1\n").arg(line);
			else
				cCode<<QString("//%1\n").arg(line);
		}

	this->linePosition=0;
	this->lineParts.clear();
	this->currentPart="";

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
		//skip escaped quote
			if((line.at(this->linePosition).toLatin1()=='\\') && (line.at(this->linePosition+1).toLatin1()=='"'))
				{
					this->currentPart+="\\\"";
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
		//unescape other escaped chars
			if(line.at(this->linePosition).toLatin1()=='\\')
				{
					this->currentPart+="\\\\"+QString(line.at(this->linePosition+1).toLatin1());
					this->linePosition+=2;
					continue;
				}
			if(line.at(this->linePosition).toLatin1()=='"')
				{
				qDebug()<<qline;
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
					QString pal=this->parseExprString(false);
					//QString pal=this->parseExprStringNew(false);
					if(isInFunction==true)
						fCode<<QString("variables[\"%1\"]=%2;\n").arg(match.captured(1)).arg(pal);
					else
						cCode<<QString("variables[\"%1\"]=%2;\n").arg(match.captured(1)).arg(pal);
					return(true);
				}
				break;
			case BASHECHO:
				{
					QString pal=this->parseExprString(false);
					//QString pal=this->parseExprStringNew(false);
					if(isInFunction==true)
						{
							fCode<<"if(capture==false)\n";
							fCode<<QString("outop<<%1<<Qt::endl;\n").arg(pal);
							fCode<<"else\n";
							fCode<<"retstr+="<<pal<<"+\"\\n\";";
						}
					else
						cCode<<QString("outop<<%1<<Qt::endl;\n").arg(pal);
					return(true);
				}
				break;

			default:
				this->createCommand(line);
//external command
				if(this->bashCommand==EXTERNALCOMMAND)
					{
						if(this->noCodeOP==false)
							{
								if(isInFunction==true)
									fCode<<mainCommandsClass->makeExternalCommand(qline);
								else
									cCode<<mainCommandsClass->makeExternalCommand(line);
							}
						return(true);
					}
				else
					return(true);
				break;
		}
//should never get here!
	return(false);
}

QString parseFileClass::parseOutputString(QString qline)
{
	//QString					tstr;
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
	//line=qline.replace(QRegularExpression("^\"|\"$"),"");
line=qline;
errop<<"--->>>>>>>"<<line<<"<<<<--------"<<Qt::endl;
	while(this->linePosition<line.length())
		{
		//skip escaped quote
			if((line.at(this->linePosition).toLatin1()=='\\') && (line.at(this->linePosition+1).toLatin1()=='"'))
				{
					this->currentPart+="\\\"";
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
		//skip escaped nl
			if((line.at(this->linePosition).toLatin1()=='\\') && (line.at(this->linePosition+1).toLatin1()=='n'))
				{
					this->currentPart+="\\n";
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
					
			if((line.at(this->linePosition).isSpace()==true))
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

	QString pal=this->parseExprStringNew(false);
	return(pal);
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
	tstr.replace(".","\\\\.");
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

//check for special vars 1st
	retcode=setSpecialDollars(line.at(1));
	if(retcode.isEmpty()==false)
		return(retcode);

	re.setPattern("\\$\\{([[:alnum:]_]+)(##?|%%?|:|//?)(.*)}");
	match=re.match(line);
	if(match.hasMatch())
		{
//${string##substring}
			if(match.captured(2).trimmed()=="##")
				{
					varname=match.captured(1).trimmed();
					needle=match.captured(3).trimmed();
					needle=this->globToRX(needle,true);
					needle="^"+needle+"(.*)$";
					retcode="QString(\"%1\").arg(QString(variables[\""+varname+"\"]).replace(QRegularExpression(QString(\"%1\").arg(\""+needle+"\")),\"\\\\1\"))";
					return(retcode);
				}
//${string#substring}
			else if(match.captured(2).trimmed()=="#")
				{
					varname=match.captured(1).trimmed();
					needle=match.captured(3).trimmed();
					needle=this->globToRX(needle,false);
					needle="^"+needle+"(.*)$";
					retcode="QString(\"%1\").arg(QString(variables[\""+varname+"\"]).replace(QRegularExpression(QString(\"%1\").arg(\""+needle+"\")),\"\\\\1\"))";
					return(retcode);
				}
//${string%substring}
			else if(match.captured(2).trimmed()=="%")
				{
					varname=match.captured(1).trimmed();
					needle=match.captured(3).trimmed();
					needle=this->globToRX(needle,true);
					needle="^(.*)"+needle+".*";
					retcode="QString(\"%1\").arg(QString(variables[\""+varname+"\"]).replace(QRegularExpression(QString(\"%1\").arg(\""+needle+"\")),\"\\\\1\"))";
					return(retcode);
				}
//${string%%substring}
			else if(match.captured(2).trimmed()=="%%")
				{
					varname=match.captured(1).trimmed();
					needle=match.captured(3).trimmed();
					needle=this->globToRX(needle,true);
					needle="^(.*?)"+needle;
					retcode="QString(\"%1\").arg(QString(variables[\""+varname+"\"]).replace(QRegularExpression(QString(\"%1\").arg(\""+needle+"\")),\"\\\\1\"))";
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
									retcode="QString(\"%1\").arg(QString(variables[\""+varname+"\"]).mid(QString(\"%1\").arg("+haystack+").toInt(nullptr,0),QString(\"%1\").arg("+needle+").toInt(nullptr,0)))";
									return(retcode);
								}
						}
					else
						{
//${string:position}
							needle=this->cleanVar(match.captured(3).trimmed());
							retcode="QString(\"%1\").arg(QString(variables[\""+varname+"\"]).mid(QString(\"%1\").arg("+needle+").toInt(nullptr,0)))";
							return(retcode);
						}
				}
			else if(match.captured(2).trimmed()=="//")
//${string//substring/replacement}
				{
				//qDebug()<<">>>>"<<line<<match.captured(3).trimmed()<<"<<<<<";
					//re.setPattern("\\$\\{([[:alnum:]_]+)(##?|%%?|:|//?)([[:alnum:]\\.]*)(/|:)?(.*)}");
					re.setPattern("\\$\\{([[:alnum:]_]+)(##?|%%?|:|//?)([^/]*)(/|:)?(.*)}");
					match=re.match(line);
					if(match.hasMatch())
						{
							varname=match.captured(1).trimmed();
							haystack=this->cleanVar(match.captured(3).trimmed());
							needle=this->cleanVar(match.captured(5).trimmed());
							retcode="QString(variables[\""+varname+"\"]).replace("+haystack+","+needle+")";
							return(retcode);
						}
				}
//${string/substring/replacement}
			else if(match.captured(2).trimmed()=="/")
				{
					re.setPattern("\\$\\{([[:alnum:]_]+)(##?|%%?|:|//?)([[:alnum:]\\.]*)(/|:)?(.*)}");
					match=re.match(line);
					if(match.hasMatch())
						{
							varname=match.captured(1).trimmed();
							haystack=this->cleanVar(match.captured(3).trimmed());
							needle=this->cleanVar(match.captured(5).trimmed());
							retcode=QString("QString(%1).remove(%1.indexOf(%2,0),QString(%2).length()).insert(%1.indexOf(%2,0),%3)").arg("variables[\""+varname+"\"]").arg(haystack).arg(needle); 
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
					retcode="variables[\""+varname+"\"].length()";
					return(retcode);
				}
		}

	retcode=line;
	retcode.replace(QRegularExpression("^\\$\\{*|\\}*$"),0);
	retcode.replace(QRegularExpression("([[:alnum:]_]+)"),"variables[\"\\1\"]");
	return(retcode);
}

QString parseFileClass::lineToBashCLIString(QString qline)
{
	QString	newline=qline;
	QString	tstr="";

	newline.replace("\\\"","\\\\\\\"");
	//newline.replace("\"","\\\"");
	//errop<<newline<<Qt::endl;
	for(int j=0;j<newline.length();j++)
		{
			if((newline.at(j).toLatin1()=='\\'))
				{
					tstr+="\\\\"+QString(newline.at(j+1).toLatin1());
					j++;
					continue;
				}
			if((newline.at(j).toLatin1()=='"') && (newline.at(j-1).toLatin1()!='\\'))
				tstr+="\\\"";
			
			tstr+=newline.at(j).toLatin1();
		}
	//tstr.replace(QRegularExpression("\\$\\{*([[:alnum:]_]+)\\}*"),"\"+variables[\"\\1\"]+\"");
	tstr.replace(QRegularExpression("\\$\\{*([[:alpha:]][[:alnum:]_]*)\\}*"),"\"+variables[\"\\1\"]+\"");
	tstr.replace(QRegularExpression("\\$\\{*([[:digit:]])\\}*"),"\"+QString(gargv[\\1])+\"");

	return(tstr);
}

QString parseFileClass::parseExprString(bool isnumexpr)
{
	QStringList	parts;
	QString		complete="";
	QString		tstr;
	bool			isnumber=isnumexpr;
	bool			doabreak;
	QRegularExpression		re;
	QRegularExpressionMatch	match;

	for(int j=0;j<this->lineParts.count();j++)
		{
			doabreak=false;
			switch(this->lineParts.at(j).typeHint)
				{
					case STRINGDATA:
						parts<<QString("\"%1\"").arg(this->lineParts.at(j).data);
						break;
					case DOUBLEQUOTESTRING:
						tstr=this->lineParts.at(j).data;
						tstr.replace(QRegularExpression("\\$\\{*([[:alnum:]_]+)\\}*"),"\"+variables[\"\\1\"]+\"");
						parts<<tstr;
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
								tstr.replace(QRegularExpression("\\b([[:alpha:]][[:alnum:]_]*)\\b"),"variables[\"\\1\"].toInt(nullptr,0)");
								tstr=tstr.trimmed();
								parts<<tstr;
								break;
							}
						tstr=this->lineParts.at(j).data;
						tstr.replace(QRegularExpression("^\\$\\(*|\\)*$"),"");
						tstr=this->lineToBashCLIString(tstr);
//call a bash function and assign
						re.setPattern("^[[:space:]]*([[:alnum:]_]+)[[:space:]]+(.*)$");
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

												pfl.noCodeOP=true;
												pfl1.noCodeOP=true;
												pfl1.quotevars=true;
												pfl.quotevars=true;
												tstr=match.captured(2).trimmed();
												pfl.parseLine(tstr);
												int argcnt=1;
												for(int kk=0;kk<pfl.lineParts.count();kk++)
													{
														pfl1.parseLine(pfl.lineParts.at(kk).data);
														tstr=pfl1.parseExprString(false);
														//tstr=pfl1.parseExprStringNew(false);
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
												doabreak=true;
											}
									}
							}

						if(doabreak==true)
							break;

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

QString parseFileClass::parseExprStringNew(bool isnumexpr)
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
						//parts<<QString("\"%1\"").arg(this->lineParts.at(j).data);
						outfmt+=this->lineParts.at(j).data;
						//errop<<"STRINGDATA"<<outfmt<<this->lineParts.at(j).data<<Qt::endl;
						break;
					case DOUBLEQUOTESTRING:
						tstr=this->lineParts.at(j).data;
						tstr.replace(QRegularExpression("\\$\\{*([[:alnum:]_]+)\\}*"),"\"+variables[\"\\1\"]+\"");
						//parts<<tstr;
						//outfmt+=this->lineParts.at(j).data;
						outfmt+=tstr;
						//errop<<"DOUBLEQUOTESTRING"<<outfmt<<Qt::endl;
						//outfmt+="%"+ QString::number(j+1);
						break;
					case VARIABLEINCURLYS:
						parts<<this->parseVar(this->lineParts.at(j).data);
						//errop<<">>----->>>"<<parts.at(parts.count()-1)<<Qt::endl;
						//outfmt+="%"+ QString::number(j+1);
						outfmt+="%"+ QString::number(argcnt++);
						//outfmt+="'%"+ QString::number(argcnt++)+"'";
						//if(this->quotevars==false)
						//	outfmt+="%"+ QString::number(argcnt++);
						//else
						//{
						////qDebug()<<outfmt;
						//	outfmt+="'%"+ QString::number(argcnt++)+"'";
						//	}
						//errop<<"VARIABLEINCURLYS"<<outfmt<<Qt::endl;
						break;
					case BRACKETS:
						//errop<<"BRACKETS"<<outfmt<<Qt::endl;
						if((this->lineParts.at(j).data.at(2)=='('))
							{
								tstr=this->lineParts.at(j).data;
								tstr.replace(QRegularExpression("^\\$\\(*|\\)*$"),"");
								tstr.replace(QRegularExpression("\\$\\{*|\\}*"),"");
								tstr.replace(QRegularExpression("\\b([[:alpha:]][[:alnum:]_]*)\\b"),"variables[\"\\1\"].toInt(nullptr,0)");
								tstr=tstr.trimmed();
								parts<<tstr;
								//outfmt+="%"+ QString::number(j+1);
								outfmt+="%"+ QString::number(argcnt++);
								//if(this->quotevars==true)
								//	outfmt+="'%"+ QString::number(argcnt++)+"'";
								//else
								//	outfmt+="%"+ QString::number(argcnt++);
								//errop<<"11111"<<outfmt<<Qt::endl;
								break;
							}
						tstr=this->lineParts.at(j).data;
						tstr.replace(QRegularExpression("^\\$\\(*|\\)*$"),"");
						tstr=this->lineToBashCLIString(tstr);
//call a bash function and assign
						re.setPattern("^[[:space:]]*([[:alnum:]_]+)[[:space:]]+(.*)$");
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

												pfl.noCodeOP=true;
												pfl1.noCodeOP=true;
												pfl.quotevars=true;
												pfl1.quotevars=true;
												tstr=match.captured(2).trimmed();
												pfl.parseLine(tstr);
												int argcnt=1;
												for(int kk=0;kk<pfl.lineParts.count();kk++)
													{
														pfl1.parseLine(pfl.lineParts.at(kk).data);
														//tstr=pfl1.parseExprString(false);
														tstr=pfl1.parseExprStringNew(false);
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
												//outfmt+="%"+ QString::number(j+1);//////////////////////
												outfmt+="%"+ QString::number(argcnt++);
												//if(this->quotevars==true)
												//	outfmt+="'%"+ QString::number(argcnt++)+"'";
												//else
												//	outfmt+="%"+ QString::number(argcnt++);
												//errop<<"22222"<<outfmt<<Qt::endl;
												doabreak=true;
											}
									}
							}
//errop<<"6666"<<outfmt<<Qt::endl;
						if(doabreak==true)
							break;
//errop<<"777777"<<outfmt<<Qt::endl;
//	outop<<QString("%1").arg(procsub("basename \"/tmp/a \\\"test\\\" file.txt\" .txt"))<<Qt::endl;

						if(this->quotevars==true)
							tstr.replace(QRegularExpression("(\"?\\+?variables\\[\".*\"\\]\\+?\"?)"),"'\\1'");
						if(isnumber==true)
							parts<<"procsub(\""+tstr+"\").toInt(nullptr,0)";
						else
							parts<<"procsub(\""+tstr+"\")";
						outfmt+="%"+ QString::number(argcnt++);
//									outfmt+="'%"+ QString::number(argcnt++)+"'";
//								else
//									outfmt+="%"+ QString::number(argcnt++);

					errop<<"aaaaaa "<<tstr<<Qt::endl;
						break;
					case VARIABLE:
						parts<<this->parseVar(this->lineParts.at(j).data);
						outfmt+="%"+ QString::number(argcnt++);
						//		if(this->quotevars==true)
						//			outfmt+="'%"+ QString::number(argcnt++)+"'";
						//		else
						//			outfmt+="%"+ QString::number(argcnt++);
						//errop<<"3333"<<outfmt<<Qt::endl;

						break;
					default:
						outfmt+=this->lineParts.at(j).data;
						errop<<"4444"<<outfmt<<Qt::endl;
						//outfmt+="%"+ QString::number(argcnt++);
						//parts<<QString("\"%1\"").arg(this->lineParts.at(j).data);
						continue;
				}
		}
//errop<<"8888"<<outfmt<<Qt::endl;

	QString outstr="";
	//QString outfmt="QString(\"";
	for(int j=0;j<parts.count();j++)
		{
			//outfmt+="%"+ QString::number(j+1);
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

//call no return val
	re.setPattern("^[[:space:]]*([[:alnum:]_]+)[[:space:]]+(.*)$");
	match=re.match(line);
	if(match.hasMatch())
		{
			for(int j=0;j<functionNames.count();j++)
				{
					if(match.captured(1).trimmed().compare(functionNames.at(j).trimmed())==0)
						{
							parseFileClass	pfl;
							parseFileClass	pfl1;
							QString varstr="{";

							pfl.noCodeOP=true;
							pfl1.noCodeOP=true;
							pfl1.quotevars=true;
							pfl.quotevars=true;

							tstr=match.captured(2).trimmed();
							pfl.parseLine(tstr);
							int argcnt=1;
							for(int kk=0;kk<pfl.lineParts.count();kk++)
								{
									pfl1.parseLine(pfl.lineParts.at(kk).data);
									tstr=pfl1.parseExprString(false);
									//tstr=pfl1.parseExprStringNew(false);
									varstr+=QString("{\"%1\",").arg(argcnt++)+tstr+"},";
								}
							varstr+="}";
							if(isInFunction==true)
								fCode<<match.captured(1).trimmed()<<"(false,"+varstr+");\n";
							else
								cCode<<match.captured(1).trimmed()<<"(false,"+varstr+");\n";
							this->bashCommand=BASHFUNCTIONCALL;
							return;
						}
				}
		}
//define
	if(mainCommandsClass->makeFunction(line)==true)
		{
			this->bashCommand=BASHFUNCTION;
			return;
		}

	re.setPattern("^[[:space:]]*(\\{|\\})[[:space:]]*$");
	match=re.match(line);
	if(match.hasMatch())
		{
			if(match.captured(1).trimmed().compare("}")==0)
				{
					fCode<<"return(retstr.left(retstr.length()-1));\n}\n\n";
					isInFunction=false;
				}
			this->bashCommand=BASHFUNCTION;
			return;
		}

//printf
	if(mainCommandsClass->makePrintf(line)==true)
		{
			this->bashCommand=BASHBUILTIN;
			return;
		}

//pushd/popd
	if(mainCommandsClass->makePushd(line)==true)
		{
			this->bashCommand=BASHBUILTIN;
			return;
		}
	if(mainCommandsClass->makePopd(line)==true)
		{
			this->bashCommand=BASHBUILTIN;
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

//while read
	if(mainCommandsClass->makeWhileRead(line)==true)
		{
			this->bashCommand=BASHWHILEREAD;
			return;
		}

//while do done
	if(mainCommandsClass->makeWhile(line)==true)
		{
			this->bashCommand=BASHWHILE;
			return;
		}

	re.setPattern("^[[:space:]]*(do)[[:space:]]*$");
	match=re.match(line);
	if(match.hasMatch())
		{
			if(isInFunction==true)
				fCode<<"{\n";//}
			else
				cCode<<"{\n";//}
			this->bashCommand=BASHDO;
			return;
		}


	re.setPattern("^[[:space:]]*(done)[[:space:]]*$");
	match=re.match(line);
	if(match.hasMatch())
		{
			if(isInFunction==true)
				fCode<<"}\n";
			else
				cCode<<"}\n";
			this->bashCommand=BASHDONE;
			return;
		}

	re.setPattern("^[[:space:]]*(done)[[:space:]]*<[[:space:]]*.*<\\((.*)\\)");
	match=re.match(line);
	if(match.hasMatch())
		{
			if(this->whileReadLine.size()>0)
				{
					QString tstr=this->lineToBashCLIString(match.captured(2).trimmed());
					tstr.replace(QRegularExpression("(\"\\+variables\\[\"[[:alpha:]][[:alnum:]_]*\"\\]\\+\")"),"\\\"\\1\\\"");//TODO//horible hack
				
					if(isInFunction==true)
						fCode<<"}\npclose(fp);\n}\n";
					else
						cCode<<"}\npclose(fp);\n}\n";
					cCode.insert(this->whileReadLine.last(),"QString proc=\""+tstr+"\";\n");
					this->whileReadLine.pop_back();
				}
			else
				{
					if(isInFunction==true)
						fCode<<"}\n";
					else
						cCode<<"}\n";
				}
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
			case '0':
				return("gargv["+QString(dollar)+"]");
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
					return("gargv["+QString(dollar)+"]");
				else
					return("fv[\""+QString(dollar)+"\"]");
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
	errop<<">>>>>>>>>>"<<strdata<<"<<<<<<<<<<<<"<<Qt::endl;
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
//			if(this->currentPart.isEmpty()==false)
//		{
//			lineParts.append({this->currentPart,STRINGDATA});
//			this->currentPart="";
//		}
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

bool parseFileClass::isAVar(QString qline)
{
	return(qline.contains(QRegularExpression("^[[:space:]]*\"?\\$\\{?")));
}

QString parseFileClass::parseOutputStringNew(QString qline)
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
	line=qline.replace(QRegularExpression("^\"|\"$"),"");
	line=qline;

	while(this->linePosition<line.length())
		{
//			if(line.at(this->linePosition).toLatin1()=='#')
//				{
//					while((this->linePosition<line.length()))// && (line.at(this->linePosition).toLatin1()=='#'))
//						this->linePosition++;
//					continue;
//				}

		//skip escaped quote
			if((line.at(this->linePosition).toLatin1()=='\\') && (line.at(this->linePosition+1).toLatin1()=='"'))
				{
					this->currentPart+="\\\"";
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
		//skip escaped nl
			if((line.at(this->linePosition).toLatin1()=='\\') && (line.at(this->linePosition+1).toLatin1()=='n'))
				{
					this->currentPart+="\\n";
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
					
			if((line.at(this->linePosition).isSpace()==true))
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

	QString pal=this->parseExprStringNew(false);
	return(pal);
}