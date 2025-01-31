#include "kcentralwidget.h"


KCentralWidget::KCentralWidget(QWidget* parent)
	: QWidget(parent)
	, m_pVLayout(NULL)
    , m_pDirectoryWatcher(NULL)
	, m_isSorted(false)
{
	//��ֱ���ֶ���
	m_pVLayout = new QVBoxLayout(this);
	
	m_pTableView1 = new QTableWidget(0,12,this);
    m_pTableView1->setHorizontalHeaderLabels({ u8"Ŀ¼", u8"�Ƿ����", u8"��Ŀ¼", u8"����", u8"ɾ��", u8"������", u8"�޸�",u8"��ע�ĺ�׺",u8"���Եĺ�׺",u8"ָ���ļ����",u8"����ָ���ļ����", u8"ʱ��" });
	
	m_pTableView1->setColumnWidth(0, 250); 
	m_pTableView1->setColumnWidth(1, 80); 
	m_pTableView1->setColumnWidth(2, 70);
	m_pTableView1->setColumnWidth(3, 50);
	m_pTableView1->setColumnWidth(4, 50);
	m_pTableView1->setColumnWidth(5, 70);
	m_pTableView1->setColumnWidth(6, 50);
	m_pTableView1->setColumnWidth(7, 150);
	m_pTableView1->setColumnWidth(8, 150);
	m_pTableView1->setColumnWidth(9, 200);
	m_pTableView1->setColumnWidth(10, 200);
	m_pTableView1->setColumnWidth(11, 170);

	m_pTableView1->setEditTriggers(QAbstractItemView::NoEditTriggers);

	m_pTableView1->setSelectionBehavior(QAbstractItemView::SelectRows);

	m_pLogTavWidget = new QTabWidget(this);
	m_pTableView2 = new QTableView(this);

	m_db = QSqlDatabase::addDatabase("QSQLITE");
	m_db.setDatabaseName("file_changes.db");

	if (!m_db.open()) {
		QMessageBox::critical(this, u8"�޷��������ݿ�", u8"�޷��������ݿ�: " + m_db.lastError().text());
	}

	QSqlQuery query;
	QString createTableQuery =
		"CREATE TABLE IF NOT EXISTS FileChanges ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"messageType TEXT, "
		"timestamp TEXT, "
		"directory TEXT"
		")";

	if (!query.exec(createTableQuery)) {
		QMessageBox::critical(this, u8"�޷��������", u8"�������ʧ��: " + query.lastError().text());
	}

	// ��ʼ�� QSqlTableModel �����ӵ����ݿ�
	m_pTableViewModel2 = new QSqlTableModel(m_pTableView2, m_db);
	m_pTableViewModel2->setTable("FileChanges"); // ����Ҫ��ʾ�����ݿ��
	m_pTableViewModel2->select(); // ���ر�����
	m_pTableViewModel2->sort(0, Qt::DescendingOrder);
	// ���ñ�ͷ
	m_pTableViewModel2->setHeaderData(1, Qt::Horizontal, tr(u8"����"));
	m_pTableViewModel2->setHeaderData(2, Qt::Horizontal, tr(u8"ʱ��"));
	m_pTableViewModel2->setHeaderData(3, Qt::Horizontal, tr(u8"����"));

	m_pTableView2->setModel(m_pTableViewModel2);
	//m_pTableView2->hideColumn(0);

	m_pTableView2->setColumnWidth(1, 100);
	m_pTableView2->setColumnWidth(3, 700);
	m_pTableView2->setColumnWidth(2, 170);

	CustomItemDelegate* delegate = new CustomItemDelegate(this);
	m_pTableView2->setItemDelegateForColumn(1, delegate);
	
	m_pTableView2->setSortingEnabled(false);
	m_pTableView2->horizontalHeader()->setSortIndicatorShown(true);
	m_pTableView2->horizontalHeader()->setSectionsClickable(true);
	m_pTableView2->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
	connect(m_pTableView2, &QTableView::doubleClicked, this, &KCentralWidget::onRowDoubleClicked);

	(void)connect(m_pTableView2->horizontalHeader(), &QHeaderView::sectionClicked, this, &KCentralWidget::onHeaderClicked);

	QWidget* searchWidgetContainer = new QWidget(this);
	QHBoxLayout* searchLayout = new QHBoxLayout(searchWidgetContainer);

	m_pSearchLabel = new QLabel(tr(u8"�ؼ���������"), this);
	m_pSearchLineEdit = new QLineEdit(this);
	m_pSearchButton = new QPushButton(tr(u8"����"), this);
	m_pSortButton = new QPushButton(tr(u8"ȡ������"), this);

	searchLayout->addWidget(m_pSearchLabel);
	searchLayout->addWidget(m_pSearchLineEdit);
	searchLayout->addWidget(m_pSearchButton);
	searchLayout->addWidget(m_pSortButton);
	searchWidgetContainer->setLayout(searchLayout);

	QWidget* inquireWidgetContainer = new QWidget(this);
	QHBoxLayout* inquireLayout = new QHBoxLayout(inquireWidgetContainer);
	m_pDateTimeBegin = new QLabel(tr(u8"��ʼʱ�䣺"), this);
	m_pDateTimeBeginEdit = new QDateTimeEdit(this);
	m_pDateTimeEnd = new QLabel(tr(u8"����ʱ�䣺"), this);
	m_pDateTimeEndEdit = new QDateTimeEdit(this);
	m_pInquireButton = new QPushButton(tr(u8"��ѯ"), this);
	m_pInquireRemuseButton = new QPushButton(tr(u8"�ָ�����ѯǰ"), this);
	QString dateTimeFormat = "yyyy-MM-dd HH:mm:ss"; 
	m_pDateTimeBeginEdit->setDisplayFormat(dateTimeFormat);
	m_pDateTimeEndEdit->setDisplayFormat(dateTimeFormat);

	// ���ó�ʼ���ں�ʱ��
	QDateTime currentDateTime = QDateTime::currentDateTime();
	m_pDateTimeBeginEdit->setDateTime(currentDateTime);
	m_pDateTimeEndEdit->setDateTime(currentDateTime);

	inquireLayout->addWidget(m_pDateTimeBegin);
	inquireLayout->addWidget(m_pDateTimeBeginEdit);
	inquireLayout->addStretch();
	inquireLayout->addWidget(m_pDateTimeEnd);
	inquireLayout->addWidget(m_pDateTimeEndEdit);
	inquireLayout->addStretch();
	inquireLayout->addWidget(m_pInquireButton);
	inquireLayout->addWidget(m_pInquireRemuseButton);
	inquireWidgetContainer->setLayout(inquireLayout);

	QWidget* tableView2Container = new QWidget(this);
	QVBoxLayout* tableView2Layout = new QVBoxLayout(tableView2Container);
	tableView2Layout->addWidget(m_pTableView2);
	tableView2Layout->addWidget(searchWidgetContainer);
	tableView2Layout->addWidget(inquireWidgetContainer);
	tableView2Container->setLayout(tableView2Layout);

	m_pLogTavWidget->addTab(tableView2Container, QString::fromLocal8Bit("�����־"));

	m_pTableView3 = new QTableView(this);


	QString createTableQueryTwo =
		"CREATE TABLE IF NOT EXISTS UserOperation ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"timestamp TEXT, "
		"operationType TEXT, "
		"directory TEXT"
		")";

	if (!query.exec(createTableQueryTwo)) {
		QMessageBox::critical(this, u8"�޷��������", u8"�������ʧ��: " + query.lastError().text());
	}

	// ��ʼ�� QSqlTableModel �����ӵ����ݿ�
	m_pTableViewModel3 = new QSqlTableModel(m_pTableView3, m_db);
	m_pTableViewModel3->setTable("UserOperation"); // ����Ҫ��ʾ�����ݿ��
	m_pTableViewModel3->select(); // ���ر�����

	m_pTableViewModel3->setHeaderData(1, Qt::Horizontal, tr(u8"ʱ��"));
	m_pTableViewModel3->setHeaderData(2, Qt::Horizontal, tr(u8"����"));
	m_pTableViewModel3->setHeaderData(3, Qt::Horizontal, tr(u8"����"));
	m_pTableView3->setModel(m_pTableViewModel3);
	m_pTableView3->hideColumn(0);

	m_pTableView3->setColumnWidth(1, 170);
	m_pTableView3->setColumnWidth(2, 150);
	m_pTableView3->setColumnWidth(3, 800);

	m_pTableView3->setSortingEnabled(false);
	m_pTableView3->horizontalHeader()->setSortIndicatorShown(true);
	m_pTableView3->horizontalHeader()->setSectionsClickable(true);
	(void)connect(m_pTableView3->horizontalHeader(), &QHeaderView::sectionClicked, this, &KCentralWidget::onHeaderTwoClicked);

	QWidget* searchWidgetContainerTwo = new QWidget(this);
	QHBoxLayout* searchLayoutTwo = new QHBoxLayout(searchWidgetContainerTwo);

	m_pSearchLabelTwo = new QLabel(tr(u8"�ؼ���������"), this);
	m_pSearchLineEditTwo = new QLineEdit(this);
	m_pSearchButtonTwo = new QPushButton(tr(u8"����"), this);
	m_pSortButtonTwo = new QPushButton(tr(u8"ȡ������"), this);

	searchLayoutTwo->addWidget(m_pSearchLabelTwo);
	searchLayoutTwo->addWidget(m_pSearchLineEditTwo);
	searchLayoutTwo->addWidget(m_pSearchButtonTwo);
	searchLayoutTwo->addWidget(m_pSortButtonTwo);
	searchWidgetContainerTwo->setLayout(searchLayoutTwo);

	QWidget* inquireWidgetContainerTwo = new QWidget(this);
	QHBoxLayout* inquireLayoutTwo = new QHBoxLayout(inquireWidgetContainerTwo);
	m_pDateTimeBeginTwo = new QLabel(tr(u8"��ʼʱ�䣺"), this);
	m_pDateTimeBeginEditTwo = new QDateTimeEdit(this);
	m_pDateTimeEndTwo = new QLabel(tr(u8"����ʱ�䣺"), this);
	m_pDateTimeEndEditTwo = new QDateTimeEdit(this);
	m_pInquireButtonTwo = new QPushButton(tr(u8"��ѯ"), this);
	m_pInquireRemuseButtonTwo = new QPushButton(tr(u8"�ָ�����ѯǰ"), this);
	QString dateTimeFormatTwo = "yyyy-MM-dd HH:mm:ss";
	m_pDateTimeBeginEditTwo->setDisplayFormat(dateTimeFormatTwo);
	m_pDateTimeEndEditTwo->setDisplayFormat(dateTimeFormatTwo);

	// ���ó�ʼ���ں�ʱ��
	QDateTime currentDateTimeTwo = QDateTime::currentDateTime();
	m_pDateTimeBeginEditTwo->setDateTime(currentDateTimeTwo);
	m_pDateTimeEndEditTwo->setDateTime(currentDateTimeTwo);

	inquireLayoutTwo->addWidget(m_pDateTimeBeginTwo);
	inquireLayoutTwo->addWidget(m_pDateTimeBeginEditTwo);
	inquireLayoutTwo->addStretch();
	inquireLayoutTwo->addWidget(m_pDateTimeEndTwo);
	inquireLayoutTwo->addWidget(m_pDateTimeEndEditTwo);
	inquireLayoutTwo->addStretch();
	inquireLayoutTwo->addWidget(m_pInquireButtonTwo);
	inquireLayoutTwo->addWidget(m_pInquireRemuseButtonTwo);
	inquireWidgetContainer->setLayout(inquireLayoutTwo);

	QWidget* tableView2ContainerTwo = new QWidget(this);
	QVBoxLayout* tableView2LayoutTwo = new QVBoxLayout(tableView2ContainerTwo);
	tableView2LayoutTwo->addWidget(m_pTableView3);
	tableView2LayoutTwo->addWidget(searchWidgetContainerTwo);
	tableView2LayoutTwo->addWidget(inquireWidgetContainerTwo);
	tableView2Container->setLayout(tableView2LayoutTwo);

	m_pLogTavWidget->addTab(tableView2ContainerTwo, QString::fromLocal8Bit("�û�������־"));
	
	m_pVLayout->addWidget(m_pTableView1);
	m_pVLayout->addWidget(m_pLogTavWidget);

	setLayout(m_pVLayout);
	connect(&m_bufferTimer, &QTimer::timeout, this, &KCentralWidget::processBuffer);
	m_bufferTimer.start(200);
}

