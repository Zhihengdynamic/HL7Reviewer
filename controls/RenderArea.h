#ifndef RENDERAREA_H
#define RENDERAREA_H


#include <QtGui/QFrame>
#include <QtGui/QPainterPath>
class QPainter;

namespace psimpl
{
	/*
	波形绘制控件。
	完成波形、波形标尺的绘制。
	具备智能标尺的功能。需要外界辅助计算标尺。

	原则：
	本类相当于一个控件，只处理绘制波形的逻辑。尽量不牵涉具体的业务逻辑。
	*/
 
	class RenderArea : public QFrame
	{
		Q_OBJECT
	public:
        RenderArea (QWidget *inParent = 0, Qt::WindowFlags inFlags = 0);

		//智能标尺使用函数
		void SetMinMaxValue(qreal dbMinValue, qreal dbMaxValue);
		void GetMinMaxValue(qreal& dbMinValue, qreal& dbMaxValue);
        void SetScale(qreal lowScale, qreal highScale);
        void GetScale(qreal & lowScale, qreal & highScale) const;

		//必须调用的接口，用于画波形
		void SetSampleRate(const QString& strSampleRate);
		void SetWaveData(const QString & strWaveData);
		void SetPacerPosition(const QString & strPacerPosition);//如果没有Pacer可以不调用或设置空字串

		//可选接口：用于绘制ECG参考标尺，以及标尺文字
		void SetWaveName(const QString & strWaveName);
		void SetDataResolution(const QString& strResolution);
		qreal GetDataResolution() const;
		void SetUnits(const QString& strUnits);
		
	public:
		virtual QSize sizeHint() const;

	protected:
		void paintEvent(QPaintEvent *inEvent);
		void resizeEvent(QResizeEvent * event);

	private:
		void BuildWavePath();
		void CalcMinMaxValue();
		void CalcMinMaxValueByWaveData(qreal& dbMinValue, qreal& dbMaxValue);

		void String2RealVector(const QString& strData, QVector<qreal>& vecWave);
		void String2IntVector(const QString& strData, QVector<qint32>& vecWave);
		
		void DrawGridAndText(QPainter& painter);
		void Draw1cmRuler(QPainter& painter);
		void DrawWaveAndPacer(QPainter& painter);
		void DrawScaleAndWaveSpeedText(QPainter& painter);

	signals:
		void WarningInformation(const QString& info);

	private:
		qint32 m_iRenderSecs;//绘制波形的秒数
		qreal m_dbMaxValue;//波形数据的最大值
		qreal m_dbMinValue;//波形数据的最小值

        qreal m_dbHighScale;
        qreal m_dbLowScale;
				
		qint32 m_iSampleRate;//波形的采样率
		QVector<qreal> m_waveData;//波形数据
		QVector<qint32> m_pacerPosition;//PACE数据。一个偏移量数组

		QString m_strWaveName;
		qreal m_dbDataResolution;//
		QString m_strUnits;

		QVector<qreal> m_xPosArray;
		QVector<qreal> m_yPosArray;
		QPainterPath m_wavePath;//绘制的路径

	private:
		qreal m_dbHorPixelsPerCentimeter;//水平方向多少像素1个厘米
		qreal m_dbVerPixelsPerCentimeter;//竖直方向多少像素1个厘米
	};

} // namespace psimpl


#endif // RENDERAREA_H
