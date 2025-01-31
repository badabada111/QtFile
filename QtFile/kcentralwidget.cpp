#include "kcentralwidget.h"


KCentralWidget::KCentralWidget(QWidget* parent)
	: QWidget(parent)
	, m_pVLayout(NULL)
    , m_pDirectoryWatcher(NULL)
	, m_isSorted(false)
{
	//垂直布局对象
	m_pVLayout = new QVBoxLayout(this);
	
	m_pTableView1 = new QTableWidget(0,12,this);
    m_pTableView1->setHorizontalHeaderLabels({ u8"目录", u8"是否监听", u8"子目录", u8"新增", u8"删除", u8"重命名", u8"修改",u8"关注的后缀",u8"忽略的后缀",u8"指定文件监控",u8"忽略指定文件监控", u8"时间" });
	
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
		QMessageBox::critical(this, u8"无法连接数据库", u8"无法连接数据库: " + m_db.lastError().text());
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
		QMessageBox::critical(this, u8"无法创建表格", u8"创建表格失败: " + query.lastError().text());
	}

	// 初始化 QSqlTableModel 并连接到数据库
	m_pTableViewModel2 = new QSqlTableModel(m_pTableView2, m_db);
	m_pTableViewModel2->setTable("FileChanges"); // 设置要显示的数据库表
	m_pTableViewModel2->select(); // 加载表数据
	m_pTableViewModel2->sort(0, Qt::DescendingOrder);
	// 设置表头
	m_pTableViewModel2->setHeaderData(1, Qt::Horizontal, tr(u8"类型"));
	m_pTableViewModel2->setHeaderData(2, Qt::Horizontal, tr(u8"时间"));
	m_pTableViewModel2->setHeaderData(3, Qt::Horizontal, tr(u8"内容"));

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

	m_pSearchLabel = new QLabel(tr(u8"关键字搜索："), this);
	m_pSearchLineEdit = new QLineEdit(this);
	m_pSearchButton = new QPushButton(tr(u8"搜索"), this);
	m_pSortButton = new QPushButton(tr(u8"取消搜索"), this);

	searchLayout->addWidget(m_pSearchLabel);
	searchLayout->addWidget(m_pSearchLineEdit);
	searchLayout->addWidget(m_pSearchButton);
	searchLayout->addWidget(m_pSortButton);
	searchWidgetContainer->setLayout(searchLayout);

	QWidget* inquireWidgetContainer = new QWidget(this);
	QHBoxLayout* inquireLayout = new QHBoxLayout(inquireWidgetContainer);
	m_pDateTimeBegin = new QLabel(tr(u8"开始时间："), this);
	m_pDateTimeBeginEdit = new QDateTimeEdit(this);
	m_pDateTimeEnd = new QLabel(tr(u8"结束时间："), this);
	m_pDateTimeEndEdit = new QDateTimeEdit(this);
	m_pInquireButton = new QPushButton(tr(u8"查询"), this);
	m_pInquireRemuseButton = new QPushButton(tr(u8"恢复到查询前"), this);
	QString dateTimeFormat = "yyyy-MM-dd HH:mm:ss"; 
	m_pDateTimeBeginEdit->setDisplayFormat(dateTimeFormat);
	m_pDateTimeEndEdit->setDisplayFormat(dateTimeFormat);

	// 设置初始日期和时间
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

	m_pLogTavWidget->addTab(tableView2Container, QString::fromLocal8Bit("表格日志"));

	m_pTableView3 = new QTableView(this);


	QString createTableQueryTwo =
		"CREATE TABLE IF NOT EXISTS UserOperation ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"timestamp TEXT, "
		"operationType TEXT, "
		"directory TEXT"
		")";

	if (!query.exec(createTableQueryTwo)) {
		QMessageBox::critical(this, u8"无法创建表格", u8"创建表格失败: " + query.lastError().text());
	}

	// 初始化 QSqlTableModel 并连接到数据库
	m_pTableViewModel3 = new QSqlTableModel(m_pTableView3, m_db);
	m_pTableViewModel3->setTable("UserOperation"); // 设置要显示的数据库表
	m_pTableViewModel3->select(); // 加载表数据

	m_pTableViewModel3->setHeaderData(1, Qt::Horizontal, tr(u8"时间"));
	m_pTableViewModel3->setHeaderData(2, Qt::Horizontal, tr(u8"类型"));
	m_pTableViewModel3->setHeaderData(3, Qt::Horizontal, tr(u8"内容"));
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

	m_pSearchLabelTwo = new QLabel(tr(u8"关键字搜索："), this);
	m_pSearchLineEditTwo = new QLineEdit(this);
	m_pSearchButtonTwo = new QPushButton(tr(u8"搜索"), this);
	m_pSortButtonTwo = new QPushButton(tr(u8"取消搜索"), this);

	searchLayoutTwo->addWidget(m_pSearchLabelTwo);
	searchLayoutTwo->addWidget(m_pSearchLineEditTwo);
	searchLayoutTwo->addWidget(m_pSearchButtonTwo);
	searchLayoutTwo->addWidget(m_pSortButtonTwo);
	searchWidgetContainerTwo->setLayout(searchLayoutTwo);

	QWidget* inquireWidgetContainerTwo = new QWidget(this);
	QHBoxLayout* inquireLayoutTwo = new QHBoxLayout(inquireWidgetContainerTwo);
	m_pDateTimeBeginTwo = new QLabel(tr(u8"开始时间："), this);
	m_pDateTimeBeginEditTwo = new QDateTimeEdit(this);
	m_pDateTimeEndTwo = new QLabel(tr(u8"结束时间："), this);
	m_pDateTimeEndEditTwo = new QDateTimeEdit(this);
	m_pInquireButtonTwo = new QPushButton(tr(u8"查询"), this);
	m_pInquireRemuseButtonTwo = new QPushButton(tr(u8"恢复到查询前"), this);
	QString dateTimeFormatTwo = "yyyy-MM-dd HH:mm:ss";
	m_pDateTimeBeginEditTwo->setDisplayFormat(dateTimeFormatTwo);
	m_pDateTimeEndEditTwo->setDisplayFormat(dateTimeFormatTwo);

	// 设置初始日期和时间
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

	m_pLogTavWidget->addTab(tableView2ContainerTwo, QString::fromLocal8Bit("用户操作日志"));
	
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

	// 获取目录路径和时间戳
	QString directory = m_pTableViewModel2->data(m_pTableViewModel2->index(row, column)).toString();
	QString timestamp = m_pTableViewModel2->data(m_pTableViewModel2->index(row, 2)).toString(); // 获取时间戳

	if (directory.isEmpty() || timestamp.isEmpty()) return;

	QString message = QString(u8"确定要回溯文件 %1 到 %2 吗？").arg(directory).arg(timestamp);

	QMessageBox::StandardButton reply = QMessageBox::question(this, u8"回溯确认", message, QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes) {
		performRollback(directory, timestamp);
	}
}

