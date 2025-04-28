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
	return(QString("fflush(NULL);\nsystem(\"%1\");").arg(line));
}

QString commandsClass::makeIntCompare(QVector<lineData> da)
{
	QString compstr="";
	int doneg=0;

	for(int j=0;j<da.count();j++)
		{
			switch(da.at(j).dtype)
				{
					case INVALID:
						break;
					case CONSTSTR:
						break;
					case STRVAR:
						compstr+="std::stol(std::string("+da.at(j).data+"),NULL,0)";
						break;
					case PROCSUB:
						break;
					case NEGATE:
						compstr+="!(";
						doneg++;
						break;
					case CONSTDATA:
						{
							QString comp=da.at(j).data;

							if(da.at(j).data.compare("-eq")==0)
								comp="==";
							if(da.at(j).data.compare("-lt")==0)
								comp="<";
							if(da.at(j).data.compare("-le")==0)
								comp="<=";
							if(da.at(j).data.compare("-gt")==0)
								comp=">";
							if(da.at(j).data.compare("-ge")==0)
								comp=">=";
							if(da.at(j).data.compare("-ne")==0)
								comp="!=";
							compstr+=" "+comp+" ";
						}
						break;
				}
		}
	for(int j=0;j<doneg;j++)
		compstr+=")";

	//fprintf(stderr,">>>%s<<<\n",compstr.toStdString().c_str());
	return(compstr);
}

QString commandsClass::makeStrCompare(QVector<lineData> da)
{
	QString compstr="";
	int doneg=0;
	QString compnumberstr;
	QString leftstr="";
	QString ritestr="";

	for(int j=0;j<da.count();j++)
		{
			switch(da.at(j).dtype)
				{
					case INVALID:
						break;
					case CONSTSTR:
						if(leftstr.length()==0)
							leftstr="std::string("+da.at(j).data+")";
						else
							ritestr=da.at(j).data;
						break;
					case STRVAR:
						if(leftstr.length()==0)
							leftstr=da.at(j).data;
						else
							ritestr=da.at(j).data;
						break;
					case PROCSUB:
						break;
					case NEGATE:
						compstr+="!(";
						doneg++;
						break;
					case CONSTDATA:
						{
							if(da.at(j).data.compare("=")==0)
								compnumberstr="==0";
							if(da.at(j).data.compare("!=")==0)
								compnumberstr="!=0";
							if(da.at(j).data.compare("<")==0)
								compnumberstr="<0";
							if(da.at(j).data.compare(">")==0)
								compnumberstr=">0";
						}
						break;
				}
		}
	compstr+=leftstr+".compare("+ritestr+")"+compnumberstr;

	for(int j=0;j<doneg;j++)
		compstr+=")";

//	fprintf(stderr,"--->>>%s<<<---\n",compstr.toStdString().c_str());
	return(compstr);
}

QString commandsClass::makeIf(QString line)
{
	QVector<lineData>	dataArray;
	QString				expstr="";
	lineData				lnd;
	QString				res;
	bool					singlelinethen=false;
	
	//clean line
	expstr=line;

	expstr.replace("if ","");

	if(expstr.endsWith("then"))
		{
			singlelinethen=true;
			expstr=expstr.replace(QRegularExpression(";[[:space:]]*then.*$"),"").trimmed();
		}
	expstr=expstr.replace(QRegularExpression("^\\[[\\[]|\\][\\]]$"),"").trimmed();

	bool		isstringexp=false;
	while(expstr.length()>0)
		{
			if(lnd.dtype==CONSTDATA)
				{
					if(lnd.data.compare("=")==0)
						isstringexp=true;
					else if(lnd.data.compare("<")==0)
						isstringexp=true;
					else if(lnd.data.compare(">")==0)
						isstringexp=true;
					else if(lnd.data.compare("==")==0)
						isstringexp=true;
					else if(lnd.data.compare("!=")==0)
						isstringexp=true;
					else
						isstringexp=false;
				}
			lnd=parseStr(expstr);
			dataArray.push_back(lnd);
			expstr=expstr.sliced(lnd.argLen).trimmed();
		}

	if(isstringexp==false)
		res="if("+makeIntCompare(dataArray)+")";
	else
		res="if("+makeStrCompare(dataArray)+")";
	if(singlelinethen==true)
		res+="\n{";

	return(res);
}

lineData commandsClass::parseStr(QString line)
{
	QString	part="";
	lineData	lnd={"",INVALID};
	bool		openquote=false;

	for(int j=0;j<line.length();j++)
		{
			switch(line.at(j).toLatin1())
				{
					case '!':
						if(openquote==false)
							{
								lnd={"!",NEGATE,1};
								return(lnd);
							}
						break;
					case ' ':
						if((part.length()>0) && (openquote==false))
							{
								if(part.at(0)=='"')
									lnd={part,CONSTSTR,part.length()};
								else
									lnd={part,CONSTDATA,part.length()};
								return(lnd);
							}
						if(openquote==true)
							{
								part+=" ";
								continue;
							}
						break;
					case '"':
						openquote=!openquote;
						part+=line.at(j).toLatin1();
						continue;
						break;
					case '$':
						if(part.length()>0)
							{
								if(part.at(0)=='"')
									lnd={part,CONSTSTR,part.length()};
								else
									lnd={part,CONSTDATA,part.length()};
								return(lnd);
							}

						j++;
						if(line.at(j).toLatin1()=='{')//is a var
							{
								j++;
								part="";
								while((j<line.length()) && (line.at(j).toLatin1()!='}'))
									{
										part+=line.at(j).toLatin1();
										j++;
									}
								lnd={part,STRVAR,part.length()+3};
								return(lnd);
							}
						if(line.at(j)=='(')//is a proc sub
							{
								part="";
								j++;
								while((j<line.length()) && (line.at(j).toLatin1()!=')'))
									{
										part+=line.at(j).toLatin1();
										j++;
									}
								lnd={part,PROCSUB,part.length()+3};
								return(lnd);
							}

						part="";
						while((j<line.length()) && ((line.at(j).isLetterOrNumber()==true) || (line.at(j).toLatin1()=='_')))
							{
								if(line.at(j).toLatin1()==' ')
									break;
								else
									part+=line.at(j).toLatin1();

								j++;
							}
						j--;
						lnd={part,STRVAR,part.length()+1};
						return(lnd);
						continue;
					
					default:
						part+=line.at(j).toLatin1();
				}
		}
	if(part.length()>0)
		{
			if(part.at(0)=='"')
				lnd={part,CONSTSTR,part.length()};
			else
				lnd={part,CONSTDATA,part.length()};
		}
	return(lnd);
}

QString commandsClass::makeEcho(QString line)
{
	QString format="";
	QString param="";
	QString tstr="";
	QString com="";

	tstr=line;
	tstr.replace("echo ","");
	tstr.replace(QRegularExpression("^\"|\"$"),"");
	mainParseClass->parseLineAsParams(tstr);

	com="printf(\"";
	for(int l=0;l<mainParseClass->dataArray.count();l++)
		{
			if(mainParseClass->dataArray.at(l).dtype==CONSTDATA)
				format+=mainParseClass->dataArray.at(l).data;
			else
				{
					format+="%s";
					//convert nl to spaces //TODO//
					param+=","+mainParseClass->dataArray.at(l).data+".c_str()";
				}
		}
	com+=format+"\\n\""+param+");";
	return(com);
}