void KCentralWidget::onRowDoubleClicked(const QModelIndex& index)
{
	if (!index.isValid()) return;

	int column = index.column();
	int row = index.row();

	if (column != 3) return;

	// ��ȡĿ¼·����ʱ���
	QString directory = m_pTableViewModel2->data(m_pTableViewModel2->index(row, column)).toString();
	QString timestamp = m_pTableViewModel2->data(m_pTableViewModel2->index(row, 2)).toString(); // ��ȡʱ���

	if (directory.isEmpty() || timestamp.isEmpty()) return;

	QString message = QString(u8"ȷ��Ҫ�����ļ� %1 �� %2 ��").arg(directory).arg(timestamp);

	QMessageBox::StandardButton reply = QMessageBox::question(this, u8"����ȷ��", message, QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes) {
		performRollback(directory, timestamp);
	}
}

void KCentralWidget::performRollback(const QString& filePath, const QString& timestamp)
{
    
    QString backupDirPath = QDir::currentPath() + "/backup";
    QDir backupDir(backupDirPath);

    if (!backupDir.exists()) {
        QMessageBox::warning(this, u8"����ʧ��", u8"����Ŀ¼�����ڣ�");
        return;
    }

    // ��ȡ�ļ�������·����
    QString fileName = QFileInfo(filePath).fileName();
    
    // ����������ر����ļ���ƥ���ļ���ǰ׺��
    QStringList filters;
    filters << QString("%1_*.bak").arg(fileName);
    QStringList backupFiles = backupDir.entryList(filters, QDir::Files, QDir::Time);

    if (backupFiles.isEmpty()) {
        QMessageBox::warning(this, u8"����ʧ��", u8"û���ҵ����ļ��ı��ݼ�¼��");
        return;
    }

    // Ŀ��ʱ��ת����ȷ��ʱ���ʽƥ�� backup ʱ�ĸ�ʽ��
    QDateTime targetTime = QDateTime::fromString(timestamp, "yyyy-MM-dd HH:mm:ss");

    if (!targetTime.isValid()) {
        QMessageBox::warning(this, u8"����ʧ��", u8"ʱ�����ʽ����");
        return;
    }

    // �������б��ݣ��ҵ���ӽ��������� `targetTime` �ı���
    QString closestBackup;
    QDateTime closestTime;

    for (const QString& file : backupFiles) {
        // ��ȡʱ������֣���ʽ��yyyyMMdd_HHmmss��
        QString timeStr = file.mid(fileName.length() + 1, 15);
        QDateTime fileTime = QDateTime::fromString(timeStr, "yyyyMMdd_HHmmss");

        if (!fileTime.isValid()) continue;

        // �ҵ���ӽ� `targetTime` �Ҳ��������ı���
        if (fileTime <= targetTime && (closestTime.isNull() || fileTime > closestTime)) {
            closestBackup = file;
            closestTime = fileTime;
        }
    }

    if (closestBackup.isEmpty()) {
        QMessageBox::warning(this, u8"����ʧ��", u8"û���ҵ�����ʱ��ı����ļ���");
        return;
    }

    // �����ļ�����·��
    QString backupFilePath = backupDir.absoluteFilePath(closestBackup);

    if (!QFile::exists(backupFilePath)) {
        QMessageBox::warning(this, u8"����ʧ��", u8"ѡ���ı����ļ������ڣ�");
        return;
    }

    // **���ݲ���**
    QFile originalFile(filePath);

    // ��ɾ��ԭ�ļ���ȷ�����Ը���
    if (originalFile.exists() && !originalFile.remove()) {
        QMessageBox::warning(this, u8"����ʧ��", u8"�޷�ɾ��ԭ�ļ���" + filePath);
        return;
    }

    // ���Ʊ����ļ���ԭ·��
    if (QFile::copy(backupFilePath, filePath)) {
        QMessageBox::information(this, u8"���ݳɹ�", QString(u8"�ļ� %1 �ѻָ��� %2").arg(filePath).arg(closestTime.toString("yyyy-MM-dd HH:mm:ss")));
    } else {
        QMessageBox::warning(this, u8"����ʧ��", u8"�ָ��ļ�ʱ��������" + backupFilePath);
    }
}

