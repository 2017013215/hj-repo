#include "StdAfx.h"
#include "mainModelVisualizer.h"
#include "entityObject.h"

#include "entitySettings.h"

ByyMainModelVisualizer::ByyMainModelVisualizer( ByyEntityObject& entObj,ByyIG& ig ) 
	: ByyStateVisualizer(entObj,ig)
{
	
}

ByyMainModelVisualizer::~ByyMainModelVisualizer()
{

}

const QString& ByyMainModelVisualizer::theType()
{
	static QString _type("MainModel");
	return _type;
}

const QString& ByyMainModelVisualizer::type()
{
	return theType();
}