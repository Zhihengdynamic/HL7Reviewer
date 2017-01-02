#include "qwaveswidget.h"
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include "RenderArea.h"
#include "scalecalculator.h"
#include "config/ConfigManager.h"
#include "controls/qwavewidget.h"

using namespace psimpl;

static const QString WAVEFORMS                       = QString("Waveforms");
static const QString MEDIANBEATTEMPLATES             = QString("MedianBeatTemplates");
static const qint32  MAX_SHOW_WAVE_COUNT             = 32;//最大支持显示的波形条数
static const qint32  LAYOUT_SPACING                  = 6;
static const qint32  LAYOUT_CONTENT_MARGINS          = 11;


QWavesWidget::QWavesWidget(QWidget* parent)
	: QWidget(parent)
	, m_iCurWaveCount(0)
{
	InitComponent();
}

QWavesWidget::~QWavesWidget()
{

}


/*!
    初始化控件的显示
    @return  void
*/
void QWavesWidget::InitComponent()
{
	QGridLayout* gridLayout = new QGridLayout(this);
	gridLayout->setSpacing(LAYOUT_SPACING);
	gridLayout->setContentsMargins(11, 11, 11, 11);

	for (qint32 i=0; i<MAX_SHOW_WAVE_COUNT; i++)
	{
		//波形名称
		QWaveWidget* waveWidgetPtr = new QWaveWidget(this);
		gridLayout->addWidget(waveWidgetPtr, i, 0, 1, 1);
		m_vecWaveWidget.push_back(waveWidgetPtr);

		//信号连接
		connect(GetWaveWidget(i), SIGNAL(WarningInformation(const QString&)), this, SLOT(OnWarningInformation(const QString&)));
	}

	//setFixedSize(sizeHint());
}


/*!
    收到错误消息响应函数
    @para[in]  const QString & info  
	@return  void
*/
void QWavesWidget::OnWarningInformation(const QString& info)
{
	emit WarningInformation(info);
}


/*!
    根据位置索引，获取波形控件对象
    @para[in]  qint32 iChannel  
    @return  QLabel*
*/
QWaveWidget* QWavesWidget::GetWaveWidget(qint32 iChannel) const
{
    if (iChannel>=0 && iChannel<m_vecWaveWidget.size())
    {
        return m_vecWaveWidget[iChannel];
    }
    return NULL;
}


/*!
    在主界面绘制波形。
    @para[in]  const QVector<STUWaveform * > & wavefroms  
    @return  void
*/
void QWavesWidget::DrawWaveforms(const QVector<STUWaveform*>& wavefroms)
{
	const qint32 iXmlWaveCount = wavefroms.size();
	m_iCurWaveCount = (iXmlWaveCount < MAX_SHOW_WAVE_COUNT) ? iXmlWaveCount : MAX_SHOW_WAVE_COUNT;

	if (iXmlWaveCount > MAX_SHOW_WAVE_COUNT)
	{
		emit WarningInformation(QString(tr("Wave count exceed maximum support count : %1.")).arg(MAX_SHOW_WAVE_COUNT));
	}
	else if (0 == iXmlWaveCount)
	{
		emit WarningInformation(QString(tr("Not found %1 element or empty element.")).arg(WAVEFORMS));
	}

	for (qint32 i=0; i<m_iCurWaveCount; i++)
	{
		STUWaveform* pWaveform = wavefroms.at(i);
		FillWaveformData(i, pWaveform);
	}

	//自动波形标尺
	QVariant vVal;
	ConfigManager::Instance()->GetConfig(ID_INTELIIGENT_CALCULATE_SCALE, vVal);
	if (vVal.toBool())
	{
		ProcessIntelligentCalculateScale(m_iCurWaveCount);
	}

	//根据波形个数，调整尺寸
	//setFixedSize(sizeHint());
}


