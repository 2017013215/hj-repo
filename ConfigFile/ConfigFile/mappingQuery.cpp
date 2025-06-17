#include "mappingQuery.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>
#include <iostream>
using namespace std;

ByyMappingQuery::ByyMappingQuery()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("../data/config/Mapping.db");
	bool ok = db.open();
}

ByyMappingQuery::~ByyMappingQuery()
{
	QSqlDatabase::database().close();
}

ByyMappingQuery* ByyMappingQuery::instance()
{
	static ByyMappingQuery _instance;
	return &_instance;
}

QString ByyMappingQuery::getEntityModel( const char* type )
{
	QStringList types=QString(type).split(":");

	QSqlQuery query(QString("select ModelDefine.model_filepath from EntityMapping "
		"INNER JOIN ModelDefine ON EntityMapping.modelId=ModelDefine.id WHERE"
		" (kind=%1 OR kind=-1) AND (domain=%2 OR domain=-1)"
		" AND (country=%3 OR country=-1) AND (category=%4 OR category=-1)"
		" AND (subCategory=%5 OR subCategory=-1) AND (specific=%6 OR specific=-1) AND (extra=%7 OR extra=-1)"
		" ORDER BY kind desc ,domain desc,country desc,category desc,subCategory desc,specific desc,extra desc")
		.arg(types[0].toInt())
		.arg(types[1].toInt())
		.arg(types[2].toInt())
		.arg(types[3].toInt())
		.arg(types[4].toInt())
		.arg(types[5].toInt())
		.arg(types[6].toInt()));

	query.next();

	return query.value(0).toString(); 
}

QString ByyMappingQuery::getEntityIcon( const char* type )
{
	QStringList types=QString(type).split(":");

	QSqlQuery query(QString("select ModelDefine.icon_filepath from EntityMapping "
		"INNER JOIN ModelDefine ON EntityMapping.modelId=ModelDefine.id WHERE"
		" (kind=%1 OR kind=-1) AND (domain=%2 OR domain=-1)"
		" AND (country=%3 OR country=-1) AND (category=%4 OR category=-1)"
		" AND (subCategory=%5 OR subCategory=-1) AND (specific=%6 OR specific=-1) AND (extra=%7 OR extra=-1)"
		" ORDER BY kind desc ,domain desc,country desc,category desc,subCategory desc,specific desc,extra desc")
		.arg(types[0].toInt())
		.arg(types[1].toInt())
		.arg(types[2].toInt())
		.arg(types[3].toInt())
		.arg(types[4].toInt())
		.arg(types[5].toInt())
		.arg(types[6].toInt()));

	while (query.next()) 
	{
		if (!query.value(0).toString().isEmpty())
		{
			return query.value(0).toString();
		}
	}

	return QString();
}

QString ByyMappingQuery::getEntityAudio( const char* type )
{
	QStringList types=QString(type).split(":");

	QSqlQuery query(QString("select filepath from EntityAudioMapping WHERE"
		" (kind=%1 OR kind=-1) AND (domain=%2 OR domain=-1)"
		" AND (country=%3 OR country=-1) AND (category=%4 OR category=-1)"
		" AND (subCategory=%5 OR subCategory=-1) AND (specific=%6 OR specific=-1) AND (extra=%7 OR extra=-1)"
		" ORDER BY kind desc ,domain desc,country desc,category desc,subCategory desc,specific desc,extra desc")
		.arg(types[0].toInt())
		.arg(types[1].toInt())
		.arg(types[2].toInt())
		.arg(types[3].toInt())
		.arg(types[4].toInt())
		.arg(types[5].toInt())
		.arg(types[6].toInt()));

	query.next();

	return query.value(0).toString();
}

QString ByyMappingQuery::getFireAudio( const char* type )
{
	QStringList types=QString(type).split(":");

	QSqlQuery query(QString("select filepath from FireAudioMapping WHERE"
		" (kind=%1 OR kind=-1) AND (domain=%2 OR domain=-1)"
		" AND (country=%3 OR country=-1) AND (category=%4 OR category=-1)"
		" AND (subCategory=%5 OR subCategory=-1) AND (specific=%6 OR specific=-1) AND (extra=%7 OR extra=-1)"
		" ORDER BY kind desc ,domain desc,country desc,category desc,subCategory desc,specific desc,extra desc")
		.arg(types[0].toInt())
		.arg(types[1].toInt())
		.arg(types[2].toInt())
		.arg(types[3].toInt())
		.arg(types[4].toInt())
		.arg(types[5].toInt())
		.arg(types[6].toInt()));

	query.next();

	return query.value(0).toString();
}

