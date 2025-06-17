#include "StdAfx.h"
#include "VerdictBaseData.h"

#include <QDebug>
#include <QDomDocument>
#include <QFile>

#include "JGeoCal/JGeoCalExp.h"

VerdictBaseData::VerdictBaseData(QObject *parent)
	: QObject(parent)
{

}

VerdictBaseData::~VerdictBaseData()
{

}

void VerdictBaseData::initEntityDamageProb(const QString& file)
{
	qDebug() << __FUNCDNAME__ << file;
	// 创建 QDomDocument 对象
	QDomDocument doc("mydocument");

	// 创建 QFile 对象并打开 XML 文件
	QFile domFile(file);
	if (!domFile.open(QIODevice::ReadOnly)) {
		qDebug() << "Failed to open file.";
		return ;
	}
	// 从文件读取 XML 数据并加载到 QDomDocument
	if (!doc.setContent(&domFile)) {
		qDebug() << __FUNCDNAME__ << domFile.readAll();
		qDebug() << "Failed to load document.";
		domFile.close();
		return;
	}
	// 关闭文件
	domFile.close();
	QDomElement docElem = doc.documentElement();
	if("EntityDamageProb" == docElem.tagName())
	{
		QDomNodeList docElemChildren = docElem.childNodes();
		for(int index0 = 0; index0 < docElemChildren.size(); index0++)
		{
			QDomNode item = docElemChildren.at(index0);
			if(item.isElement())
			{
				QDomElement itElem = item.toElement();
				if("Item" == itElem.tagName())
				{
					QString entClsName = itElem.attribute("EntClsName");
					EntityDamageProbItem itemInfo;
					itemInfo.attrs.insert("EntClsName", itElem.attribute("EntClsName"));
					itemInfo.attrs.insert("EntTag",		itElem.attribute("EntTag"));
					QDomNodeList itElemChildren = itElem.childNodes();
					for(int index1 = 0; index1 < itElemChildren.size(); index1++)
					{
						QDomNode warheadProbTable = itElemChildren.at(index1);
						if(warheadProbTable.isElement())
						{
							QDomElement warheadProbTableElem = warheadProbTable.toElement();
							if("WarheadProbTable" == warheadProbTableElem.tagName())
							{
								WarheadProbTable warheadProbTable;
								QString warheadType = warheadProbTableElem.attribute("WarheadType");
								warheadProbTable.attrs.insert("WarheadType", warheadProbTableElem.attribute("WarheadType"));
								QDomNodeList warheadProbTableElemChildren = warheadProbTableElem.childNodes();
								for(int index2 = 0; index2 < warheadProbTableElemChildren.size(); index2++)
								{
									QDomNode impactProbTable = warheadProbTableElemChildren.at(index2);
									if(impactProbTable.isElement())
									{
										QDomElement impactProbTableElem = impactProbTable.toElement();
										if("ImpactProbTable" == impactProbTableElem.tagName())
										{
											ImpactProbTable impactTable;
											QString impactType = impactProbTableElem.attribute("ImpactType");
											impactTable.attrs.insert("ImpactType", impactProbTableElem.attribute("ImpactType"));
											QDomNodeList impactProbTableElemChildren = impactProbTableElem.childNodes();
											for(int index3 = 0; index3 < impactProbTableElemChildren.size(); index3++)
											{
												QDomNode dmgProb = impactProbTableElemChildren.at(index3);
												if(dmgProb.isElement())
												{
													QDomElement dmgProbElem = dmgProb.toElement();
													DmgProb probdata;
													probdata.max = dmgProbElem.attribute("DistMax_m").toDouble();
													probdata.min = dmgProbElem.attribute("DistMin_m").toDouble();
													probdata.tntKg = dmgProbElem.attribute("EffPower_TNTkg").toDouble();
													probdata.length = 0;
													QDomNodeList dmgProbElemChildren = dmgProbElem.childNodes();
													for(int index4 = 0; index4 < dmgProbElemChildren.size(); index4++)
													{
														QDomNode prob = dmgProbElemChildren.at(index4);
														if(prob.isElement())
														{
															QDomElement probElem = prob.toElement();
															if("Prob" == probElem.tagName())
															{
																probdata.azDeg.append(probElem.attribute("Az_deg").toDouble());
																probdata.elDeg.append(probElem.attribute("El_deg").toDouble());
																probdata.pt.append(probElem.attribute("Pt").toDouble());
																probdata.length++;
															}
														}
													}
													impactTable.dmgProbs.append(probdata);
												}
											}
											warheadProbTable.impactProbTables.insert(impactType, impactTable);
										}
									}
								}
								itemInfo.warheadProbTables.insert(warheadType, warheadProbTable);
							}
						}
					}
					m_entityDamageProbItems.insert(entClsName, itemInfo);
				}
			}
		}
	}
}

QString matchString(const QStringList& list, const QString& match)
{
	foreach(QString tmpQString, list)
	{
		if(tmpQString.toUpper().contains(match.toUpper()))
		{
			return tmpQString;
		}
	}
	return "";
}

