#ifndef MAPPINGQUERY_H
#define MAPPINGQUERY_H

#include "configfile_global.h"

#include <QVector>
#include <QColor>
#include <QStringList>
#include <QToolButton>

struct EntityType
{
	EntityType(const QString& str)
	{
		fromString(str);
	}

	EntityType(){}

	int kind;
	int domain;
	int country;
	int category;
	int subCategory;
	int specific;
	int extra;

	QString toString()
	{
		return QString("%1:%2:%3:%4:%5:%6:%7").arg(kind).arg(domain).arg(country).arg(category).arg(subCategory).arg(specific).arg(extra);
	}

	void fromString(const QString& str)
	{
		QStringList typeList=str.split(":");

		kind=typeList[0].toInt();
		domain=typeList[1].toInt();
		country=typeList[2].toInt();
		category=typeList[3].toInt();
		subCategory=typeList[4].toInt();
		specific=typeList[5].toInt();
		extra=typeList[6].toInt();
	}
};

struct ModelDefine
{
	int id;
	QString name;
	QString model_filepath;
	QString icon_filepath;
};

struct JunBiaoId
{
	JunBiaoId():libId(0),markId(0){}
	int libId;
	int markId;

	QString toString()
	{
		return QString("lib:%1,mark:%2").arg(libId).arg(markId);
	}
};

struct EntityModelMapping
{
	int id;
	EntityType entityType;
	int modelId;
	QString modelFilepath;

	JunBiaoId junBiaoId;
};

struct AudioMapping
{
	int id;
	EntityType entityType;
	QString modelFilepath;
};

enum ElementKind{BiaoPei,XiangXing};

enum ForceType{Friendly,Opposing,Neutral};


struct SandboxCategory
{
	int id;
	QString name;
	int kindType;
	int forceType;
};

struct SandboxElement
{
	SandboxElement()
	{
		id=-1;
	}

	int id;
	QString name;
	QString iconFilename;

	SandboxCategory category;
};

struct ElementForceType
{
	int id;
	QString name;
	QColor color;
};

class CONFIGFILE_EXPORT ByyMappingQuery
{
public:
	~ByyMappingQuery();

	static ByyMappingQuery* instance();

	QString getEntityModel(const char* type);
	QString getEntityIcon(const char* type);
	JunBiaoId getEntityJunBiao(const char* type);

	QString getEntityAudio(const char* type);
	QString getFireAudio(const char* type);
	QString getDetonationAudio(const char* type);

	void startTransaction();

	void rollbackTransaction();

	void commitTransaction();


	QVector<ModelDefine> getModelDefines();
	QVector<EntityModelMapping> getEntityMappings();

	QVector<AudioMapping> getAudioMappings(const QString& table);

	bool findModelName(const QString& name);
	void renameModel(int id,const QString& name);
	int addModelDefine(const ModelDefine& md);
	void removeModelDefine(int id);
	void repathModel(int id,const QString& model_filepath);
	void repathModelIcon(int id,const QString& icon_filepath);

	bool findEntityType(EntityType& type);
	int addEntityType(EntityType& type,int modelId,JunBiaoId junbiaoId=JunBiaoId());
	void removeEntityType(int id);
	void reEntityType(int id,EntityType& type);
	void reEntityModel(int id,int modelId);
	void reEntityJunBiao(int id,JunBiaoId junbiaoId);

	bool findAudioMapping(EntityType& type,const QString& table);
	int addAudioMapping(EntityType& type,const QString& filepath,const QString& table);
	void removeAudioMapping(int id,const QString& table);
	void reAudioType(int id,EntityType& type,const QString& table);
	void reAudioFile(int id,const QString& filepath,const QString& table);

	int addSandboxElement(SandboxElement &e);
	void removeSandboxElement(int id);
	void changeSandboxElement(SandboxElement &e);
	QVector<SandboxElement> getSandboxElements();
	int getElementCount(int category);

	QVector<SandboxCategory> getElementCategories();
	int addCategory(const SandboxCategory &category);
	void removeCategory(int id);
	void changeCategory(int id,const QString &name);

	QVector<ElementForceType> getForecTypes();
	

private:
	ByyMappingQuery();
};

#endif // MAPPINGQUERY_H
