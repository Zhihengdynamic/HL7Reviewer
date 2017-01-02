#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "config/ConfigID.h"

class QVariant;
/*
配置管理类。
提供获取配置和更新配置的接口。
*/
class ConfigManager
{
public:
	ConfigManager(void);
	virtual ~ConfigManager(void);

public:
	static ConfigManager* Instance();
	static void ReleaseInstance();

	void GetConfig(quint32 id, QVariant& vVal);
	void UpdateConfig(quint32 id, const QVariant& vVal);

private:
	void InitSettings();
	void WriteSetting(const QString& key, const QVariant& vVal);
	void ReadSettings(const QString& key, QVariant& vVal);

private:
	static ConfigManager* m_pInst;
};

#endif // CONFIGMANAGER_H