void KCentralWidget::backupFile(const QString& filePath, const QString& timestamp)
{
	QFile file(filePath);
	if (!file.exists()) {
		qDebug() << u8"�ļ������ڣ�" << filePath;
		return;
	}

	// **��ȡ����·������ֹ·������**
	QString absoluteFilePath = QFileInfo(filePath).absoluteFilePath();
	qDebug() << u8"���Ա����ļ���" << absoluteFilePath;

	// **���Դ��ļ���ȷ�����Զ�**
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << u8"�޷����ļ����б��ݣ�" << absoluteFilePath << "-" << file.errorString();
		return;
	}
	file.close();

	// **��������Ŀ¼**
	QString backupDirPath = QDir::currentPath() + "/backup";  // ����·��
	QDir backupDir(backupDirPath);

	if (!backupDir.exists()) {
		if (!backupDir.mkpath(backupDirPath)) {
			qDebug() << u8"��������Ŀ¼ʧ�ܣ�" << backupDir.absolutePath();
			return;
		}
	}

	// **��֤Ŀ¼�Ƿ��д**
	QFileInfo dirInfo(backupDirPath);
	if (!dirInfo.exists() || !dirInfo.isWritable()) {
		qDebug() << u8"����Ŀ¼�����ڻ򲻿�д��" << backupDir.absolutePath();
		return;
	}

	// **��ʽ��ʱ���������Ƿ��ַ�**
	QString safeTimestamp = QDateTime::fromString(timestamp, "yyyy-MM-dd HH:mm:ss")
		.toString("yyyyMMdd_HHmmss");

	// **�滻�����ַ�������ո�ͷǷ��ַ�**
	QString safeFileName = QFileInfo(filePath).fileName();
	safeFileName.replace(" ", "_");  // �滻�ո�
	safeFileName.replace(":", "_");  // �滻ð�ŵ��ַ�

	// **���ɱ����ļ�·��**
	QString backupFileName = QString("%1/%2_%3.bak")
		.arg(backupDirPath)
		.arg(safeFileName)
		.arg(safeTimestamp);

	qDebug() << u8"Դ�ļ���" << absoluteFilePath;
	qDebug() << u8"Ŀ�걸���ļ���" << backupFileName;

	// **ɾ���Ѵ��ڵı���**
	if (QFile::exists(backupFileName) && !QFile::remove(backupFileName)) {
		qDebug() << u8"�޷�ɾ���ɵı����ļ���" << backupFileName;
		return;
	}

	// **���Ը����ļ�**
	if (!file.copy(backupFileName)) {
		qDebug() << u8"����ʧ�ܣ�" << file.errorString();
	}
	else {
		qDebug() << u8"���ݳɹ���" << backupFileName;
	}
}