QString ByyMappingQuery::getDetonationAudio( const char* type )
{
	QStringList types=QString(type).split(":");

	QSqlQuery query(QString("select filepath from DetonationAudioMapping WHERE"
		" (kind=%1 OR kind=-1) AND (domain=%2 OR domain=-1)"
		" AND (country=%3 OR country=-1) AND (category=%4 OR category=-1)"
		" AND (subCategory=%5 OR subCategory=-1) AND (specific=%6 OR specific=-1) AND (extra=%7 OR extra=-1)"
		" ORDER BY kind desc ,domain desc,country desc,category desc,subCategory desc,specific desc,extra desc")
		.arg(types[0].toInt())
		.arg(types[1].toInt())
		.arg(types[2].toInt())
		.arg(types[3].toInt())
		.arg(types[4].toInt())
		.arg(types[5].toInt())
		.arg(types[6].toInt()));

	query.next();

	return query.value(0).toString();
}

void ByyMappingQuery::startTransaction()
{
	QSqlQuery query("BEGIN TRANSACTION");
}

void ByyMappingQuery::rollbackTransaction()
{
	QSqlQuery query("ROLLBACK TRANSACTION");
}

void ByyMappingQuery::commitTransaction()
{
	QSqlQuery query("COMMIT TRANSACTION");
}

QVector<ModelDefine> ByyMappingQuery::getModelDefines()
{
	QVector<ModelDefine> _result;

	QSqlQuery query("select * from ModelDefine");

	while(query.next())
	{
		ModelDefine def;

		def.id=query.value(0).toInt();
		def.name=query.value(1).toString();
		def.model_filepath=query.value(2).toString();
		def.icon_filepath=query.value(3).toString();

		_result.append(def);
	}

	return _result;
}

JunBiaoId ByyMappingQuery::getEntityJunBiao( const char* type )
{
	QStringList types=QString(type).split(":");

	QSqlQuery query(QString("select mgs_lib_id,mgs_mark_id from EntityMapping WHERE"
		" (kind=%1 OR kind=-1) AND (domain=%2 OR domain=-1)"
		" AND (country=%3 OR country=-1) AND (category=%4 OR category=-1)"
		" AND (subCategory=%5 OR subCategory=-1) AND (specific=%6 OR specific=-1) AND (extra=%7 OR extra=-1)"
		" ORDER BY kind desc,domain desc,country desc,category desc,subCategory desc,specific desc,extra desc" )
		.arg(types[0].toInt())
		.arg(types[1].toInt())
		.arg(types[2].toInt())
		.arg(types[3].toInt())
		.arg(types[4].toInt())
		.arg(types[5].toInt())
		.arg(types[6].toInt()));

	JunBiaoId id;

	while (query.next())
	{
		id.libId=query.value(0).toInt();
		id.markId=query.value(1).toInt();

		if (id.libId!=0&&id.markId!=0)
		{
			break;
		}
	}

	return id;
}

QVector<EntityModelMapping> ByyMappingQuery::getEntityMappings()
{
	QVector<EntityModelMapping> _result;

	QSqlQuery query("select EntityMapping.*,ModelDefine.name from EntityMapping "
		"INNER JOIN ModelDefine ON EntityMapping.modelId=ModelDefine.id");

	while(query.next())
	{
		EntityModelMapping mapping;

		mapping.id=query.value(0).toInt();
		mapping.entityType.kind=query.value(1).toInt();
		mapping.entityType.domain=query.value(2).toInt();
		mapping.entityType.country=query.value(3).toInt();
		mapping.entityType.category=query.value(4).toInt();
		mapping.entityType.subCategory=query.value(5).toInt();
		mapping.entityType.specific=query.value(6).toInt();
		mapping.entityType.extra=query.value(7).toInt();
		mapping.modelId=query.value(8).toInt();
		mapping.junBiaoId.libId=query.value(9).toInt();
		mapping.junBiaoId.markId=query.value(10).toInt();
		mapping.modelFilepath=query.value(11).toString();

		_result.append(mapping);
	}
	return _result;
}

QVector<AudioMapping> ByyMappingQuery::getAudioMappings( const QString& table )
{
	QVector<AudioMapping> _result;

	QSqlQuery query(QString("select * from ")+table);

	while(query.next())
	{
		AudioMapping mapping;
		
		mapping.id=query.value(0).toInt();
		mapping.entityType.kind=query.value(1).toInt();
		mapping.entityType.domain=query.value(2).toInt();
		mapping.entityType.country=query.value(3).toInt();
		mapping.entityType.category=query.value(4).toInt();
		mapping.entityType.subCategory=query.value(5).toInt();
		mapping.entityType.specific=query.value(6).toInt();
		mapping.entityType.extra=query.value(7).toInt();
		mapping.modelFilepath=query.value(8).toString();

		_result.append(mapping);
	}

	return _result;
}

bool ByyMappingQuery::findModelName( const QString& name )
{
	QSqlQuery query(QString("select * from ModelDefine where name='%1'").arg(name));

	return query.next();
}

