#ifndef HL7REVIEWER_H
#define HL7REVIEWER_H

#include <QtGui/QMainWindow>
#include "ui_hl7reviewer.h"

class QSQLiteParser;
class QAction;
class QLabel;
class QWavesWidget;

class HL7Reviewer : public QMainWindow
{
	Q_OBJECT

public:
	HL7Reviewer(QWidget *parent = 0, Qt::WFlags flags = 0);
	~HL7Reviewer();

	void OnInitDialog();

public slots:
	void OnOpenDatabaseDirClicked();
	void OnOpenHL7LogDirClicked();
	void OnOpenCISLogClicked();

	void OnQuitClicked();
	void OnAboutClicked();
	void OnActionGroupTriggered(QAction* action);
	void OnWarningInformation(const QString& info);

private:
	void InitCentralWidget();
	void HideAllWidget();
	void RepaintWave();

private:
	void UpdateDisplayPixelsPerCM();

private:
	QSQLiteParser*               m_sqliteParserPtr;
	QLabel*                      m_infoLabelPtr;
	QWavesWidget*                m_wavesWidgetPtr;

private:
	Ui::HL7ReviewerClass ui;
	Q_DISABLE_COPY(HL7Reviewer);
};

#endif // HL7REVIEWER_H
