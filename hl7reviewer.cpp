#include "hl7reviewer.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QActionGroup>
#include <QMessageBox>
#include <QFileDialog>
#include <QScrollArea>
#include <QFileInfo>
#include "config/ConfigManager.h"
#include "controls/qwaveswidget.h"
#include "mode/qsqliteparser.h"


using namespace psimpl;

static const QString HL7_VIEWER               = QString("HL7 Viewer");

/*!
    ���캯��
    @para[in]  QWidget * parent  
    @para[in]  Qt::WFlags flags  
    @return  
*/

HL7Reviewer::HL7Reviewer(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
	, m_sqliteParserPtr(NULL)
	, m_infoLabelPtr(NULL)
	, m_wavesWidgetPtr(NULL)
{
	ConfigManager::Instance();

	ui.setupUi(this);

	//��������ֵ,��Ҫ����InitDialog��ǰ��
	UpdateDisplayPixelsPerCM();

	OnInitDialog();
}

/*!
    ��������
    @return  
*/
HL7Reviewer::~HL7Reviewer()
{
	if (NULL != m_sqliteParserPtr)
	{
		delete m_sqliteParserPtr;
		m_sqliteParserPtr = NULL;
	}
}

/*!
    ��ʼ��������ʾ�������źŵȡ�
    @return  void
*/
void HL7Reviewer::OnInitDialog()
{
	//���ڱ���
	setWindowTitle(HL7_VIEWER);
	setWindowIcon(QIcon(":/hl7viewer/app.png"));

	//������
	ui.mainToolBar->addAction(ui.actionOpenDatabaseDir);
	ui.mainToolBar->addAction(ui.actionOpenHL7LogDir);
	ui.mainToolBar->addAction(ui.actionOpenCISLog);
	ui.mainToolBar->addSeparator();

	InitCentralWidget();

	OnWarningInformation("");

	//�ź�����
	connect(ui.actionOpenDatabaseDir, SIGNAL(triggered()), this, SLOT(OnOpenDatabaseDirClicked()));
	connect(ui.actionOpenHL7LogDir, SIGNAL(triggered()), this, SLOT(OnOpenHL7LogDirClicked()));
	connect(ui.actionOpenCISLog, SIGNAL(triggered()), this, SLOT(OnOpenCISLogClicked()));

	connect(ui.actionQuit, SIGNAL(triggered()), this, SLOT(OnQuitClicked()));
	connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(OnAboutClicked()));
	connect(m_wavesWidgetPtr, SIGNAL(WarningInformation(const QString&)), this, SLOT(OnWarningInformation(const QString&)));

	//�������пؼ�
	HideAllWidget();
}


void HL7Reviewer::InitCentralWidget()
{
	QVBoxLayout* verticalLayout = new QVBoxLayout(ui.centralWidget);
	verticalLayout->setSpacing(6);
	verticalLayout->setContentsMargins(11, 11, 11, 11);

	ui.centralWidget->layout()->setSizeConstraint(QLayout::SetFixedSize);

	//��Ϣ��ʾ�ؼ�
	m_infoLabelPtr = new QLabel(ui.centralWidget);
	m_infoLabelPtr->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));
	verticalLayout->addWidget(m_infoLabelPtr);

	//���οؼ�
	m_wavesWidgetPtr = new QWavesWidget(ui.centralWidget);
	verticalLayout->addWidget(m_wavesWidgetPtr);
	
	//����Ϊ������ͼ
	QScrollArea* scrollArea = new QScrollArea;
	scrollArea->setWidget(ui.centralWidget);
	setCentralWidget(scrollArea);
}


/*!
    ���ز��οؼ��Ͳ����ؼ�
    @return  void
*/
void HL7Reviewer::HideAllWidget()
{
	m_wavesWidgetPtr->setVisible(false);
}


