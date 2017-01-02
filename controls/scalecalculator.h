#ifndef QSCALECALC_H
#define QSCALECALC_H

#include <QMap>
#include <QString>

namespace psimpl
{
	class RenderArea;
	/*
	波形标尺计算类。
	*/
	class ScaleCalculator
	{
	public:
		ScaleCalculator();
		~ScaleCalculator();

		void SetWaveMinMaxValue(const QString& waveName, qreal dbMinValue, qreal dbMaxValue);
		void GetWaveMinMaxValue(const QString& waveName, qreal& dbMinValue, qreal& dbMaxValue, qreal dbDataResolution = 0.0);
		
	private:
		void ClearScaleData();
		void GetECGWaveMinMaxValue(qreal& dbMinValue, qreal& dbMaxValue, qreal dbDataResolution);

	private:
		void CalcMinMaxValueByDataResolution(qreal& dbMinValue, qreal& dbMaxValue);

	private:
		struct STUWaveInfo
		{
			qreal dbMinValue;
			qreal dbMaxValue;

			QString strWaveName;
			qreal dbDataResolution;//对于ECG，1单位大小的波形数据是多少（0.001cm）,根据resolution可以精确绘制高度确定的波形
		};

		QMap<QString, STUWaveInfo*> m_mapScaleData;
		qint32 m_iHashNameCharCount;//标识波形名称的前几个字母相同时， 采用同样大小的标尺

		bool m_bRenderMedicalEcgWave;//ECG波形采用医用标准。1厘米对应1mV.
	};
}

#endif // QSCALECALC_H
