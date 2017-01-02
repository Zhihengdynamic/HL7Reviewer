#ifndef QWAVESWIDGET_H
#define QWAVESWIDGET_H

#include <QWidget>

class QLabel;
struct STUWaveform;
struct STUAnnotation;
namespace psimpl
{
	class RenderArea;
}
class QWaveWidget;

/*
	一组波形控件
	每一组波形：有一个波形名称label和一个波形区域RenderArea
*/
class QWavesWidget : public QWidget
{
	Q_OBJECT

public:
	QWavesWidget(QWidget* parent);
	~QWavesWidget();

	void DrawWaveforms(const QVector<STUWaveform*>& wavefroms);
	
signals:
	void WarningInformation(const QString& info);

public slots:
	void OnWarningInformation(const QString& info);

private:
	void InitComponent();

    QWaveWidget* GetWaveWidget(qint32 iChannel) const;
	void FillWaveformData(qint32 iWaveformIndex, const STUWaveform* pWaveform);
	void ProcessIntelligentCalculateScale(qint32 iWaveCount);

	void HideAllWidget();
	void SetUsefulWidgetVisable();

public:
	//virtual QSize sizeHint() const;
	virtual void setVisible(bool visible);

private:
	qint32                       m_iCurWaveCount;//当前波形个数
    QVector<QWaveWidget*>        m_vecWaveWidget;//波形控件

private:
	Q_DISABLE_COPY(QWavesWidget);
};

#endif // QWAVESWIDGET_H