void KCentralWidget::insertData(const QString& action, const QString& time, const QString& filepath)
{
	// �����ݼ��뻺����
	m_dataBuffer.enqueue({ action, time, filepath });

	// ����������ﵽһ��������������������
	if (m_dataBuffer.size() >= BUFFER_PROCESS_SIZE) {
		processBuffer();
	}
}

void KCentralWidget::processBuffer()
{
	// �жϻ������Ƿ�Ϊ��
	if (m_dataBuffer.isEmpty()) return;

	QSqlDatabase db = QSqlDatabase::database();
	if (!db.isOpen())
	{
		qDebug() << u8"���ݿ�����ʧ��";
		logDatabaseError(u8"���ݿ�δ����");
		return;
	}

	QSqlQuery query;
	if (!db.transaction())
	{
		qDebug() << u8"���ݿ���������ʧ��";
		logDatabaseError(u8"���ݿ���������ʧ��");
		return;
	}

	bool allSucceeded = true;
	while (!m_dataBuffer.isEmpty())
	{
		QList<QString> data = m_dataBuffer.dequeue();  // �ӻ�����ȡ������

		query.prepare("INSERT INTO FileChanges (messageType, timestamp, directory) "
			"VALUES (?, ?, ?)");
		query.addBindValue(data[0]);  // action
		query.addBindValue(data[1]);  // time
		query.addBindValue(data[2]);  // filepath

		if (!query.exec())
		{
			qDebug() << u8"���ݿ��������ʧ��" << query.lastError().text();
			logDatabaseError(query.lastError().text());  // ��¼����ʧ�ܵ���־
			allSucceeded = false;
			break;  // ����ʧ�ܣ�����ѭ��
		}
	}

	if (allSucceeded)
	{
		if (!db.commit())
		{
			qDebug() << u8"���ݿ������ύʧ��";
			logDatabaseError(u8"���ݿ������ύʧ��");
		}
	}
	else
	{
		db.rollback();  // �����ʧ�ܵĲ��룬�ع�����
		qDebug() << u8"����ع�";
		logDatabaseError(u8"����ع�");
	}

	m_pTableViewModel2->select();  // ���½���
}