/*!
    绘制波形函数。根据在指定索引的控件，绘制指令的波形
    @para[in]  qint32 iWaveformIndex  
    @para[in]  const STUWaveform * pWaveform  
    @return  void
*/
void QWavesWidget::FillWaveformData(qint32 iWaveformIndex, const STUWaveform* pWaveform)
{
	if (NULL!=pWaveform && iWaveformIndex>=0 && iWaveformIndex<MAX_SHOW_WAVE_COUNT)
	{
		GetWaveWidget(iWaveformIndex)->DrawWaveform(pWaveform);
	}
	else
	{
		Q_ASSERT(false);
	}
}

/*!
    处理智能标尺的绘制。
	即先从各控件中，取得最大值和最小值和波形名称。然后设置到标尺计算类，计算完成后，逐个控件刷新。
    @return  void
*/
void QWavesWidget::ProcessIntelligentCalculateScale(qint32 iWaveCount)
{
	ScaleCalculator scaleCalcualtor;
	qreal dbMinValue = 0;
	qreal dbMaxValue = 0;

	//获取当前所有显示波形的最大值，最小值及名称
	for (qint32 iWaveformIndex=0; iWaveformIndex<iWaveCount; iWaveformIndex++)
	{
		GetWaveWidget(iWaveformIndex)->GetMinMaxValue(dbMinValue, dbMaxValue);
		scaleCalcualtor.SetWaveMinMaxValue(GetWaveWidget(iWaveformIndex)->GetWaveName(), dbMinValue, dbMaxValue);
	}

	//获取计算完成后的最大值和最小值，逐个刷新显示。
	for (qint32 iWaveformIndex=0; iWaveformIndex<iWaveCount; iWaveformIndex++)
	{
		scaleCalcualtor.GetWaveMinMaxValue(GetWaveWidget(iWaveformIndex)->GetWaveName(),
			dbMinValue, dbMaxValue/*, GetWaveWidget(iWaveformIndex)->GetDataResolution()*/);
		GetWaveWidget(iWaveformIndex)->SetScale(dbMinValue, dbMaxValue);
	}
}


///*!
//    重载基类函数
//    @return  QT_NAMESPACE::QSize
//*/
//QSize QWavesWidget::sizeHint() const
//{
//	const psimpl::RenderArea* renderAreaPtr = GetRenderArea(0);
//	const QLabel* labelPtr = GetWaveNameLabel(0);
//	qint32 labelHeight = labelPtr->height();
//	qint32 renderAreaHeight = renderAreaPtr->sizeHint().height();
//	qint32 maxHeight = (labelHeight > renderAreaHeight) ? labelHeight : renderAreaHeight;
//
//	qint32 iHeight = m_iCurWaveCount * maxHeight 
//		+ (m_iCurWaveCount -1) * LAYOUT_SPACING
//		+ LAYOUT_CONTENT_MARGINS * 2;
//
//	qint32 iWidth = labelPtr->sizeHint().width() 
//		+ LAYOUT_SPACING 
//		+ renderAreaPtr->sizeHint().width() 
//		+ LAYOUT_SPACING + LAYOUT_SPACING * 2;
//
//	return QSize(iWidth, iHeight);
//}


/*!
    重载基本函数
    @para[in]  bool visible  
    @para[out]  bool visible  
    @return  void
*/
void QWavesWidget::setVisible(bool visible)
{
	QWidget::setVisible(visible);
	if (visible)
	{
		SetUsefulWidgetVisable();
	}
	else
	{
		HideAllWidget();
	}
}


/*!
    隐藏所有控件
    @return  void
*/
void QWavesWidget::HideAllWidget()
{
	for (qint32 i=0; i<MAX_SHOW_WAVE_COUNT; i++)
	{
		GetWaveWidget(i)->setVisible(false);
	}
	setFixedSize(0,0);
}


/*!
    显示有用的控件
    @return  void
*/
void QWavesWidget::SetUsefulWidgetVisable()
{
	for (qint32 i=0; i<m_iCurWaveCount; i++)
	{
		GetWaveWidget(i)->setVisible(true);
	}
	//setFixedSize(sizeHint());
}