void ByyMappingQuery::renameModel( int id,const QString& name )
{
	QSqlQuery query(QString("update ModelDefine set name='%1' where id='%2'").arg(name).arg(id));
}

int ByyMappingQuery::addModelDefine( const ModelDefine& md )
{
	QSqlQuery query(QString("insert into ModelDefine (name,model_filepath,icon_filepath) values ('%1','%2','%3')").arg(md.name).arg(md.model_filepath).arg(md.icon_filepath));

	query.exec("SELECT last_insert_rowid()");
	query.next();
	return query.value(0).toInt();
}

void ByyMappingQuery::removeModelDefine( int id )
{
	QSqlQuery query(QString("delete from ModelDefine where id=%1").arg(id));
}

void ByyMappingQuery::repathModel( int id,const QString& model_filepath)
{
	QSqlQuery query(QString("update ModelDefine set model_filepath='%1' where id='%2'").arg(model_filepath).arg(id));
}

void ByyMappingQuery::repathModelIcon( int id,const QString& icon_filepath )
{
	QSqlQuery query(QString("update ModelDefine set icon_filepath='%1' where id='%2'").arg(icon_filepath).arg(id));
}

bool ByyMappingQuery::findEntityType( EntityType& type )
{
	QSqlQuery query(QString("select * from EntityMapping where kind=%1 and domain=%2 and country=%3 and category=%4 and subCategory=%5 and specific=%6 and extra=%7")
		.arg(type.kind)
		.arg(type.domain)
		.arg(type.country)
		.arg(type.category)
		.arg(type.subCategory)
		.arg(type.specific)
		.arg(type.extra));

	return query.next();
}

int ByyMappingQuery::addEntityType( EntityType& type,int modelId ,JunBiaoId junbiaoId)
{
	QSqlQuery query(QString("insert into EntityMapping (kind,domain,country,category,subCategory,specific,extra,modelId,mgs_lib_id,mgs_mark_id) values (%1,%2,%3,%4,%5,%6,%7,%8,%9,%10)")
		.arg(type.kind)
		.arg(type.domain)
		.arg(type.country)
		.arg(type.category)
		.arg(type.subCategory)
		.arg(type.specific)
		.arg(type.extra)
		.arg(modelId)
		.arg(junbiaoId.libId)
		.arg(junbiaoId.markId));

	query.exec("SELECT last_insert_rowid()");
	query.next();
	return query.value(0).toInt();
}

void ByyMappingQuery::removeEntityType( int id )
{
	QSqlQuery query(QString("delete from EntityMapping where id=%1").arg(id));
}

void ByyMappingQuery::reEntityType( int id,EntityType& type )
{
	QSqlQuery query(QString("update EntityMapping set kind=%1,domain=%2,country=%3,category=%4,subCategory=%5,specific=%6,extra=%7 where id=%9")
		.arg(type.kind)
		.arg(type.domain)
		.arg(type.country)
		.arg(type.category)
		.arg(type.subCategory)
		.arg(type.specific)
		.arg(type.extra)
		.arg(id));
}

void ByyMappingQuery::reEntityModel( int id,int modelId )
{
	QSqlQuery query(QString("update EntityMapping set modelId=%1 where id=%2")
		.arg(modelId)
		.arg(id));
}

void ByyMappingQuery::reEntityJunBiao( int id,JunBiaoId junbiaoId )
{
	QSqlQuery query(QString("update EntityMapping set mgs_lib_id=%1,mgs_mark_id=%2 where id=%3")
		.arg(junbiaoId.libId)
		.arg(junbiaoId.markId)
		.arg(id));
}

bool ByyMappingQuery::findAudioMapping( EntityType& type,const QString& table )
{
	QSqlQuery query(QString("select * from %8 where kind=%1 and domain=%2 and country=%3 and category=%4 and subCategory=%5 and specific=%6 and extra=%7")
		.arg(type.kind)
		.arg(type.domain)
		.arg(type.country)
		.arg(type.category)
		.arg(type.subCategory)
		.arg(type.specific)
		.arg(type.extra)
		.arg(table));

	return query.next();
}

int ByyMappingQuery::addAudioMapping( EntityType& type,const QString& filepath,const QString& table )
{
	QSqlQuery query(QString("insert into %9 (kind,domain,country,category,subCategory,specific,extra,filepath) values (%1,%2,%3,%4,%5,%6,%7,'%8')")
		.arg(type.kind)
		.arg(type.domain)
		.arg(type.country)
		.arg(type.category)
		.arg(type.subCategory)
		.arg(type.specific)
		.arg(type.extra)
		.arg(filepath)
		.arg(table));

	query.exec("SELECT last_insert_rowid()");
	query.next();
	return query.value(0).toInt();
}

