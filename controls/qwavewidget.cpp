#include "qwavewidget.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include "controls/RenderArea.h"
#include "controls/waveformstructs.h"
#include "config/ConfigManager.h"


using namespace psimpl;
static const qint32 LAYOUT_SPACING            = 6;
static const qint32 LAYOUT_CONTENT_MARGINS    = 11;
static const qint32 LABEL_MIN_WIDTH           = 100;

QWaveWidget::QWaveWidget(QWidget *parent)
    : QWidget(parent)
    , m_labelWaveName(NULL)
    , m_lineEditHighScale(NULL)
    , m_lineEditLowScale(NULL)
    , m_renderArea(NULL)
    , m_color(NULL)
{
    InitComponent();
}

QWaveWidget::~QWaveWidget()
{

}

void QWaveWidget::InitComponent()
{
    QGridLayout * girdLayout = new QGridLayout(this);
    girdLayout->setSpacing(LAYOUT_SPACING);
    girdLayout->setContentsMargins(LAYOUT_CONTENT_MARGINS, LAYOUT_CONTENT_MARGINS, LAYOUT_CONTENT_MARGINS, LAYOUT_CONTENT_MARGINS);
    m_labelWaveName = new QLabel(this);
    m_lineEditHighScale = new QLineEdit(this);
    m_lineEditLowScale = new QLineEdit(this);
    m_renderArea = new psimpl::RenderArea(this);
    girdLayout->addWidget(m_lineEditHighScale, 0, 0, 1, 1);
    girdLayout->addWidget(m_labelWaveName, 1, 0, 1, 1);
    girdLayout->addWidget(m_lineEditLowScale, 2, 0, 1, 1);
    girdLayout->addWidget(m_renderArea, 0, 1, 3, 1);

    connect(m_renderArea, SIGNAL(WarningInformation(const QString &)), this, SLOT(OnWarningInformation(const QString &)));
    connect(m_lineEditHighScale, SIGNAL(editingFinished()), this, SLOT(OnHighScaleEditingFinished()));
    connect(m_lineEditLowScale, SIGNAL(editingFinished()), this, SLOT(OnLowScaleEditingFinished()));

    qint32 redFactor = qrand() % 255;
    qint32 greenFactor = 255;
    qint32 blueFactor = qrand() % 255;
    m_color = new QColor(redFactor, greenFactor, blueFactor);
    m_labelWaveName->setStyleSheet(QString::fromUtf8("backgrond-color: rgb(%1, %2, %3);")
        .arg(m_color->red())
        .arg(m_color->green())
        .arg(m_color->blue()));
}

void QWaveWidget::OnWarningInformation(const QString& info)
{
    emit WarningInformation(info);
}

void QWaveWidget::OnHighScaleEditingFinished()
{
    qreal lowScale = 0.0;
    qreal hightScale = 0.0;
    m_renderArea->GetScale(lowScale, hightScale);

    hightScale = m_lineEditHighScale->text().toDouble() / m_renderArea->GetDataResolution();

    if (lowScale > hightScale)
    {
        m_lineEditHighScale->setStyleSheet(QString::fromUtf8("backgrond-color: rgb(255, 0, 0);"));
    }
    else
    {
        m_lineEditHighScale->setStyleSheet(QString::fromUtf8("backgrond-color: rgb(255,255, 255);"));
        m_renderArea->SetScale(lowScale, hightScale);
    }
}

void QWaveWidget::OnLowScaleEditingFinished()
{
    qreal lowScale = 0.0;
    qreal hightScale = 0.0;
    m_renderArea->GetScale(lowScale, hightScale);

    lowScale = m_lineEditLowScale->text().toDouble() / m_renderArea->GetDataResolution();

    if (lowScale > hightScale)
    {
        m_lineEditLowScale->setStyleSheet(QString::fromUtf8("backgrond-color: rgb(255, 0, 0);"));
    }
    else
    {
        m_lineEditLowScale->setStyleSheet(QString::fromUtf8("backgrond-color: rgb(255,255, 255);"));
        m_renderArea->SetScale(lowScale, hightScale);
    }
}

void QWaveWidget::DrawWaveform(const STUWaveform* pWaveform)
{
    FillWaveformData(pWaveform);
}

