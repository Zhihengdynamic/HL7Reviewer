#include "RenderArea.h"
#include <QtGui/QPainter>
#include "config/ConfigManager.h"

namespace psimpl
{
	static const qreal PRECISION = 1e-6;
	
	RenderArea::RenderArea (QWidget *inParent, Qt::WindowFlags inFlags)
		: QFrame (inParent, inFlags)
	{
		m_dbMaxValue = 0;
		m_dbMinValue = 0;
        m_dbHighScale = 0;
        m_dbLowScale = 0;
		m_iSampleRate = 0;
		m_dbDataResolution = 0.0;

		setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));

		QVariant vVal;
		ConfigManager::Instance()->GetConfig(ID_MAX_RENDER_SECONDS, vVal);
		m_iRenderSecs = vVal.toInt();

		ConfigManager::Instance()->GetConfig(ID_HOR_PIXELS_PER_CM, vVal);
		m_dbHorPixelsPerCentimeter = vVal.toDouble();

		ConfigManager::Instance()->GetConfig(ID_VER_PIXELS_PER_CM, vVal);
		m_dbVerPixelsPerCentimeter = vVal.toDouble();

		setFixedSize(sizeHint());
	}

	/*!
	    设置最大值和最小值，控制绘制波形的上标尺和下标尺
	    @para[in]  qreal dbMinValue  
	    @para[in]  qreal dbMaxValue  
	    @return  void
	*/
	void RenderArea::SetMinMaxValue(qreal dbMinValue, qreal dbMaxValue)
	{
		if ((m_dbMaxValue-dbMaxValue>PRECISION || m_dbMaxValue-dbMaxValue<(-PRECISION))//m_dbMaxValue !=dbMaxValue
			|| (m_dbMinValue-dbMinValue>PRECISION || m_dbMinValue-dbMinValue<(-PRECISION)) )//m_dbMinValue !=dbMinValue
		{
			m_dbMaxValue = dbMaxValue;
			m_dbMinValue = dbMinValue;

			BuildWavePath();
			update();
		}
	}

	/*!
	    获取绘制波形的最大值和最小值
	    @para[out]  qreal & dbMinValue  
	    @para[out]  qreal & dbMaxValue  
	    @return  void
	*/
	void RenderArea::GetMinMaxValue(qreal& dbMinValue, qreal& dbMaxValue)
	{
		dbMinValue = m_dbMinValue;
		dbMaxValue = m_dbMaxValue;
	}

    void RenderArea::SetScale(qreal lowScale, qreal highScale)
    {
        m_dbLowScale = lowScale;
        m_dbHighScale = highScale;
    }

    void RenderArea::GetScale(qreal & lowScale, qreal & highScale) const
    {
        lowScale = m_dbLowScale;
        highScale = m_dbHighScale;
    }

	/*!
	    设置波形的采样率
	    @para[in]  const QString & strSampleRate  
	    @return  void
	*/
	void RenderArea::SetSampleRate(const QString& strSampleRate)
	{
		m_iSampleRate = strSampleRate.toInt();
	}

	/*!
	    设置波形数据，数据间以“，”分割
	    @para[in]  const QString & strWaveData  
	    @return  void
	*/
	void RenderArea::SetWaveData(const QString & strWaveData)
	{
		m_dbMinValue = 0;
		m_dbMaxValue = 0;

		String2RealVector(strWaveData, m_waveData);
		if (m_waveData.size() > (m_iRenderSecs * m_iSampleRate))
		{
			emit WarningInformation(QString(tr("Wave seconds exceed %1 seconds. "
				"The wave exceed %1 seconds will be trim.")).arg(m_iRenderSecs));
		}

		CalcMinMaxValue();
		BuildWavePath();
		update();
	}

	/*!
	    设置波形Pacer数据，数据间以“，”分割
	    @para[in]  const QString & strPacerPosition  
	    @return  void
	*/
	void RenderArea::SetPacerPosition(const QString & strPacerPosition)
	{
		String2IntVector(strPacerPosition, m_pacerPosition);
	}

	/*!
	    设置波形名称。如果是ECG波形，则画1cm的参考标尺。
	    @para[in]  const QString & strWaveName  
	    @return  void
	*/
	void RenderArea::SetWaveName(const QString & strWaveName)
	{
		m_strWaveName = strWaveName;
	}

	/*!
	    设置波形数据的分辨率。
		用于绘制波形标尺文字。即实际单位下的波形采样值=（波形数据*分辨率）。
	    @para[in]  const QString & strResolution  
	    @return  void
	*/
	void RenderArea::SetDataResolution(const QString& strResolution)
	{
		m_dbDataResolution = strResolution.toDouble();
	}

	qreal RenderArea::GetDataResolution() const
	{
		return m_dbDataResolution;
	}

	/*!
	    设置波形采样值的单位。
		用于绘制波形标尺文字
	    @para[in]  const QString & strUnits  
	    @return  void
	*/
	void RenderArea::SetUnits(const QString& strUnits)
	{
		m_strUnits = strUnits;
	}

	/*!
	    把字符串，按照“，”分割符，转换成一组浮点数，放到容器中。
	    @para[in]  const QString & strData  
	    @para[out]  QVector<qreal> & vecWave  
	    @return  void
	*/
	void RenderArea::String2RealVector(const QString& strData, QVector<qreal>& vecWave)
	{
		vecWave.clear();
		QStringList list = strData.split(",", QString::SkipEmptyParts);
		for (qint32 i = 0; i < list.size(); ++i)
		{
			vecWave.push_back(list.at(i).toDouble());
		}
	}

	void RenderArea::String2IntVector(const QString& strData, QVector<qint32>& vecWave)
	{
		vecWave.clear();
		QStringList list = strData.split(",", QString::SkipEmptyParts);
		for (qint32 i = 0; i < list.size(); ++i)
		{
			vecWave.push_back(list.at(i).toInt());
		}
	}

	/*!
	    构建绘制路径
	    @return  void
	*/
	void RenderArea::BuildWavePath()
	{
		QPainterPath path;
		qint32 iPointCount = m_waveData.size();
		if (iPointCount > 0)
		{
			if (iPointCount > (m_iRenderSecs * m_iSampleRate))
			{
				iPointCount = (m_iRenderSecs * m_iSampleRate);
			}

			m_xPosArray.clear();
			const qreal dbWidth = width();
			for (qint32 i=0; i<iPointCount; i++)
			{
				m_xPosArray.push_back(dbWidth/(m_iRenderSecs*m_iSampleRate)*i);
			}

			qint32 iYScale = m_dbMaxValue - m_dbMinValue;
			const qreal dbHeight = height();

			m_yPosArray.clear();
			for (qint32 i=0; i<iPointCount; i++)
			{
				qreal iYPos = dbHeight/2;
				if (0 != iYScale)
				{
					iYPos = dbHeight - (m_waveData[i]-m_dbMinValue)*dbHeight/iYScale;
				}
				m_yPosArray.push_back(iYPos);
			}

			//////////////////////////////////////////////////////////////////////////
			path.moveTo(m_xPosArray[0], m_yPosArray[0]);
			for (qint32 i=1; i<iPointCount; i++)
			{
				path.lineTo(m_xPosArray[i], m_yPosArray[i]);
			}
		}

		m_wavePath = path;
	}

	/*!
	    计算波形数据的最大值和最小值。
		1.如果没有设置m_dbDataResolution,则使用本波形数据的最小值和最大值。
		2.如果设置了m_dbDataResolution，则根据m_dbDataResolution计算出最小值和最大值，使波形的绘制满足m_dbDataResolution要求
	    @return  void
	*/
	void RenderArea::CalcMinMaxValue()
	{
		CalcMinMaxValueByWaveData(m_dbMinValue, m_dbMaxValue);
	}

	/*!
	    找到波形数据的最大值和最小值
	    @para[out]  qreal & dbMinValue  
	    @para[out]  qreal & dbMaxValue  
	    @return  void
	*/
	void RenderArea::CalcMinMaxValueByWaveData(qreal& dbMinValue, qreal& dbMaxValue)
	{
		qint32 iPointCount = m_waveData.size();

		if (iPointCount > 0)
		{
			dbMaxValue = m_waveData[0];
			dbMinValue = m_waveData[0];
		}

		for (qint32 i=1; i<iPointCount; i++)
		{
			if (m_waveData[i] > dbMaxValue)
			{
				dbMaxValue = m_waveData[i];
			}
			else if (m_waveData[i] < dbMinValue)
			{
				dbMinValue = m_waveData[i];
			}
		}
	}

	/*!
	    绘制事件
	    @para[in]  QPaintEvent *  
	    @return  void
	*/
	void RenderArea::paintEvent(QPaintEvent * /*inEvent*/)
	{
		QPainter painter (this);
		painter.setRenderHint(QPainter::Antialiasing);

		QPen pen;  // creates a default pen
		pen.setCapStyle(Qt::RoundCap);
		pen.setJoinStyle(Qt::MiterJoin);
		painter.setPen(pen);

		//画网格和网格文字
		DrawGridAndText(painter);
		
		//画一个参考刻度，固定画1厘米长
		Draw1cmRuler(painter);

		//画波形
		DrawWaveAndPacer(painter);
		
		//画标尺文字
		DrawScaleAndWaveSpeedText(painter);
	}
	

	/*!
	    画网格和网格上的文字
	    @para[in]  QPainter & painter  
	    @return  void
	*/
	void RenderArea::DrawGridAndText(QPainter& painter)
	{
		const qreal WIDTH = width();
		const qreal HEIGHT = height();

		//设置画笔和画刷
		QPen pen(painter.pen());
		pen.setStyle(Qt::SolidLine);
		pen.setWidth(2);
		pen.setBrush(Qt::darkGreen);
		painter.setPen(pen);

		//画边框
		QPainterPath framePath;
		framePath.addRect(0, 0, WIDTH, HEIGHT);
		painter.drawPath(framePath);

		//画网络
		pen.setStyle(Qt::DotLine);
		pen.setWidth(1);
		painter.setPen(pen);
		const qint32 yGridCount = 4;
		for (qint32 i=1; i<yGridCount; i++)
		{//画横向网格
			qreal yGridPos = HEIGHT/yGridCount*i;
			painter.drawLine(QPointF(0, yGridPos), QPointF(WIDTH, yGridPos));
		}

		const qint32 xGridCount = m_iRenderSecs*5;
		for (qint32 i=1; i<xGridCount; i++)
		{//画竖向网格
			qreal xGridPos = WIDTH/xGridCount*i;
			painter.drawLine(QPointF(xGridPos, 0), QPointF(xGridPos, HEIGHT));
		}

		//画刻度和刻度文字
		pen.setStyle(Qt::SolidLine);
		pen.setWidth(2);
		painter.setPen(pen);
		qreal xPos = 0;
		for (qint32 i=1; i<m_iRenderSecs; i++)
		{
			xPos = WIDTH/m_iRenderSecs*i;
			painter.drawLine(QPointF(xPos, HEIGHT), QPointF(xPos, HEIGHT-5));

			painter.drawText(QPointF(xPos+5, HEIGHT-5), QString("%1s").arg(i));
		}
		painter.drawText(QPointF(WIDTH-20, HEIGHT-5), QString("%1s").arg(m_iRenderSecs));
	}

	/*!
	    画1cm的参考标尺
	    @para[in]  QPainter & painter 
	    @return  void
	*/
	void RenderArea::Draw1cmRuler(QPainter& painter)
	{
		if (m_strWaveName.left(3) == "ECG")
		{
			QVariant vVal;
			ConfigManager::Instance()->GetConfig(ID_RENDER_MEDICAL_ECG_WAVE, vVal);
			if (vVal.toBool())
			{
				const qreal HEIGHT = height();

				qint32 xPos = 20;
				QPointF pt1(xPos, HEIGHT/2-m_dbVerPixelsPerCentimeter/2);
				QPointF pt2(xPos, HEIGHT/2+m_dbVerPixelsPerCentimeter/2);

				QPen pen(painter.pen());
				pen.setStyle(Qt::SolidLine);
				pen.setBrush(Qt::black);
				pen.setWidth(2);
				painter.setPen(pen);
				painter.drawLine(pt1, pt2);

				QPointF positionF(xPos+5, HEIGHT/2);
				painter.drawText(positionF, tr("1mV"));
			}
		}
	}

	/*!
	    画波形和Pacer
	    @para[in]  QPainter & painter  
	    @return  void
	*/
	void RenderArea::DrawWaveAndPacer(QPainter& painter)
	{
		if (m_iSampleRate < 0 || m_waveData.size() <= 0)
		{
			return;
		}

		if (!m_wavePath.isEmpty()) 
		{
			painter.setPen(Qt::darkBlue);
			painter.drawPath(m_wavePath);
		}

		//画Pacer
		QPen pen(painter.pen());
		painter.setPen(Qt::magenta);
		for (qint32 i=0; i<m_pacerPosition.size(); i++)
		{
			const qreal WIDTH = width();
			const qreal HEIGHT = height();

			qint32 iPacerPos = m_pacerPosition[i];
			qint32 xPos = WIDTH/(m_iRenderSecs*m_iSampleRate)*iPacerPos;

			//固定画1厘米长
			QPointF pt1(xPos, HEIGHT/2-m_dbVerPixelsPerCentimeter/2);
			QPointF pt2(xPos, HEIGHT/2+m_dbVerPixelsPerCentimeter/2);

			painter.drawLine(pt1, pt2);
		}
	}

	/*!
	    画标尺文字和波形走速
	    @para[in]  QPainter & painter  
	    @return  void
	*/
	void RenderArea::DrawScaleAndWaveSpeedText(QPainter& painter)
	{
		QPen pen(painter.pen());
		pen.setBrush(Qt::black);
		painter.setPen(pen);
		if (m_dbDataResolution > PRECISION && !m_strUnits.isEmpty())
		{
			const qreal HEIGHT = height();

			QString strHighScale;
			QString strLowScale;
			QString strWaveSpeed;

			QVariant vVal;
			//波形走速
			ConfigManager::Instance()->GetConfig(ID_WAVE_SPEED, vVal);
			strWaveSpeed = QString(tr("  WaveSpeed: %1 mm/s")).arg(vVal.toInt());
			
			//标尺
			ConfigManager::Instance()->GetConfig(ID_RENDER_WAVE_SCALE_TEXT, vVal);
			if (vVal.toBool())
			{
				strHighScale = QString(tr("%1 %2")).arg(m_dbMaxValue*m_dbDataResolution).arg(m_strUnits);
				strLowScale = QString(tr("%1 %2")).arg(m_dbMinValue*m_dbDataResolution).arg(m_strUnits);
			}
			painter.drawText(QPointF(5,10), strHighScale+strWaveSpeed);
			painter.drawText(QPointF(5,HEIGHT-3), strLowScale);
		}
	}

	void RenderArea::resizeEvent(QResizeEvent * /*event*/)
	{
		BuildWavePath();
	}

	QSize RenderArea::sizeHint() const
	{
		QVariant vVal;
		ConfigManager::Instance()->GetConfig(ID_RENDER_AREA_HEIGHT, vVal);
		qint32 renderAreaHeight = vVal.toInt();

		ConfigManager::Instance()->GetConfig(ID_WAVE_SPEED, vVal);
		qreal dbWaveSpeed = vVal.toDouble();
		qreal renderAreaWidth = dbWaveSpeed / 10.0 * m_dbHorPixelsPerCentimeter * m_iRenderSecs;

		return QSize(renderAreaWidth, renderAreaHeight);
	}

} // namespace psimpl