void VerdictBaseData::initWeaponHitProb(const QString& file)
{	
	qDebug() << __FUNCDNAME__ << file;
	// 创建 QDomDocument 对象
	QDomDocument doc("mydocument");

	// 创建 QFile 对象并打开 XML 文件
	QFile domFile(file);
	if (!domFile.open(QIODevice::ReadOnly)) {
		qDebug() << "Failed to open file.";
		return ;
	}
	// 从文件读取 XML 数据并加载到 QDomDocument
	if (!doc.setContent(&domFile)) {
		qDebug() << __FUNCDNAME__ << domFile.readAll();
		qDebug() << "Failed to load document.";
		domFile.close();
		return;
	}
	// 关闭文件
	domFile.close();

	QDomElement docElem = doc.documentElement();
	if("WeaponHitProb" == docElem.tagName())
	{
		QDomNodeList docElemChildren = docElem.childNodes();
		for(int index0 = 0; index0 < docElemChildren.size(); index0++)
		{
			QDomNode item = docElemChildren.at(index0);
			if(item.isElement())
			{
				QDomElement itElem = item.toElement();
				if("Item" == itElem.tagName())
				{
					QString entClsName = itElem.attribute("EntClsName");
					WeaponHitProbItem itemInfo;
					itemInfo.attrs.insert("EntClsName", itElem.attribute("EntClsName"));
					itemInfo.attrs.insert("EntTag",		itElem.attribute("EntTag"));

					QDomNodeList itElemChildren = itElem.childNodes();
					for(int index1 = 0; index1 < itElemChildren.size(); index1++)
					{
						QDomNode hitProbTable = itElemChildren.at(index1);
						if(hitProbTable.isElement())
						{
							QDomElement hitProbTableElem = hitProbTable.toElement();
							if("HitProbTable" == hitProbTableElem.tagName())
							{
								HitProbTable hitProbData;
								QString hitProbDataType = QString("%1-%2").arg(hitProbTableElem.attribute("Targetkind")).arg(hitProbTableElem.attribute("Targetdomain"));
								hitProbData.attrs.insert("Targetkind",	hitProbTableElem.attribute("Targetkind"));
								hitProbData.attrs.insert("Targetdomain",hitProbTableElem.attribute("Targetdomain"));
								hitProbData.attrs.insert("StdProb",		hitProbTableElem.attribute("StdProb"));

								QDomNodeList hitProbTableElemChildren = hitProbTableElem.childNodes();
								for(int index2 = 0; index2 < hitProbTableElemChildren.size(); index2++)
								{
									QDomNode corrFactor = hitProbTableElemChildren.at(index2);
									if(corrFactor.isElement())
									{
										QDomElement corrFactorElem = corrFactor.toElement();
										if("corrFactor" == corrFactorElem.tagName())
										{
											CorrFactor corrFactorData;
											QString corrFactorType = corrFactorElem.attribute("FacTag");
											corrFactorData.attrs.insert("FacTag", corrFactorElem.attribute("FacTag"));

											QDomNodeList corrFactorElemChildren = corrFactorElem.childNodes();
											if("TgtJamType" == corrFactorType)
											{
												for(int index3 = 0; index3 < corrFactorElemChildren.size(); index3++)
												{
													QDomNode facValue = corrFactorElemChildren.at(index3);
													if(facValue.isElement())
													{
														QDomElement facValueElem = facValue.toElement();
														if("facValue" == facValueElem.tagName())
														{
															QString dd = facValueElem.attribute("TgtJamType");
															double cc = facValueElem.attribute("CorrFac").toDouble();
															corrFactorData.typeValues.insert(facValueElem.attribute("TgtJamType"), facValueElem.attribute("CorrFac").toDouble());
														}
													}
												}
											}
											else 
											{
												for(int index3 = 0; index3 < corrFactorElemChildren.size(); index3++)
												{
													QDomNode facValue = corrFactorElemChildren.at(index3);
													if(facValue.isElement())
													{
														QDomElement facValueElem = facValue.toElement();
														if("facValue" == facValueElem.tagName())
														{
															double max = 0.0;
															double min = 0.0;

															QDomNamedNodeMap attributes = facValueElem.attributes();
															for (int i = 0; i < attributes.count(); ++i) {
																if(attributes.item(i).toAttr().name().toUpper().contains("MAX"))
																{
																	max = attributes.item(i).toAttr().value().toDouble();
																}
																else if(attributes.item(i).toAttr().name().toUpper().contains("MIN"))
																{
																	min = attributes.item(i).toAttr().value().toDouble();
																}
															}

															double corrFac = facValueElem.attribute("CorrFac").toDouble();
															KeyValues corrFacKv;
															corrFacKv.insert("max",		max);
															corrFacKv.insert("min",		min);
															corrFacKv.insert("corrFac",	corrFac);
															corrFactorData.facValues.append(corrFacKv);
														}
													}
												}
											}


											hitProbData.corrFactors.insert(corrFactorType, corrFactorData);
										}
									}
								}

								itemInfo.hitProbTables.insert(hitProbDataType ,hitProbData);
							}
						}
					}

					m_weaponHitProbItems.insert(entClsName, itemInfo);
				}
			}
		}
	}
}

double ImpactProbTable::getImpactProb(double distance, double az, double el, double& tnt) const
{
	QList<DmgProb>::const_iterator iter = dmgProbs.constBegin();
	while(iter != dmgProbs.constEnd())
	{
		if((iter->min <= distance) && (iter->max >= distance))
		{
			tnt = iter->tntKg;
			return JGeoCal::CInterpolate::GetValueIDW(iter->length, 
												      const_cast<double*>(iter->azDeg.data()), 
													  const_cast<double*>(iter->elDeg.data()), 
													  const_cast<double*>(iter->pt.data()), 
													  az, el);
		}
		iter++;
	}
	tnt = 0.0;
	return 0.0;
}

double CorrFactor::factorByType(int type) const
{
	double ret = 0.0;
	QString typestring = QString::number(type);
	if(typeValues.contains(typestring))
	{
		ret = typeValues.value(typestring, 0.0).toDouble();
	}
	return ret; 
}
