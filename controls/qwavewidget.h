#ifndef QWAVEWIDGET_H
#define QWAVEWIDGET_H

class QLabel;
class QLineEdit;
class QColor;
struct STUWaveform;
struct STUAnnotation;

namespace psimpl
{
    class RenderArea;
}

#include <QtWidgets/QWidget>

class QWaveWidget : public QWidget
{
    Q_OBJECT

public:
    QWaveWidget(QWidget *parent);
    ~QWaveWidget();

    void DrawWaveform(const STUWaveform* pWaveform);
    void SetScale(qreal dbLowScale, qreal dbHighScale);
    void GetMinMaxValue(qreal & dbMinValue, qreal & dbMaxValue);
    const QString GetWaveName() const;
    qreal GetWaveDataResolution() const;

signals:
    void WarningInformation(const QString& info);

public slots:
    void OnWarningInformation(const QString& info);
    void OnHighScaleEditingFinished();
    void OnLowScaleEditingFinished();

public:
    virtual void setVisible(bool visible);
    virtual QSize sizeHint() const;

private:
    void InitComponent();
    void FillWaveformData(const STUWaveform* pWaveform);
    void AppendPacerPosition(const QVector<STUAnnotation*>& vecAnnotation, qint32 iOffset, QString& strPacerPosition);

private:
    QLabel*              m_labelWaveName;
    QLineEdit*           m_lineEditHighScale;
    QLineEdit*           m_lineEditLowScale;
    psimpl::RenderArea*  m_renderArea;
    QColor*              m_color;

private:
    Q_DISABLE_COPY(QWaveWidget)
};

#endif // QWAVEWIDGET_H