void HL7Reviewer::OnOpenDatabaseDirClicked()
{
	QVariant vVal;
	ConfigManager::Instance()->GetConfig(ID_LAST_OPEN_FILE_PATH, vVal);
	QString dirName = QFileDialog::getExistingDirectory(this,
		tr("Open directory"), vVal.toString(),
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!dirName.isEmpty())
	{
		//���±�����
		QString windowsTitle = QString(tr("%1 - %2")).arg(dirName).arg(HL7_VIEWER); 
		setWindowTitle(windowsTitle);

		//����·��
		ConfigManager::Instance()->Instance()->UpdateConfig(ID_LAST_OPEN_FILE_PATH, dirName);

		//�����ļ�
		if (NULL == m_sqliteParserPtr)
		{
			m_sqliteParserPtr = new QSQLiteParser(this);
		}
		QString errorMsg;
		bool bSuccess = m_sqliteParserPtr->ParsePatientList(dirName, errorMsg);
		if (!bSuccess)
		{
			HideAllWidget();
			OnWarningInformation(QString(tr("Parse file errorMsg: %1").arg(errorMsg)));
			return;
		}

		//Ĭ����ʾ����
		//ui.action_Draw_Waveforms->setChecked(true);
		//RepaintWave();
	}
	else
	{
		return;
	}
}

void HL7Reviewer::OnOpenHL7LogDirClicked()
{

}

void HL7Reviewer::OnOpenCISLogClicked()
{

}

/*!
    ���˳�����Ӧ����
    @return  void
*/
void HL7Reviewer::OnQuitClicked()
{
	QApplication::instance()->quit();
}


/*!
    �����ڡ���Ӧ����
    @return  void
*/
void HL7Reviewer::OnAboutClicked()
{
	QMessageBox::about(this, tr("About HL7Viewer"),
		tr("<h2>HL7Viewer 0.1.0</h2>"
		"<p>Copyright &copy; 2016-2017 Mindray."
		"<p>HL7Viewer is a small application that review hl7 message sent by Mindray eGateay Application."
		"<p><h3>usage:</h3> "
		"<ul>"
		"<Li>'File | Open ', open directory</li>"
		));
}



/*!
    ��ʾ�����Ρ�ƽ��ģ�塢�����б�
    @return  void
*/
void HL7Reviewer::OnActionGroupTriggered(QAction* action)
{
	if (NULL != m_sqliteParserPtr)
	{
		OnWarningInformation("");
	}
	else
	{
		OnWarningInformation(tr("Please Open Directory ..."));
	}

	if (true)
	{
	}
	else
	{
		Q_ASSERT(false);
	}
}

/*!
    ��Ϣ��ʾ�ۺ��������ó�����
	1. ����������ʱ���ã�
	2. ���������������������֧����Ŀʱ���ã�
	3. ������ǰ���������Ƶ�������
	4. Ҳ�����ֶ�����
    @para[in]  const QString & info  
    @return  void
*/
void HL7Reviewer::OnWarningInformation(const QString& info)
{
	if (info.length()>0)
	{
		m_infoLabelPtr->setText( QString(tr(" *** WARNING *** ")) + info);
	}
	else
	{
		m_infoLabelPtr->setText(info);
	}
}


/*!
    重刷波形
    @return  void
*/
void HL7Reviewer::RepaintWave()
{
	//if (ui.action_Draw_Waveforms->isChecked())
	//{
	//	OnDrawWaveformsClicked();
	//}
	//else if (ui.action_Draw_MedianBeatTemplates->isChecked())
	//{
	//	OnDrawMedianBeatTemplatesClicked();
	//}
}


/*!
    ��������ֵ,��Ҫ����InitDialog��ǰ��
*/
void HL7Reviewer::UpdateDisplayPixelsPerCM()
{
    qreal horDotsPerCM = 44; // 试验�?
    qreal verDotsPerCM = 44; // 试验�?

	horDotsPerCM = width();
	horDotsPerCM = 10.0 * horDotsPerCM / widthMM();

    verDotsPerCM = height();
    verDotsPerCM = 10.0 * verDotsPerCM / heightMM();

    //��������ֵ,��Ҫ����InitDialog��ǰ��
    ConfigManager::Instance()->UpdateConfig(ID_HOR_PIXELS_PER_CM, horDotsPerCM);
    ConfigManager::Instance()->UpdateConfig(ID_VER_PIXELS_PER_CM, verDotsPerCM);
}