void QWaveWidget::FillWaveformData(const STUWaveform* pWaveform)
{
    if (NULL != pWaveform && NULL != m_renderArea && NULL != m_labelWaveName)
    {
        m_labelWaveName->setText(pWaveform->m_type);//波形名称标签
        if (pWaveform->m_waveformSegments.size() > 0)
        {
            m_renderArea->SetWaveName(pWaveform->m_type);//波形名称
            m_renderArea->SetDataResolution(pWaveform->m_waveformSegments[0]->m_dataResolation);//分辨率
            m_renderArea->SetUnits(pWaveform->m_units);//单位

            m_renderArea->SetSampleRate(pWaveform->m_waveformSegments[0]->m_sampleRate);

            QString strAllWaveData;
            QString strPacerPosition;
            qint32 iSample = pWaveform->m_waveformSegments[0]->m_sampleRate.toInt();
            for (qint32 iSeg=0; iSeg<pWaveform->m_waveformSegments.size(); iSeg++)
            {
                strAllWaveData += pWaveform->m_waveformSegments[iSeg]->m_data;
                if (iSeg != (pWaveform->m_waveformSegments.size()-1))
                {
                    strAllWaveData += QString(", ");
                }

                AppendPacerPosition(pWaveform->m_waveformSegments[iSeg]->m_annotation, iSeg*iSample, strPacerPosition);
            }
            m_renderArea->SetWaveData(strAllWaveData);
            m_renderArea->SetPacerPosition(strPacerPosition);
        }
    }
}

/*!
    生成PACE字符串。比如在分别在偏移位置100，200的地方绘制数据，
	则字符串为：“100，200，”。生成的字符串传递给绘制控件处理。
    @para[in]  const QVector<STUAnnotation * > & vecAnnotation  
    @para[in]  qint32 iOffset  累积偏移量
    @para[out]  QString & strPacerPosition  
    @return  void
*/
void QWaveWidget::AppendPacerPosition(const QVector<STUAnnotation*>& vecAnnotation, 
										qint32 iOffset, 
										QString& strPacerPosition)
{
	STUAnnotation* pAnnotation = NULL;

	for (QVector<STUAnnotation*>::const_iterator it=vecAnnotation.begin(); it!=vecAnnotation.end(); it++)
	{
		pAnnotation = *it;

		if (pAnnotation->m_type == QString("Pacer"))
		{
			qint32 iPacerPos = pAnnotation->m_offset.toInt() + iOffset;
			strPacerPosition += QString("%1,").arg(iPacerPos);
		}
	}
}

void QWaveWidget::SetScale(qreal dbLowScale, qreal dbHighScale)
{
    m_renderArea->SetScale(dbLowScale, dbHighScale);
    
    qreal dataResolution = m_renderArea->GetDataResolution();
    m_lineEditHighScale->setText(QString("%1").arg(dbHighScale*dataResolution));
    m_lineEditLowScale->setText(QString("%1").arg(dbLowScale*dataResolution));
}

void QWaveWidget::GetMinMaxValue(qreal & dbMinValue, qreal & dbMaxValue)
{
    m_renderArea->GetMinMaxValue(dbMinValue, dbMaxValue);
}

const QString QWaveWidget::GetWaveName() const
{
    if (NULL != m_labelWaveName)
    {
        return m_labelWaveName->text();
    }
    return "";
}

qreal QWaveWidget::GetWaveDataResolution() const
{
    if (NULL != m_renderArea)
    {
        return m_renderArea->GetDataResolution();
    }
    return 1.0;
}


QSize QWaveWidget::sizeHint() const
{
    const psimpl::RenderArea* renderAreaPtr = m_renderArea;
    const QLabel* labelPtr = m_labelWaveName;

    qint32 labelHeight = labelPtr->height();
    qint32 renderAreaHeight = renderAreaPtr->sizeHint().height();
    qint32 maxHeight = (labelHeight > renderAreaHeight) ? labelHeight : renderAreaHeight;

    qint32 labelWidth = (LABEL_MIN_WIDTH > labelPtr->sizeHint().width()) ? LABEL_MIN_WIDTH : labelPtr->sizeHint().width();

    qint32 iHeight = maxHeight
        + LAYOUT_SPACING * 2;
    qint32 iWidth = labelWidth
        + LAYOUT_SPACING
        + renderAreaPtr->sizeHint().width()
        + LAYOUT_SPACING * 2;

    return QSize(iWidth, iHeight);
}

void QWaveWidget::setVisible(bool visible)
{
    QWidget::setVisible(visible);
    if (NULL != m_labelWaveName)
    {
        m_labelWaveName->setVisible(visible);
    }
    if (NULL != m_lineEditHighScale)
    {
        m_lineEditHighScale->setVisible(visible);
    }
    if (NULL != m_lineEditLowScale)
    {
        m_lineEditLowScale->setVisible(visible);
    }
    if (NULL != m_renderArea)
    {
        m_renderArea->setVisible(visible);
    }
}