void KCentralWidget::performRollback(const QString& filePath, const QString& timestamp)
{
    
    QString backupDirPath = QDir::currentPath() + "/backup";
    QDir backupDir(backupDirPath);

    if (!backupDir.exists()) {
        QMessageBox::warning(this, u8"回溯失败", u8"备份目录不存在！");
        return;
    }

    // 获取文件名（无路径）
    QString fileName = QFileInfo(filePath).fileName();
    
    // 查找所有相关备份文件（匹配文件名前缀）
    QStringList filters;
    filters << QString("%1_*.bak").arg(fileName);
    QStringList backupFiles = backupDir.entryList(filters, QDir::Files, QDir::Time);

    if (backupFiles.isEmpty()) {
        QMessageBox::warning(this, u8"回溯失败", u8"没有找到该文件的备份记录！");
        return;
    }

    // 目标时间转换（确保时间格式匹配 backup 时的格式）
    QDateTime targetTime = QDateTime::fromString(timestamp, "yyyy-MM-dd HH:mm:ss");

    if (!targetTime.isValid()) {
        QMessageBox::warning(this, u8"回溯失败", u8"时间戳格式错误！");
        return;
    }

    // 遍历所有备份，找到最接近但不超过 `targetTime` 的备份
    QString closestBackup;
    QDateTime closestTime;

    for (const QString& file : backupFiles) {
        // 提取时间戳部分（格式：yyyyMMdd_HHmmss）
        QString timeStr = file.mid(fileName.length() + 1, 15);
        QDateTime fileTime = QDateTime::fromString(timeStr, "yyyyMMdd_HHmmss");

        if (!fileTime.isValid()) continue;

        // 找到最接近 `targetTime` 且不晚于它的备份
        if (fileTime <= targetTime && (closestTime.isNull() || fileTime > closestTime)) {
            closestBackup = file;
            closestTime = fileTime;
        }
    }

    if (closestBackup.isEmpty()) {
        QMessageBox::warning(this, u8"回溯失败", u8"没有找到符合时间的备份文件！");
        return;
    }

    // 备份文件完整路径
    QString backupFilePath = backupDir.absoluteFilePath(closestBackup);

    if (!QFile::exists(backupFilePath)) {
        QMessageBox::warning(this, u8"回溯失败", u8"选定的备份文件不存在！");
        return;
    }

    // **回溯操作**
    QFile originalFile(filePath);

    // 先删除原文件，确保可以覆盖
    if (originalFile.exists() && !originalFile.remove()) {
        QMessageBox::warning(this, u8"回溯失败", u8"无法删除原文件：" + filePath);
        return;
    }

    // 复制备份文件回原路径
    if (QFile::copy(backupFilePath, filePath)) {
        QMessageBox::information(this, u8"回溯成功", QString(u8"文件 %1 已恢复到 %2").arg(filePath).arg(closestTime.toString("yyyy-MM-dd HH:mm:ss")));
    } else {
        QMessageBox::warning(this, u8"回溯失败", u8"恢复文件时发生错误：" + backupFilePath);
    }
}