void KCentralWidget::logDatabaseError(const QString& errorMsg)
{
	QFile logFile("database_error.log");
	if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) 
	{
		QTextStream out(&logFile);
		out << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << ": " << errorMsg << "\n";
		logFile.close();
	}
}

void KCentralWidget::insertDataTwo(const QString& time, const QString& action, const QString& filepath)
{
	QSqlQuery query;
	query.prepare("INSERT INTO UserOperation (timestamp, operationType, directory) "
		"VALUES (?, ?, ?)");
	query.addBindValue(time);
	query.addBindValue(action);
	query.addBindValue(filepath);

	if (query.exec())
	{
		m_pTableViewModel3->select();
	}
	else
	{
		qDebug() << u8"���ݿ��������ʧ��" << query.lastError().text();
	}
}

void KCentralWidget::onHeaderClicked(int index)
{
	m_pTableView2->setSortingEnabled(true);
	m_pTableViewModel2->setSort(index, m_pTableView2->horizontalHeader()->sortIndicatorOrder());
	m_pTableViewModel2->select();

	m_isSorted = true;
}

void KCentralWidget::onHeaderTwoClicked(int index)
{
	m_pTableView3->setSortingEnabled(true);
	m_pTableViewModel3->setSort(index, m_pTableView3->horizontalHeader()->sortIndicatorOrder());
	m_pTableViewModel3->select();

	m_isSorted = true;
}

void CustomItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (index.column() == 1)
	{
		QString action = index.data().toString();
		QColor color;
		if (action == QString::fromLocal8Bit("�����ļ�"))
		{
			color = Qt::green;
		}
		else if (action == QString::fromLocal8Bit("ɾ���ļ�"))
		{
			color = Qt::red;
		}
		else if (action == QString::fromLocal8Bit("�޸��ļ�"))
		{
			color = Qt::magenta;
		}
		else if (action == QString::fromLocal8Bit("���������ɣ�"))
		{
			color = Qt::blue;
		}
		else if (action == QString::fromLocal8Bit("���������£�"))
		{
			color = Qt::cyan;
		}
		else if (action == QString::fromLocal8Bit("δ֪"))
		{
			color = Qt::black;
		}
		painter->save();
		painter->setPen(color);
		painter->drawText(option.rect, Qt::AlignLeft | Qt::AlignVCenter, action);
		painter->restore();
	}
}

KCentralWidget::~KCentralWidget()
{
	m_bufferTimer.stop();
}
