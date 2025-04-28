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
			this->parseLine(line);
		}
}

bool parseFileClass::parseLine(QString line)
{
	QString	namecom="";
	bool		isassign=false;

	//check for comment or 0 len
	if(line.startsWith('#'))
		{
			if(this->verboseCCode==true)
				this->cFile+="//"+line.remove(0,1)+"\n";
			return(false);
		}
	if(line.length()==0)
		{
			return(false);
		}

	this->lineParts=line.split(" ");

	if(this->verboseCCode==true)
		this->cFile+=QString("//%1\n").arg(line);

	if(this->verboseCompile==true)
		qDebug()<<"processing "<<line;

	for(int j=0;j<this->lineParts.count();j++)
		{
			namecom="";
			isassign=false;
			//check for assign
			for(int k=0;k<this->lineParts.at(j).length();k++)
				{
					if(this->lineParts.at(j).at(k)=='=')
						{
							isassign=true;
							break;
						}
					namecom+=this->lineParts.at(j).at(k);
				}

		if(isassign==true)
			{
				QString tstr="";
				QString assignparam="";

				this->cFileDeclares<<"std::string "+namecom+";";
				this->cFile+=namecom+"=";
				tstr=line;
				tstr.replace(namecom+"=","");
				tstr.replace(QRegularExpression("^\"|\"$"),"");
				this->parseLineAsParams(tstr);

				assignparam=this->dataArrayToString();
				if(assignparam.at(assignparam.length()-1)!=';')
					assignparam+=";";
				this->cFile+=assignparam+"\n";
				return(true);
			}
		else
			{
				QString	com=this->createCommand(line);
				this->cFile+=com+"\n";
				return(true);
			}
		}
	return(false);
}

QString parseFileClass::dataArrayToString(void)
{
	QString		cline="";
	int			lastpart=this->dataArray.count();
	const char	*add="+";

	for(int j=0;j<lastpart;j++)
		{
			if(j==lastpart-1)
				add="";

			switch(this->dataArray.at(j).dtype)
				{
					case CONSTDATA:
						cline+="\""+this->dataArray.at(j).data+"\""+add;
						break;
					case STRVAR:
						cline+=this->dataArray.at(j).data+add;
						break;
					case PROCSUB:
						cline+=QString("procsub(\"%1\")%2").arg(this->dataArray.at(j).data).arg(add);
						break;
					case INVALID:
						break;
					default:
						break;
				}
		}
	return(cline);
}

QString parseFileClass::createCommand(QString line)
{
	if(this->lineParts.at(0)=="echo")
		return(mainCommandsClass->makeEcho(line));

	if(this->lineParts.at(0)=="if")
		return(mainCommandsClass->makeIf(line));
	if(this->lineParts.at(0)=="then")
		return(QString("{"));
	if(this->lineParts.at(0)=="else")
		return(QString("}\nelse\n{"));

	if(this->lineParts.at(0)=="fi")
		return(QString("}"));

	return(mainCommandsClass->makeExternalCommand(line));
}

void parseFileClass::parseLineAsParams(QString line)
{
	QVector<lineData>	dataArray;
	QString part="";

	for(int j=0;j<line.length();j++)
		{
			switch(line.at(j).toLatin1())
				{
					case '$':
						if(part.length()>0)
							{
								dataArray.push_back({part,CONSTDATA});
								part="";
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
								dataArray.push_back({part,STRVAR});
								part="";
								continue;
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
								dataArray.push_back({part,PROCSUB});
								part="";
								continue;
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
						dataArray.push_back({part,STRVAR});
						part="";
						continue;
						break;
					
					default:
						part+=line.at(j).toLatin1();
				}
		}

	if((part.length()>0) )
		dataArray.push_back({part,CONSTDATA});

//	for(int j=0;j<dataArray.count();j++)
//		fprintf(stderr,">>%s<< %s\n",dataArray.at(j).data.toStdString().c_str(),typeAsText[dataArray.at(j).dtype]);

	this->dataArray=dataArray;
}
