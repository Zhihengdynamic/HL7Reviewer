#include "scalecalculator.h"
#include "config/ConfigManager.h"
#include <QString>

namespace psimpl
{
	static const qreal PRECISION = 1e-6;

	ScaleCalculator::ScaleCalculator()
	{
		QVariant vVal;
		ConfigManager::Instance()->GetConfig(ID_HASH_NAME_CHAR_COUNT, vVal);
		m_iHashNameCharCount = vVal.toInt();

		ConfigManager::Instance()->GetConfig(ID_RENDER_MEDICAL_ECG_WAVE, vVal);
		m_bRenderMedicalEcgWave = vVal.toBool();
	}

	ScaleCalculator::~ScaleCalculator()
	{
		ClearScaleData();
	}

	void ScaleCalculator::SetWaveMinMaxValue(const QString& waveName, qreal dbMinValue, qreal dbMaxValue)
	{
		qDebug("Items in list: %d\n", m_mapScaleData.size());
        qDebug("Items QStringName: %s, iMin =%f, iMax=%f\n ", qPrintable(waveName), dbMinValue, dbMaxValue);
		
		if (!waveName.isEmpty())
		{
			QString waveNameHash = waveName.left(m_iHashNameCharCount);
			STUWaveInfo* pValue = NULL;

			QMap<QString, STUWaveInfo*>::iterator it=m_mapScaleData.find(waveNameHash);
			if (m_mapScaleData.end() != it)
			{
				pValue = it.value();
				if (pValue->dbMinValue > dbMinValue)
				{
					pValue->dbMinValue = dbMinValue;
				}
				if (pValue->dbMaxValue < dbMaxValue)
				{
					pValue->dbMaxValue = dbMaxValue;
				}

                qDebug("#map:  QStringName: %s, iMin =%f, iMax=%f \n", qPrintable(waveNameHash), pValue->dbMinValue, pValue->dbMaxValue);
			}
			else
			{
				pValue = new STUWaveInfo;
				pValue->dbMinValue = dbMinValue;
				pValue->dbMaxValue = dbMaxValue;
				m_mapScaleData.insert(waveNameHash, pValue);
                qDebug("#map:  QStringName: %s, iMin =%f, iMax=%f \n", qPrintable(waveNameHash), pValue->dbMinValue, pValue->dbMaxValue);
			}
		}
		else
		{
			Q_ASSERT(false);
		}
	}

	void ScaleCalculator::GetWaveMinMaxValue(const QString& waveName, qreal& dbMinValue, qreal & dbMaxValue, qreal dbDataResolution)
	{
		if (m_bRenderMedicalEcgWave && waveName.left(3)=="ECG" && dbDataResolution > PRECISION)
		{//ECG波形采用医用标准。1厘米对应1mV.则不用参于下面的计算
			GetECGWaveMinMaxValue(dbMinValue, dbMaxValue, dbDataResolution);
			return;
		}
		else
		{
			QString waveNameHash = waveName.left(m_iHashNameCharCount);
			STUWaveInfo* pValue = NULL;

			QMap<QString, STUWaveInfo*>::iterator it=m_mapScaleData.find(waveNameHash);
			if (m_mapScaleData.end() != it)
			{
				pValue = it.value();
				dbMinValue = pValue->dbMinValue;
				dbMaxValue = pValue->dbMaxValue;
			}
			else
			{
				Q_ASSERT(false);
			}

			//为了使波形在任何情况下，都不充满波形区域的上下高度，本处略微加大最小值和最小值的范围
			if (dbMaxValue > dbMinValue)
			{
				const qreal dbTolerance = (dbMaxValue - dbMinValue) * 0.10;
				dbMinValue -= dbTolerance;
				dbMaxValue += dbTolerance;
			}
			else
			{
				Q_ASSERT(false);
			}
		}
	}

	void ScaleCalculator::ClearScaleData()
	{
		STUWaveInfo* pValue = NULL;
		for (QMap<QString, STUWaveInfo*>::iterator it=m_mapScaleData.begin(); it!=m_mapScaleData.end(); it++)
		{
			pValue = it.value();
			if (NULL != pValue)
			{
				delete pValue;
				pValue = NULL;
			}
		}
		m_mapScaleData.clear();
	}

	/*!
	    根据dbDataResolution和窗口大小及Pixels Per Centimeter，计算ECG窗口的最大值和最小值
	    @para[out]  qreal & dbMinValue  
	    @para[out]  qreal & dbMaxValue  
	    @para[in]  qreal dbDataResolution  
	    @return  void
	*/
	void ScaleCalculator::GetECGWaveMinMaxValue(qreal& dbMinValue, qreal& dbMaxValue, qreal dbDataResolution)
	{
		if (dbDataResolution > PRECISION)
		{
			//假定每厘米有 Pixels个点，dataResolution表示:1单位大小的波形数据是多少（0.001cm）。
			//对于波形数据x, 应画的像素点数为y,
			//则 y = x * dataResolution / 1000 * Pixels
			//对于z个像素，对应的数值大小为: z*x/y= 1000*z / （dataResolution*Pixels）
			static const qint32 FACTOR = 1000; 

			QVariant vVal;
			ConfigManager::Instance()->GetConfig(ID_VER_PIXELS_PER_CM, vVal);
			qreal dbVerPixelsPerCentimeter = vVal.toDouble();

			//获取波形区域的窗口高度（像素数）
			ConfigManager::Instance()->GetConfig(ID_RENDER_AREA_HEIGHT, vVal);
			qint32 renderAreaHeight = vVal.toInt();

			dbMaxValue = FACTOR * (renderAreaHeight/2.0) / (dbDataResolution * dbVerPixelsPerCentimeter);
			dbMinValue = -dbMaxValue;
		}
		else
		{
			Q_ASSERT(false);
		}
	}
}