void KCentralWidget::backupFile(const QString& filePath, const QString& timestamp)
{
	QFile file(filePath);
	if (!file.exists()) {
		qDebug() << u8"文件不存在：" << filePath;
		return;
	}

	// **获取绝对路径，防止路径错误**
	QString absoluteFilePath = QFileInfo(filePath).absoluteFilePath();
	qDebug() << u8"尝试备份文件：" << absoluteFilePath;

	// **尝试打开文件，确保可以读**
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << u8"无法打开文件进行备份：" << absoluteFilePath << "-" << file.errorString();
		return;
	}
	file.close();

	// **创建备份目录**
	QString backupDirPath = QDir::currentPath() + "/backup";  // 绝对路径
	QDir backupDir(backupDirPath);

	if (!backupDir.exists()) {
		if (!backupDir.mkpath(backupDirPath)) {
			qDebug() << u8"创建备份目录失败：" << backupDir.absolutePath();
			return;
		}
	}

	// **验证目录是否可写**
	QFileInfo dirInfo(backupDirPath);
	if (!dirInfo.exists() || !dirInfo.isWritable()) {
		qDebug() << u8"备份目录不存在或不可写：" << backupDir.absolutePath();
		return;
	}

	// **格式化时间戳，避免非法字符**
	QString safeTimestamp = QDateTime::fromString(timestamp, "yyyy-MM-dd HH:mm:ss")
		.toString("yyyyMMdd_HHmmss");

	// **替换特殊字符，避免空格和非法字符**
	QString safeFileName = QFileInfo(filePath).fileName();
	safeFileName.replace(" ", "_");  // 替换空格
	safeFileName.replace(":", "_");  // 替换冒号等字符

	// **生成备份文件路径**
	QString backupFileName = QString("%1/%2_%3.bak")
		.arg(backupDirPath)
		.arg(safeFileName)
		.arg(safeTimestamp);

	qDebug() << u8"源文件：" << absoluteFilePath;
	qDebug() << u8"目标备份文件：" << backupFileName;

	// **删除已存在的备份**
	if (QFile::exists(backupFileName) && !QFile::remove(backupFileName)) {
		qDebug() << u8"无法删除旧的备份文件：" << backupFileName;
		return;
	}

	// **尝试复制文件**
	if (!file.copy(backupFileName)) {
		qDebug() << u8"备份失败：" << file.errorString();
	}
	else {
		qDebug() << u8"备份成功：" << backupFileName;
	}
}

void KCentralWidget::insertData(const QString& action, const QString& time, const QString& filepath)
{
	// 将数据加入缓存区
	m_dataBuffer.enqueue({ action, time, filepath });

	// 如果缓存区达到一定数量，立即触发处理
	if (m_dataBuffer.size() >= BUFFER_PROCESS_SIZE) {
		processBuffer();
	}
}

void KCentralWidget::processBuffer()
{
	// 判断缓存区是否为空
	if (m_dataBuffer.isEmpty()) return;

	QSqlDatabase db = QSqlDatabase::database();
	if (!db.isOpen())
	{
		qDebug() << u8"数据库连接失败";
		logDatabaseError(u8"数据库未连接");
		return;
	}

	QSqlQuery query;
	if (!db.transaction())
	{
		qDebug() << u8"数据库事务启动失败";
		logDatabaseError(u8"数据库事务启动失败");
		return;
	}

	bool allSucceeded = true;
	while (!m_dataBuffer.isEmpty())
	{
		QList<QString> data = m_dataBuffer.dequeue();  // 从缓存区取出数据

		query.prepare("INSERT INTO FileChanges (messageType, timestamp, directory) "
			"VALUES (?, ?, ?)");
		query.addBindValue(data[0]);  // action
		query.addBindValue(data[1]);  // time
		query.addBindValue(data[2]);  // filepath

		if (!query.exec())
		{
			qDebug() << u8"数据库插入数据失败" << query.lastError().text();
			logDatabaseError(query.lastError().text());  // 记录插入失败的日志
			allSucceeded = false;
			break;  // 插入失败，跳出循环
		}
	}

	if (allSucceeded)
	{
		if (!db.commit())
		{
			qDebug() << u8"数据库事务提交失败";
			logDatabaseError(u8"数据库事务提交失败");
		}
	}
	else
	{
		db.rollback();  // 如果有失败的插入，回滚事务
		qDebug() << u8"事务回滚";
		logDatabaseError(u8"事务回滚");
	}

	m_pTableViewModel2->select();  // 更新界面
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
		qDebug() << u8"数据库插入数据失败" << query.lastError().text();
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
		if (action == QString::fromLocal8Bit("新增文件"))
		{
			color = Qt::green;
		}
		else if (action == QString::fromLocal8Bit("删除文件"))
		{
			color = Qt::red;
		}
		else if (action == QString::fromLocal8Bit("修改文件"))
		{
			color = Qt::magenta;
		}
		else if (action == QString::fromLocal8Bit("重命名（旧）"))
		{
			color = Qt::blue;
		}
		else if (action == QString::fromLocal8Bit("重命名（新）"))
		{
			color = Qt::cyan;
		}
		else if (action == QString::fromLocal8Bit("未知"))
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