void ByyMappingQuery::removeAudioMapping( int id,const QString& table )
{
	QSqlQuery query(QString("delete from %2 where id=%1").arg(id).arg(table));
}

void ByyMappingQuery::reAudioType( int id,EntityType& type,const QString& table )
{
	QSqlQuery query(QString("update %9 set kind=%1,domain=%2,country=%3,category=%4,subCategory=%5,specific=%6,extra=%7 where id=%8")
		.arg(type.kind)
		.arg(type.domain)
		.arg(type.country)
		.arg(type.category)
		.arg(type.subCategory)
		.arg(type.specific)
		.arg(type.extra)
		.arg(id)
		.arg(table));
}

void ByyMappingQuery::reAudioFile( int id,const QString& filepath,const QString& table )
{
	QSqlQuery query(QString("update %3 set filepath='%1' where id='%2'").arg(filepath).arg(id).arg(table));
}

QVector<SandboxElement> ByyMappingQuery::getSandboxElements()
{
	QVector<SandboxElement> _result;

	QSqlQuery query("Select count(*) from SandboxElements");
	query.next();

	_result.resize(query.value(0).toInt());
	
	query.exec(QString("Select * from SandboxElements INNER JOIN SandboxCategories ON SandboxElements.category=SandboxCategories.id"));

	for(int i=0;query.next();++i)
	{
		_result[i].id=query.value(0).toInt();
		_result[i].name=query.value(1).toString();
		_result[i].iconFilename=query.value(2).toString();
		_result[i].category.id=query.value(4).toInt();
		_result[i].category.name=query.value(5).toString();
		_result[i].category.kindType=query.value(6).toInt();
		_result[i].category.forceType=query.value(7).toInt();
	}

	return _result;
}

int ByyMappingQuery::addSandboxElement( SandboxElement &e )
{
	QSqlQuery query;

	query.exec(QString("insert into SandboxElements (name,iconfilename,category) values('%1','%2',%3);")
		.arg(e.name).arg(e.iconFilename).arg(e.category.id));

	query.exec("SELECT last_insert_rowid()");
	query.next();

	return query.value(0).toInt();
}

void ByyMappingQuery::removeSandboxElement( int id )
{
	QSqlQuery query(QString("delete from SandboxElements where id=%1").arg(id));
}

void ByyMappingQuery::changeSandboxElement( SandboxElement &e )
{
	QSqlQuery query;

	query.exec(QString("update SandboxElements set name='%1',iconfilename='%2',category=%3 where id=%4")
		.arg(e.name)
		.arg(e.iconFilename)
		.arg(e.category.id)
		.arg(e.id)
		);
}

int ByyMappingQuery::addCategory( const SandboxCategory &category )
{
	QSqlQuery query;

	query.exec(QString("insert into SandboxCategories (name,kindtype,forcetype) values('%1',%2,%3);")
		.arg(category.name)
		.arg(category.kindType)
		.arg(category.forceType));

	query.exec("SELECT last_insert_rowid()");
	query.next();
	return query.value(0).toInt();
}

void ByyMappingQuery::removeCategory( int id )
{
	QSqlQuery sqlQuery;
	sqlQuery.exec(QString("delete from SandboxCategories where id=%1").arg(id));
}

void ByyMappingQuery::changeCategory( int id,const QString &name )
{
	QSqlQuery sqlQuery;
	sqlQuery.exec(QString("update SandboxCategories set name='%1' where id=%2").arg(name).arg(id));
}

QVector<SandboxCategory> ByyMappingQuery::getElementCategories()
{
	QVector<SandboxCategory> _result;

	QSqlQuery query("Select count(*) from SandboxCategories");
	query.next();

	_result.resize(query.value(0).toInt());

	query.exec(QString("Select * from SandboxCategories;"));

	for(int i=0;query.next();++i)
	{
		_result[i].id=query.value(0).toInt();
		_result[i].name=query.value(1).toString();
		_result[i].kindType=query.value(2).toInt();
		_result[i].forceType=query.value(3).toInt();
	}

	return _result;
}

QVector<ElementForceType> ByyMappingQuery::getForecTypes()
{
	QVector<ElementForceType> _result;

	_result.resize(3);

	_result[0].id=Friendly;
	_result[0].name=QString::fromLocal8Bit("红方");
	_result[0].color=Qt::red;

	_result[1].id=Opposing;
	_result[1].name=QString::fromLocal8Bit("蓝方");
	_result[1].color=Qt::blue;

	_result[2].id=Neutral;
	_result[2].name=QString::fromLocal8Bit("第三方");
	_result[2].color=Qt::green;

	return _result;
}

int ByyMappingQuery::getElementCount( int category )
{
	QSqlQuery query(QString("Select count(*) from SandboxElements where category=%1").arg(category));

	query.next();

	return query.value(0).toInt();
}
