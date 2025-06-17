//TargetControlEvt.h: interface for the CTargetControlEvt module.
//!!HDOSE_CLASS(CTargetControlEvt,CCGFBaseEvt)
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_TargetControlEvt_H)
#define AFX_TargetControlEvt_H

#include "snighdose_global.h"

//#include "JSCGFSimEvt/CGFBaseEvt.h"
//{{HDOSE_CLS_DES(CTargetControlEvt)
/*
Author:		sunzhouhe
Version:	1.0
Descript:	键盘控制实体
 */
//}}HDOSE_CLS_DES

namespace JSCGFSim{
class CTargetControlEvt /*目标控制事件 */ : public CEvt 
{
	DECLARE_CREATE;
public:
	CTargetControlEvt(){}
	virtual ~CTargetControlEvt(){}
	void setDirection(int val){m_direction = val;}
	double getDirection(){return m_direction;}
	void setTargetName(string name){m_targetName = name;}
	string getTargetName(){return m_targetName;}
	
private:
	int m_direction;
	string m_targetName;
};
}

IMPLEMENT_CREATE_EX(CTargetControlEvt,CEvt,ClassInit);
#endif
