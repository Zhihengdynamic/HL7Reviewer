#include "ConfigManager.h"
#include <QVariant>
#include <QSettings>

ConfigManager* ConfigManager::m_pInst = NULL;


ConfigManager::ConfigManager(void)
{
	InitSettings();
}

ConfigManager::~ConfigManager(void)
{
}

ConfigManager* ConfigManager::Instance()
{
	if (NULL == m_pInst)
	{
		m_pInst = new ConfigManager;
	}
	return m_pInst;
}

void ConfigManager::ReleaseInstance()
{
	if (NULL != m_pInst)
	{
		delete m_pInst;
		m_pInst = NULL;
	}
}


/*!
    获取配置。外部需要根据获取值的类型进行转换。
    @para[in]  quint32 id  配置ID
    @para[out]  QVariant & vVal  配置值
    @return  void
*/
void ConfigManager::GetConfig(quint32 id, QVariant& vVal)
{
	//查找内存中的值
	quint32 index = 0;
	for (index=0; index!=sizeof(s_cfgList)/sizeof(s_cfgList[0]); index++)
	{
		if (id == s_cfgList[index].cfgID)
		{
			vVal = s_cfgList[index].cfgVal;
			break;
		}
	}

	//没有找到，从注册表中获取
	if (index == sizeof(s_cfgList)/sizeof(s_cfgList[0]))
	{
		QString idStr = QString("%1").arg(id);
		ReadSettings(idStr, vVal);
	}
}


/*!
    更新配置
    @para[in]  quint32 id  配置ID
    @para[in]  const QVariant & vVal  配置值
    @return  void
*/
void ConfigManager::UpdateConfig(quint32 id, const QVariant& vVal)
{
	for (quint32 index=0; index!=sizeof(s_cfgList)/sizeof(s_cfgList[0]); index++)
	{
		if (id == s_cfgList[index].cfgID)
		{
			switch(s_cfgList[index].cfgValType)
			{
			case QVariant::Bool:
				s_cfgList[index].cfgVal = vVal.value<bool>();
				break;
			case QVariant::Char:
				s_cfgList[index].cfgVal = vVal.value<char>();
				break;
			case QVariant::Double:
				s_cfgList[index].cfgVal = vVal.value<double>();
				break;
			case QVariant::Int:
				s_cfgList[index].cfgVal = vVal.value<int>();
				break;
			case QVariant::LongLong:
				s_cfgList[index].cfgVal = vVal.value<qint64>();
				break;
			case QVariant::String:
				s_cfgList[index].cfgVal = vVal.value<QString>();
				break;
			case QVariant::UInt:
				s_cfgList[index].cfgVal = vVal.value<uint>();
				break;
			default:
				Q_ASSERT(false);
			}

			QString idStr = QString("%1").arg(id);
			WriteSetting(idStr, s_cfgList[index].cfgVal);
		}
	}
}


/*!
    初始化配置。把配置读入内存
    @return  void
*/
void ConfigManager::InitSettings()
{
	for (quint32 index=0; index!=sizeof(s_cfgList)/sizeof(s_cfgList[0]); index++)
	{
		s_cfgList[index].cfgVal = s_cfgList[index].cfgDefVal;

		//
		QString idStr = QString("%1").arg(s_cfgList[index].cfgID);
		QVariant vVal = s_cfgList[index].cfgVal;
		ReadSettings(idStr, vVal);
		switch(s_cfgList[index].cfgValType)
		{
		case QVariant::Bool:
			s_cfgList[index].cfgVal = vVal.value<bool>();
			break;
		case QVariant::Char:
			s_cfgList[index].cfgVal = vVal.value<char>();
			break;
		case QVariant::Double:
			s_cfgList[index].cfgVal = vVal.value<double>();
			break;
		case QVariant::Int:
			s_cfgList[index].cfgVal = vVal.value<int>();
			break;
		case QVariant::LongLong:
			s_cfgList[index].cfgVal = vVal.value<qint64>();
			break;
		case QVariant::String:
			s_cfgList[index].cfgVal = vVal.value<QString>();
			break;
		case QVariant::UInt:
			s_cfgList[index].cfgVal = vVal.value<uint>();
			break;
		default:
			Q_ASSERT(false);
		}
	}
}


/*!
    写入配置。把配置写入到注册表
    @para[in]  const QString & key  
    @para[in]  const QVariant & vVal  
    @return  void
*/
void ConfigManager::WriteSetting(const QString& key, const QVariant& vVal)
{
	QSettings settings;
	settings.setValue(key, vVal);
}


/*!
    从注册表读取配置。
    @para[in]  const QString & key  
    @para[out]  QVariant & vVal  
    @return  void
*/
void ConfigManager::ReadSettings(const QString& key, QVariant& vVal)
{
	QSettings settings;
	vVal = settings.value(key, vVal);
}
