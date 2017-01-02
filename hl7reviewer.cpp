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
    构造函数
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

	//更新配置值,需要放在InitDialog的前面
	UpdateDisplayPixelsPerCM();

	OnInitDialog();
}

/*!
    析构函数
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
    初始化界面显示。连接信号等。
    @return  void
*/
void HL7Reviewer::OnInitDialog()
{
	//窗口标题
	setWindowTitle(HL7_VIEWER);
	setWindowIcon(QIcon(":/hl7viewer/app.png"));

	//工具栏
	ui.mainToolBar->addAction(ui.actionOpenDatabaseDir);
	ui.mainToolBar->addAction(ui.actionOpenHL7LogDir);
	ui.mainToolBar->addAction(ui.actionOpenCISLog);
	ui.mainToolBar->addSeparator();

	InitCentralWidget();

	OnWarningInformation("");

	//信号连接
	connect(ui.actionOpenDatabaseDir, SIGNAL(triggered()), this, SLOT(OnOpenDatabaseDirClicked()));
	connect(ui.actionOpenHL7LogDir, SIGNAL(triggered()), this, SLOT(OnOpenHL7LogDirClicked()));
	connect(ui.actionOpenCISLog, SIGNAL(triggered()), this, SLOT(OnOpenCISLogClicked()));

	connect(ui.actionQuit, SIGNAL(triggered()), this, SLOT(OnQuitClicked()));
	connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(OnAboutClicked()));
	connect(m_wavesWidgetPtr, SIGNAL(WarningInformation(const QString&)), this, SLOT(OnWarningInformation(const QString&)));

	//隐藏所有控件
	HideAllWidget();
}


void HL7Reviewer::InitCentralWidget()
{
	QVBoxLayout* verticalLayout = new QVBoxLayout(ui.centralWidget);
	verticalLayout->setSpacing(6);
	verticalLayout->setContentsMargins(11, 11, 11, 11);

	ui.centralWidget->layout()->setSizeConstraint(QLayout::SetFixedSize);

	//信息提示控件
	m_infoLabelPtr = new QLabel(ui.centralWidget);
	m_infoLabelPtr->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));
	verticalLayout->addWidget(m_infoLabelPtr);

	//波形控件
	m_wavesWidgetPtr = new QWavesWidget(ui.centralWidget);
	verticalLayout->addWidget(m_wavesWidgetPtr);
	
	//设置为滚动视图
	QScrollArea* scrollArea = new QScrollArea;
	scrollArea->setWidget(ui.centralWidget);
	setCentralWidget(scrollArea);
}


/*!
    隐藏波形控件和参数控件
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
		//更新标题栏
		QString windowsTitle = QString(tr("%1 - %2")).arg(dirName).arg(HL7_VIEWER); 
		setWindowTitle(windowsTitle);

		//保存路径
		ConfigManager::Instance()->Instance()->UpdateConfig(ID_LAST_OPEN_FILE_PATH, dirName);

		//解析文件
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

		//默认显示波形
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
    “退出”响应函数
    @return  void
*/
void HL7Reviewer::OnQuitClicked()
{
	QApplication::instance()->quit();
}


/*!
    “关于”响应函数
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
    显示（波形、平均模板、参数列表）
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
    信息提示槽函数。调用场景：
	1. 当解析出错时调用；
	2. 当波形条数超过界面最大支持数目时调用；
	3. 超过当前界面最大绘制的秒数；
	4. 也可以手动触发
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
    閲嶅埛娉㈠舰
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
    更新配置值,需要放在InitDialog的前面
*/
void HL7Reviewer::UpdateDisplayPixelsPerCM()
{
    qreal horDotsPerCM = 44; // 璇曢獙鍊?
    qreal verDotsPerCM = 44; // 璇曢獙鍊?

	horDotsPerCM = width();
	horDotsPerCM = 10.0 * horDotsPerCM / widthMM();

    verDotsPerCM = height();
    verDotsPerCM = 10.0 * verDotsPerCM / heightMM();

    //更新配置值,需要放在InitDialog的前面
    ConfigManager::Instance()->UpdateConfig(ID_HOR_PIXELS_PER_CM, horDotsPerCM);
    ConfigManager::Instance()->UpdateConfig(ID_VER_PIXELS_PER_CM, verDotsPerCM);
}
