#include "kmainwindow.h"
#include <QFileDialog>
#include <QKeySequence>
#include <QMessageBox>
#include <QDebug>
#include <QMouseEvent>
#include <kselectedbar.h>


KMainWindow::KMainWindow(QWidget *parent)
    : QWidget(parent)
    ,m_pVLayout(Q_NULLPTR)
    ,m_pHLayout(Q_NULLPTR)
{
    ui.setupUi(this);
    initWindows();
    createAction();
    init_systemTrayIcon();//系统托盘
    initContextMenu();
    (void)connect(m_pToolBar->m_pOptionFile, &QPushButton::clicked, this, &KMainWindow::onOptionFileButtonClicked);
    (void)connect(m_pToolBar->m_pAddFile, &QPushButton::clicked, this, &KMainWindow::onAddFileButtonClicked);
    (void)connect(m_pCentralWidget->m_pSearchButton, &QPushButton::clicked, this, &KMainWindow::onSearchButtonClicked);//关键字
    (void)connect(m_pCentralWidget->m_pSortButton, &QPushButton::clicked, this, &KMainWindow::onResumeSearchClicked);//关键字
    (void)connect(m_pCentralWidget->m_pInquireButton, &QPushButton::clicked, this, &KMainWindow::onInquireButtonClicked);//时间戳
    (void)connect(m_pCentralWidget->m_pInquireRemuseButton, &QPushButton::clicked, this, &KMainWindow::onResumeSearchClicked);//恢复时间戳

    (void)connect(m_pCentralWidget->m_pSearchButtonTwo, &QPushButton::clicked, this, &KMainWindow::onSearchButtonTwoClicked);//关键字
    (void)connect(m_pCentralWidget->m_pSortButtonTwo, &QPushButton::clicked, this, &KMainWindow::onResumeSearchTwoClicked);//关键字
    (void)connect(m_pCentralWidget->m_pInquireButtonTwo, &QPushButton::clicked, this, &KMainWindow::onInquireButtonTwoClicked);//时间戳
    (void)connect(m_pCentralWidget->m_pInquireRemuseButtonTwo, &QPushButton::clicked, this, &KMainWindow::onResumeSearchTwoClicked);//恢复时间戳

    (void)connect(m_pMenuBar->clearAction, &QAction::triggered, this, &KMainWindow::clearLog);
    (void)connect(m_pMenuBar->stopAction, &QAction::triggered, this, &KMainWindow::onStopClicked);
    (void)connect(m_pMenuBar->resumeAction, &QAction::triggered, this, &KMainWindow::onResumeClicked);
    (void)connect(m_pMenuBar->stopAllAction, &QAction::triggered, this, &KMainWindow::onStopAllClicked);
    (void)connect(m_pMenuBar->resumeAllAction, &QAction::triggered, this, &KMainWindow::onResumeAllClicked);
    (void)connect(m_pMenuBar->moveAction, &QAction::triggered, this, &KMainWindow::onRemoveFileButtonClicked);
    (void)connect(m_pMenuBar->saveAction, &QAction::triggered, this, &KMainWindow::onSaveLogButtonClicked);
    (void)connect(m_pMenuBar->addAction, &QAction::triggered, this, &KMainWindow::onOptionFileButtonClicked);
    (void)connect(m_pMenuBar->editAction, &QAction::triggered, this, &KMainWindow::onEditFileButtonClicked);
    (void)connect(m_pMenuBar->quitAction, &QAction::triggered, this, &KMainWindow::quitMusicPlayer);

    (void)connect(m_pMenuBar->intoAction, &QAction::triggered, this, &KMainWindow::onImportButtonClicked);
    (void)connect(m_pMenuBar->pushAction, &QAction::triggered, this, &KMainWindow::onExportButtonClicked);

    (void)connect(m_pToolBar->m_pLineEdit, &QLineEdit::textChanged, this, &KMainWindow::onDirectoryTextChanged);
    (void)connect(m_pSubDirListWidget, &QListWidget::itemClicked, this, &KMainWindow::onSubDirItemClicked);


    m_pTimer = new QTimer(this);
    (void)connect(m_pTimer, &QTimer::timeout, this, &KMainWindow::onTimerTimeout);
    m_pTimer->start(1000); // 每 5 秒检查一次
    //loadTableViewData();
    setAcceptDrops(true);

    QString fileName = "./config.xml";  // 也可以使用其他固定路径
    if (QFile::exists(fileName))
    {
        importData(fileName);
    }

}

void KMainWindow::onExportButtonClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export to XML"), "", tr("XML Files (*.xml)"));
    if (!fileName.isEmpty())
    {
        exportData(fileName);
    }
}

void KMainWindow::onImportButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Import from XML"), "", tr("XML Files (*.xml)"));
    if (!fileName.isEmpty())
    {
        importData(fileName);
    }
}

void KMainWindow::exportData(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Failed to open file for writing.";
        return;
    }

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("DirectoryWatchers");

    int rowCount = m_pCentralWidget->m_pTableView1->rowCount();
    int colCount = m_pCentralWidget->m_pTableView1->columnCount();

    for (int row = 0; row < rowCount; ++row)
    {
        xmlWriter.writeStartElement("DirectoryWatcher");

        for (int col = 0; col < colCount; ++col)
        {
            QTableWidgetItem* item = m_pCentralWidget->m_pTableView1->item(row, col);
            QString text = item ? item->text() : "";

            if (col == 0) // Directory
            {
                xmlWriter.writeTextElement("Directory", text);
            }
            else if (col >= 2 && col <= 6) // QCheckBox 状态
            {
                xmlWriter.writeTextElement("Event" + QString::number(col - 1), item->checkState() == Qt::Checked ? "true" : "false");
            }
            else if (col == 7) // Watch Extensions
            {
                xmlWriter.writeTextElement("WatchExtensions", text);
            }
            else if (col == 8) // Ignore Extensions
            {
                xmlWriter.writeTextElement("IgnoreExtensions", text);
            }
            else if (col == 9) // Watch File Extensions
            {
                xmlWriter.writeTextElement("WatchFileExtensions", text);
            }
            else if (col == 10) // Ignore File Extensions
            {
                xmlWriter.writeTextElement("IgnoreFileExtensions", text);
            }
            else if (col == 11) // Timestamp
            {
                xmlWriter.writeTextElement("Timestamp", text);
            }
        }

        xmlWriter.writeEndElement(); 
    }

    xmlWriter.writeEndElement(); 
    xmlWriter.writeEndDocument();
}

void KMainWindow::importData(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open file for reading.";
        return;
    }

    QXmlStreamReader xmlReader(&file);

    while (!xmlReader.atEnd() && !xmlReader.hasError())
    {
        xmlReader.readNext();

        if (xmlReader.isStartElement())
        {
            if (xmlReader.name() == "DirectoryWatcher")
            {
                QString directory;
                QStringList watchExtensions, ignoreExtensions, watchFileExtensions, ignoreFileExtensions;
                QString timestamp;
                QMap<int, Qt::CheckState> eventStates;

                while (!(xmlReader.isEndElement() && xmlReader.name() == "DirectoryWatcher"))
                {
                    xmlReader.readNext();

                    if (xmlReader.isStartElement())
                    {
                        QString elementName = xmlReader.name().toString();
                        QString elementText = xmlReader.readElementText();

                        if (elementName == "Directory")
                        {
                            directory = elementText;
                        }
                        else if (elementName.startsWith("Event"))
                        {
                            int eventIndex = elementName.mid(5).toInt() + 1;
                            eventStates[eventIndex] = (elementText == "true") ? Qt::Checked : Qt::Unchecked;
                        }
                        else if (elementName == "WatchExtensions")
                        {
                            watchExtensions = elementText.split(", ", Qt::SkipEmptyParts);
                            qDebug() << "Parsed WatchExtensions:" << watchExtensions;
                        }
                        else if (elementName == "IgnoreExtensions")
                        {
                            ignoreExtensions = elementText.split(", ", Qt::SkipEmptyParts);
                            qDebug() << "Parsed IgnoreExtensions:" << ignoreExtensions;
                        }
                        else if (elementName == "WatchFileExtensions")
                        {
                            watchFileExtensions = elementText.split(", ", Qt::SkipEmptyParts);
                            qDebug() << "Parsed WatchFileExtensions:" << watchFileExtensions;
                        }
                        else if (elementName == "IgnoreFileExtensions")
                        {
                            ignoreFileExtensions = elementText.split(", ", Qt::SkipEmptyParts);
                            qDebug() << "Parsed IgnoreFileExtensions:" << ignoreFileExtensions;
                        }
                        else if (elementName == "Timestamp")
                        {
                            timestamp = elementText;
                        }
                    }
                }
                if (m_directoryWatchers.contains(directory))
                {
                    QMessageBox::warning(this, tr(u8"警告"), tr(u8"该目录已经在监听列表中."));
                    continue;
                }

                int row = m_pCentralWidget->m_pTableView1->rowCount();
                m_pCentralWidget->m_pTableView1->insertRow(row);
                m_pCentralWidget->m_pTableView1->setItem(row, 0, new QTableWidgetItem(directory));
                m_pCentralWidget->m_pTableView1->setItem(row, 1, new QTableWidgetItem(u8"是"));

                for (int col = 2; col <= 6; ++col)
                {
                    QTableWidgetItem* checkBoxItem = new QTableWidgetItem();
                    checkBoxItem->setCheckState(eventStates[col]);
                    m_pCentralWidget->m_pTableView1->setItem(row, col, checkBoxItem);
                }

                m_pCentralWidget->m_pTableView1->setItem(row, 7, new QTableWidgetItem(watchExtensions.join(", ")));
                m_pCentralWidget->m_pTableView1->setItem(row, 8, new QTableWidgetItem(ignoreExtensions.join(", ")));
                m_pCentralWidget->m_pTableView1->setItem(row, 9, new QTableWidgetItem(watchFileExtensions.join(", ")));
                m_pCentralWidget->m_pTableView1->setItem(row, 10, new QTableWidgetItem(ignoreFileExtensions.join(", ")));
                m_pCentralWidget->m_pTableView1->setItem(row, 11, new QTableWidgetItem(timestamp));

                KDirectoryWatcher* watcher = new KDirectoryWatcher(directory);
                QSet<int> allowedActions;
                bool watchSubdirectories;
                if (eventStates[2] == Qt::Checked)
                {
                    watchSubdirectories = true;
                    watcher->setWatchSubdirectories(watchSubdirectories);  
                }
                else if (eventStates[2] == Qt::Unchecked)
                {
                    watchSubdirectories = false;
                    watcher->setWatchSubdirectories(watchSubdirectories);
                }

                if (eventStates[3] == Qt::Checked) allowedActions.insert(FILE_ACTION_ADDED);
                if (eventStates[4] == Qt::Checked) allowedActions.insert(FILE_ACTION_REMOVED);
                if (eventStates[5] == Qt::Checked)
                {
                    allowedActions.insert(FILE_ACTION_RENAMED_OLD_NAME);
                    allowedActions.insert(FILE_ACTION_RENAMED_NEW_NAME);
                }
                if (eventStates[6] == Qt::Checked) allowedActions.insert(FILE_ACTION_MODIFIED);

                watcher->setAllowedActions(allowedActions);
                watcher->setExtensionsToWatch(watchExtensions);
                watcher->setExtensionsToIgnore(ignoreExtensions);
                watcher->setFilesToWatch(watchFileExtensions);
                watcher->setFilesToIgnore(ignoreFileExtensions);
                watcher->startWatching();

                m_directoryWatchers[directory] = watcher;
                (void)connect(watcher, &KDirectoryWatcher::fileChanged, this, &KMainWindow::onDirectoryChanged);
                watchedFiles.append(directory);
                (void)connect(m_pCentralWidget->m_pTableView1, &QTableWidget::cellChanged, this, &KMainWindow::onCheckboxStateChanged);
            }
        }
    }

    if (xmlReader.hasError())
    {
        qDebug() << "Error parsing XML: " << xmlReader.errorString();
    }
    updateStatusBar();
}

void KMainWindow::initWindows()
{
    m_pVLayout = new QVBoxLayout(this);
    m_pVLayout->setSpacing(0);
    m_pVLayout->setMargin(0);

    m_pMenuBar = new KMenuBar(this);

    m_pToolBar = new KToolBar(this);
    m_pCentralWidget = new KCentralWidget(this);
    m_pStatusBar = new QStatusBar(this);
    

    m_pVLayout->addWidget(m_pMenuBar);
    m_pVLayout->addWidget(m_pToolBar);
    m_pVLayout->addWidget(m_pCentralWidget);
    m_pVLayout->addWidget(m_pStatusBar);

    m_pSubDirListWidget = new QListWidget(this);
    m_pSubDirListWidget->setVisible(false); 
    m_pSubDirListWidget->setFixedSize(300, 200);
    m_pSubDirListWidget->setWindowFlags(Qt::Popup); 
    
    setLayout(m_pVLayout);
 }

void KMainWindow::onDirectoryTextChanged(const QString& directory)
{
    if (directory.endsWith("\\")) {
        QDir dir(directory);
        if (!dir.exists()) {
            m_pSubDirListWidget->setVisible(false);
            return;
        }

        QStringList subDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

        m_pSubDirListWidget->clear();
        m_pSubDirListWidget->addItems(subDirs);
        int width = m_pToolBar->m_pLineEdit->width();
        int height = qMin(200, subDirs.size() * 40); 

        m_pSubDirListWidget->setFixedSize(width, height);
        m_pSubDirListWidget->setVisible(!subDirs.isEmpty()); 

        QPoint pos = m_pToolBar->m_pLineEdit->mapToGlobal(QPoint(0, m_pToolBar->m_pLineEdit->height()));
        m_pSubDirListWidget->move(pos);
    }
    else {
        m_pSubDirListWidget->setVisible(false);
    }
}

void KMainWindow::onSubDirItemClicked(QListWidgetItem* item)
{
    QString subDir = item->text();
    QString directory = m_pToolBar->m_pLineEdit->text();
    QString fullPath = QDir(directory).filePath(subDir);
    m_pToolBar->m_pLineEdit->setText(fullPath); 
    m_pSubDirListWidget->setVisible(false); 
}

void KMainWindow::onAddFileButtonClicked()
{
    QString directory = m_pToolBar->m_pLineEdit->text();

    if (directory.trimmed().isEmpty()) {
        QMessageBox::warning(this, u8"无效输入", u8"请输入一个有效的目录路径。");
        return;
    }

    QDir dir(directory);
    if (!dir.exists()) {
        QMessageBox::warning(this, u8"无效的目录", u8"指定的目录不存在。请输入一个有效的目录。");
        return;
    }
    if (watchedFiles.contains(directory)) 
    {
        QMessageBox::warning(this, tr(u8"警告"), tr(u8"该目录已经在监听列表中"));
        return;
    }
    else
    {
        watchedFiles.append(directory);
    }
    m_pDirectoryWatcher = new KDirectoryWatcher(directory);
    m_directoryWatchers[directory] = m_pDirectoryWatcher;
    m_pDirectoryWatcher->startWatching();
    (void)connect(m_pDirectoryWatcher, &KDirectoryWatcher::fileChanged, this, &KMainWindow::onDirectoryChanged);
    int row = m_pCentralWidget->m_pTableView1->rowCount();
    m_pCentralWidget->m_pTableView1->insertRow(row);
    m_pCentralWidget->m_pTableView1->setItem(row, 0, new QTableWidgetItem(directory));

    for (int col = 2; col <= 6; ++col) { 
        QTableWidgetItem* checkBoxItem = new QTableWidgetItem();
        checkBoxItem->setFlags(checkBoxItem->flags() | Qt::ItemIsUserCheckable);
        checkBoxItem->setCheckState(Qt::Checked); 
        m_pCentralWidget->m_pTableView1->setItem(row, col, checkBoxItem);
        (void)connect(m_pCentralWidget->m_pTableView1, &QTableWidget::cellChanged, this, &KMainWindow::onCheckboxStateChanged);
    }
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-yy HH:mm:ss");
    m_pCentralWidget->m_pTableView1->setItem(row, 11, new QTableWidgetItem(currentTime));
    m_pCentralWidget->m_pTableView1->setItem(row, 1, new QTableWidgetItem(u8"是"));

    QString operation = QString::fromLocal8Bit("开始监听");

    QString details = QString(u8"文件: %1, 关注的后缀: 空, 忽略的后缀: 空, 指定关注文件: 空, 忽略指定的文件: 空")
        .arg(directory);
    addUserOperationLog(currentTime, operation, details);
    updateStatusBar();
}

void KMainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction(); // 允许拖放
    }
    else {
        event->ignore();
    }
}

void KMainWindow::dropEvent(QDropEvent* event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) return;

    for (const QUrl& url : urls) {
        QString filePath = url.toLocalFile();
        QFileInfo fileInfo(filePath);

        if (fileInfo.isDir()) {
            qDebug() << u8"拖入的文件夹：" << filePath;
            addWatchDirectory(filePath);  // 处理拖入的文件夹
        }
        else {
            qDebug() << u8"拖入的文件：" << filePath;
        }
    }
}

void KMainWindow::addWatchDirectory(const QString& directory)
{
    if (directory.isEmpty()) 
        return;

    if (m_directoryWatchers.contains(directory))
    {
        QMessageBox::warning(this, tr(u8"警告"), tr(u8"该目录已经在监听列表中."));
        return;
    }

    m_pToolBar->m_pLineEdit->setText(directory);
    m_pDirectoryWatcher = new KDirectoryWatcher(directory);
    m_directoryWatchers[directory] = m_pDirectoryWatcher;
    watchedFiles.append(directory);

    // 默认监听所有事件
    QSet<int> allowedActions = { FILE_ACTION_ADDED, FILE_ACTION_REMOVED, FILE_ACTION_MODIFIED, FILE_ACTION_RENAMED_OLD_NAME, FILE_ACTION_RENAMED_NEW_NAME };
    m_pDirectoryWatcher->setAllowedActions(allowedActions);
    m_pDirectoryWatcher->setWatchSubdirectories(true);  // 默认监听子目录

    QStringList watchExtensions, ignoreExtensions, watchFileExtensions, ignoreFileExtensions;
    m_pDirectoryWatcher->setExtensionsToWatch(watchExtensions);
    m_pDirectoryWatcher->setExtensionsToIgnore(ignoreExtensions);
    m_pDirectoryWatcher->setFilesToWatch(watchFileExtensions);
    m_pDirectoryWatcher->setFilesToIgnore(ignoreFileExtensions);

    m_pDirectoryWatcher->startWatching();
    (void)connect(m_pDirectoryWatcher, &KDirectoryWatcher::fileChanged, this, &KMainWindow::onDirectoryChanged);
    qDebug() << u8"拖入的文件夹路径:" << directory;

    // 插入到界面表格
    int row = m_pCentralWidget->m_pTableView1->rowCount();
    m_pCentralWidget->m_pTableView1->insertRow(row);
    m_pCentralWidget->m_pTableView1->setItem(row, 0, new QTableWidgetItem(directory));
    m_pCentralWidget->m_pTableView1->setItem(row, 1, new QTableWidgetItem(u8"是"));

    for (int col = 2; col <= 6; ++col)
    {
        QTableWidgetItem* checkBoxItem = new QTableWidgetItem();
        checkBoxItem->setCheckState(Qt::Checked);
        m_pCentralWidget->m_pTableView1->setItem(row, col, checkBoxItem);
        (void)connect(m_pCentralWidget->m_pTableView1, &QTableWidget::cellChanged, this, &KMainWindow::onCheckboxStateChanged);
    }

    m_pCentralWidget->m_pTableView1->setItem(row, 7, new QTableWidgetItem(watchExtensions.join(", ")));
    m_pCentralWidget->m_pTableView1->setItem(row, 8, new QTableWidgetItem(ignoreExtensions.join(", ")));
    m_pCentralWidget->m_pTableView1->setItem(row, 9, new QTableWidgetItem(watchFileExtensions.join(", ")));
    m_pCentralWidget->m_pTableView1->setItem(row, 10, new QTableWidgetItem(ignoreFileExtensions.join(", ")));

    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    m_pCentralWidget->m_pTableView1->setItem(row, 11, new QTableWidgetItem(currentTime));

    QString operation = QString::fromLocal8Bit("开始监听");
    QString details = QString(u8"文件: %1, 关注的后缀: 空, 忽略的后缀: 空, 指定关注文件: 空, 忽略指定的文件: 空").arg(directory);
    addUserOperationLog(currentTime, operation, details);

    updateStatusBar();
}


void KMainWindow::onOptionFileButtonClicked()
{
    CustomDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString directory = dialog.getFilePath();
        QSet<int> allowedActions = dialog.getAllowedActions();
        QList<QCheckBox*>selectedEvents = dialog.getCheckBoxes();

        if (!directory.isEmpty())
        {
            m_pToolBar->m_pLineEdit->setText(directory);
            m_pDirectoryWatcher = new KDirectoryWatcher(directory);
            m_directoryWatchers[directory] = m_pDirectoryWatcher;
            watchedFiles.append(directory);

            m_pDirectoryWatcher->setAllowedActions(allowedActions);
            m_pDirectoryWatcher->setWatchSubdirectories(dialog.isSubdirectoryWatched());

            QStringList watchExtensions = dialog.getWatchExtensions();
            m_pDirectoryWatcher->setExtensionsToWatch(watchExtensions);
            QStringList ignoreExtensions = dialog.getIgnoreExtensions();
            m_pDirectoryWatcher->setExtensionsToIgnore(ignoreExtensions);

            QStringList watchFileExtensions = dialog.getWatchFileExtensions();
            m_pDirectoryWatcher->setFilesToWatch(watchFileExtensions);
            QStringList ignoreFileExtensions = dialog.getIgnoreFileExtensions();
            m_pDirectoryWatcher->setFilesToIgnore(ignoreFileExtensions);

            m_pDirectoryWatcher->startWatching();
            (void)connect(m_pDirectoryWatcher, &KDirectoryWatcher::fileChanged, this, &KMainWindow::onDirectoryChanged);
            qDebug() << u8"选择的文件夹路径:" << directory;

            int row = m_pCentralWidget->m_pTableView1->rowCount();
            m_pCentralWidget->m_pTableView1->insertRow(row);
            m_pCentralWidget->m_pTableView1->setItem(row, 0, new QTableWidgetItem(directory));
            
            int col = 2;
            for (QCheckBox* checkBox : selectedEvents)
            {
                QTableWidgetItem* checkBoxItem = new QTableWidgetItem();
                checkBoxItem->setCheckState(checkBox->isChecked() ? Qt::Checked : Qt::Unchecked);
                m_pCentralWidget->m_pTableView1->setItem(row, col++, checkBoxItem);
                (void)connect(m_pCentralWidget->m_pTableView1, &QTableWidget::cellChanged, this, &KMainWindow::onCheckboxStateChanged);
            }
            QStringList watchExtensionsList = dialog.getWatchExtensions();
            QString watchExtensionsString;
            watchExtensionsString = watchExtensionsList.join(", ");
            m_pCentralWidget->m_pTableView1->setItem(row, 7, new QTableWidgetItem(watchExtensionsString));

            QStringList ignoreExtensionsList = dialog.getIgnoreExtensions();
            QString ignoreExtensionsString;
            ignoreExtensionsString = ignoreExtensionsList.join(", ");
            m_pCentralWidget->m_pTableView1->setItem(row, 8, new QTableWidgetItem(ignoreExtensionsString));

            QStringList watchFileExtensionsList = dialog.getWatchFileExtensions();
            QString watchFileExtensionsString;
            watchFileExtensionsString = watchFileExtensionsList.join(", ");
            m_pCentralWidget->m_pTableView1->setItem(row, 9, new QTableWidgetItem(watchFileExtensionsString));

            QStringList ignoreFileExtensionsList = dialog.getIgnoreFileExtensions();
            QString ignoreFileExtensionsString;
            ignoreFileExtensionsString = ignoreFileExtensionsList.join(", ");
            m_pCentralWidget->m_pTableView1->setItem(row, 10, new QTableWidgetItem(ignoreFileExtensionsString));

            QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
            m_pCentralWidget->m_pTableView1->setItem(row, 11, new QTableWidgetItem(currentTime));
            m_pCentralWidget->m_pTableView1->setItem(row, 1, new QTableWidgetItem(u8"是"));
            QString operation = QString::fromLocal8Bit("开始监听");

            QString reWatchExtensions = watchExtensionsList.isEmpty() ? u8"空" : watchExtensionsList.join(", ");
            QString reIgnoreExtensions = ignoreExtensionsList.isEmpty() ? u8"空" : ignoreExtensionsList.join(", ");
            QString reWatchFileExtensions = watchFileExtensionsList.isEmpty() ? u8"空" : watchFileExtensionsList.join(", ");
            QString reIgnoreFileExtensions = ignoreFileExtensionsList.isEmpty() ? u8"空" : ignoreFileExtensionsList.join(", ");
            QString details = QString(u8"文件: %1, 关注的后缀: %2, 忽略的后缀: %3, 指定关注文件: %4, 忽略指定的文件: %5")
                .arg(directory)
                .arg(reWatchExtensions)
                .arg(reIgnoreExtensions)
                .arg(reWatchFileExtensions)
                .arg(reIgnoreFileExtensions);
            addUserOperationLog(currentTime , operation, details);
        }
    }
    updateStatusBar();
}

const QList<QString>& KMainWindow::getWatchedFiles() const
{
    return watchedFiles;
}

void KMainWindow::onEditFileButtonClicked()
{
    int row = m_pCentralWidget->m_pTableView1->currentRow();
    if (row < 0)
    {
        QMessageBox::warning(this, tr(u8"警告"), tr(u8"请选择要编辑的条目"));
        return;
    }

    QString directory = m_pCentralWidget->m_pTableView1->item(row, 0)->text();
    if (directory.isEmpty() || !m_directoryWatchers.contains(directory))
    {
        QMessageBox::warning(this, tr(u8"警告"), tr(u8"无效的目录"));
        return;
    }

    KDirectoryWatcher* watcher = m_directoryWatchers[directory];

    CustomDialog dialog(this);
    dialog.setFilePath(directory);
    dialog.setWatchExtensions(watcher->getExtensionsToWatch());
    dialog.setIgnoreExtensions(watcher->getExtensionsToIgnore());
    dialog.setWatchFiles(watcher->getFilesToWatch());
    dialog.setIgnoreFiles(watcher->getFilesToIgnore());
    dialog.setSubdirectoryWatched(watcher->isWatchSubdirectories());
    dialog.setDirectoryLineEditReadOnly(true);

    if (dialog.exec() == QDialog::Accepted)
    {
        QString newDirectory = dialog.getFilePath();
        QStringList watchExtensions = dialog.getWatchExtensions();
        QStringList ignoreExtensions = dialog.getIgnoreExtensions();
        QStringList watchFiles = dialog.getWatchFileExtensions();
        QStringList ignoreFiles = dialog.getIgnoreFileExtensions();
        QSet<int> allowedActions = dialog.getAllowedActions();
        QList<QCheckBox*> selectedEvents = dialog.getCheckBoxes();
        watchedFiles.removeOne(directory);

        bool isSubdirectoryChecked = dialog.isSubdirectoryWatched();
        watcher->setWatchSubdirectories(isSubdirectoryChecked);

        m_pCentralWidget->m_pTableView1->item(row, 0)->setText(newDirectory);
        watchedFiles.append(newDirectory);
        watcher->setAllowedActions(allowedActions);
        int col = 2;
        for (QCheckBox* checkBox : selectedEvents)
        {
            QTableWidgetItem* checkBoxItem = new QTableWidgetItem();
            checkBoxItem->setCheckState(checkBox->isChecked() ? Qt::Checked : Qt::Unchecked);
            m_pCentralWidget->m_pTableView1->setItem(row, col++, checkBoxItem);
            (void)connect(m_pCentralWidget->m_pTableView1, &QTableWidget::cellChanged, this, &KMainWindow::onCheckboxStateChanged);
        }

        QString watchExtensionsString = watchExtensions.join(", ");
        m_pCentralWidget->m_pTableView1->setItem(row, 7, new QTableWidgetItem(watchExtensionsString));

        QString ignoreExtensionsString = ignoreExtensions.join(", ");
        m_pCentralWidget->m_pTableView1->setItem(row, 8, new QTableWidgetItem(ignoreExtensionsString));

        QString watchFilesString = watchFiles.join(", ");
        m_pCentralWidget->m_pTableView1->setItem(row, 9, new QTableWidgetItem(watchFilesString));

        QString ignoreFilesString = ignoreFiles.join(", ");
        m_pCentralWidget->m_pTableView1->setItem(row, 10, new QTableWidgetItem(ignoreFilesString));

        QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        m_pCentralWidget->m_pTableView1->setItem(row, 11, new QTableWidgetItem(currentTime));

        QString operation = QString::fromLocal8Bit("编辑文件监视配置");
        QString reWatchExtensions = watchExtensions.isEmpty() ? u8"空" : watchExtensions.join(", ");
        QString reIgnoreExtensions = ignoreExtensions.isEmpty() ? u8"空" : ignoreExtensions.join(", ");
        QString reWatchFiles = watchFiles.isEmpty() ? u8"空" : watchFiles.join(", ");
        QString reIgnoreFiles = ignoreFiles.isEmpty() ? u8"空" : ignoreFiles.join(", ");

        QString details = QString(u8"目录: %1, 关注的后缀: %2, 忽略的后缀: %3, 指定关注文件: %4, 忽略指定的文件: %5")
            .arg(newDirectory)
            .arg(reWatchExtensions)
            .arg(reIgnoreExtensions)
            .arg(reWatchFiles)
            .arg(reIgnoreFiles);

        if (directory != newDirectory)
        {
            watcher->stopWatching();
            m_directoryWatchers.remove(directory);
            watchedFiles.removeOne(directory);

            KDirectoryWatcher* newWatcher = new KDirectoryWatcher(newDirectory);
            newWatcher->setAllowedActions(allowedActions);
            newWatcher->setExtensionsToWatch(watchExtensions);
            newWatcher->setExtensionsToIgnore(ignoreExtensions);
            newWatcher->setFilesToWatch(watchFiles);
            newWatcher->setFilesToIgnore(ignoreFiles);

            m_directoryWatchers[newDirectory] = newWatcher;

            newWatcher->startWatching();
            (void)connect(newWatcher, &KDirectoryWatcher::fileChanged, this, &KMainWindow::onDirectoryChanged);
        }
        else
        {
            watcher->clearExtensionsToWatch();
            watcher->clearExtensionsToIgnore();
            watcher->clearFilesToWatch();
            watcher->clearFilesToIgnore();

            watcher->setExtensionsToWatch(watchExtensions);
            watcher->setExtensionsToIgnore(ignoreExtensions);
            watcher->setFilesToWatch(watchFiles);
            watcher->setFilesToIgnore(ignoreFiles);

            watcher->startWatching();
        }
        addUserOperationLog(currentTime, operation, details);
    }
    updateStatusBar();
}

void KMainWindow::onCheckboxStateChanged(int row, int col)
{
    if (col >= 2 && col <= 6) 
    {
        QTableWidgetItem* item = m_pCentralWidget->m_pTableView1->item(row, col);
        if (!item) return;

        QString directory = m_pCentralWidget->m_pTableView1->item(row, 0)->text();
        KDirectoryWatcher* watcher = m_directoryWatchers.value(directory, nullptr);

        if (!watcher) return;

        Qt::CheckState checkState = item->checkState();
        QSet<int> allowedActions = watcher->getAllowedActions();

        switch (col) {
        case 2:
            bool watchSubdirectories;
            if (checkState == Qt::Checked)
            {
                watchSubdirectories = true;
                watcher->setWatchSubdirectories(watchSubdirectories);
            }
            else if(checkState == Qt::Unchecked)
            {
                watchSubdirectories = false;
                watcher->setWatchSubdirectories(watchSubdirectories);
            }
            break;
        case 3: 
            if (checkState == Qt::Checked)
                allowedActions.insert(FILE_ACTION_ADDED);
            else
                allowedActions.remove(FILE_ACTION_ADDED);
            break;
        case 4: 
            if (checkState == Qt::Checked)
                allowedActions.insert(FILE_ACTION_REMOVED);
            else
                allowedActions.remove(FILE_ACTION_REMOVED);
            break;
        case 6: 
            if (checkState == Qt::Checked)
                allowedActions.insert(FILE_ACTION_MODIFIED);
            else
                allowedActions.remove(FILE_ACTION_MODIFIED);
            break;
        case 5: 
            if (checkState == Qt::Checked) {
                allowedActions.insert(FILE_ACTION_RENAMED_OLD_NAME);
                allowedActions.insert(FILE_ACTION_RENAMED_NEW_NAME);
            }
            else {
                allowedActions.remove(FILE_ACTION_RENAMED_OLD_NAME);
                allowedActions.remove(FILE_ACTION_RENAMED_NEW_NAME);
            }
            break;
        default:
            return; 
        }

        watcher->setAllowedActions(allowedActions);
    }
}

void KMainWindow::onDirectoryChanged(const QString& directory, const QString& action)
{
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QTimer::singleShot(500, [this, directory, currentTime]() {
        m_pCentralWidget->backupFile(directory, currentTime);
        });
    QString message = QString("%1 - %2 - %3").arg(action).arg(currentTime).arg(directory);
    m_fileChangeQueue.enqueue(message);
   // KCentralWidget::createFileBackup(directory);
   // m_pCentralWidget->backupFile(directory, currentTime);
    m_pCentralWidget->insertData(action, currentTime, directory);

}

void KMainWindow::addUserOperationLog(const QString& time , const QString& operation, const QString& details)
{
    m_pCentralWidget->insertDataTwo(time, operation, details);

    m_pCentralWidget->m_pTableView3->scrollToBottom();
}

void KMainWindow::onTimerTimeout()
{
    if (!m_fileChangeQueue.isEmpty())
    {
        QString changesSummary;
        int count = 0;

        while (!m_fileChangeQueue.isEmpty() && count < 10) // 每次最多处理10条变化
        {
            changesSummary += m_fileChangeQueue.dequeue() + "\n";
            count++;
        }

        // 显示提示
        if (!changesSummary.isEmpty() && m_pToolBar->m_pEnableNotifications->isChecked())
        {
            QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information;
            m_SystemTray->showMessage(u8"文件变化", changesSummary, icon);
        }
    }
}

void KMainWindow::onSearchButtonClicked()
{
    QString keyword = m_pCentralWidget->m_pSearchLineEdit->text();

    QString filter = QString("messageType LIKE '%%1%'OR timestamp LIKE '%%1%' OR directory LIKE '%%1%'").arg(keyword);
    m_pCentralWidget->m_pTableViewModel2->setFilter(filter);
}

void KMainWindow::onResumeSearchClicked()
{
    m_pCentralWidget->m_pSearchLineEdit->clear();
    m_pCentralWidget->m_pTableViewModel2->setFilter("");
}

void KMainWindow::onInquireButtonClicked()
{
    m_previousFilter = m_pCentralWidget->m_pTableViewModel2->filter();
    QString begin = m_pCentralWidget->m_pDateTimeBeginEdit->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString end = m_pCentralWidget->m_pDateTimeEndEdit->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString filter = QString("timestamp BETWEEN '%1' AND '%2'").arg(begin, end);
    m_pCentralWidget->m_pTableViewModel2->setFilter(filter);
    m_pCentralWidget->m_pTableViewModel2->select();
}

void KMainWindow::onInquireResumeButtonClicked()
{
    m_pCentralWidget->m_pTableViewModel2->setFilter(m_previousFilter);
    m_pCentralWidget->m_pTableViewModel2->select();
}

void KMainWindow::onSearchButtonTwoClicked()
{
    QString keyword = m_pCentralWidget->m_pSearchLineEditTwo->text();

    QString filter = QString("timestamp LIKE '%%1%'OR operationType LIKE '%%1%' OR directory LIKE '%%1%'").arg(keyword);
    m_pCentralWidget->m_pTableViewModel3->setFilter(filter);
}

void KMainWindow::onResumeSearchTwoClicked()
{
    m_pCentralWidget->m_pSearchLineEditTwo->clear();
    m_pCentralWidget->m_pTableViewModel3->setFilter("");
}

void KMainWindow::onInquireButtonTwoClicked()
{
    m_previousTwoFilter = m_pCentralWidget->m_pTableViewModel3->filter();
    QString begin = m_pCentralWidget->m_pDateTimeBeginEditTwo->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString end = m_pCentralWidget->m_pDateTimeEndEditTwo->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString filter = QString("timestamp BETWEEN '%1' AND '%2'").arg(begin, end);
    m_pCentralWidget->m_pTableViewModel3->setFilter(filter);
    m_pCentralWidget->m_pTableViewModel3->select();
}

void KMainWindow::onInquireResumeButtonTwoClicked()
{
    m_pCentralWidget->m_pTableViewModel3->setFilter(m_previousTwoFilter);
    m_pCentralWidget->m_pTableViewModel3->select();
}

void KMainWindow::onStopAllClicked()
{
    for (int row = 0; row < m_pCentralWidget->m_pTableView1->rowCount(); ++row)
    {
        m_pCentralWidget->m_pTableView1->setItem(row, 1, new QTableWidgetItem(u8"否"));
    }

    QStringList stoppedDirectories;
    foreach(const QString & directory, m_directoryWatchers.keys())
    {
        if (m_directoryWatchers.contains(directory))
        {
            KDirectoryWatcher* watcher = m_directoryWatchers.value(directory);
            watcher->stopWatching();
            stoppedDirectories.append(directory);
            qDebug() << "PausedAll watching for:" << directory;
        }
    }

    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString operation = u8"停止所有文件监听";
    QString details = QString(u8"停止监听文件: %1").arg(stoppedDirectories.join(", "));
    addUserOperationLog(currentTime, operation, details);
    updateStatusBar();
}

void KMainWindow::onResumeAllClicked()
{
    for (int row = 0; row < m_pCentralWidget->m_pTableView1->rowCount(); ++row)
    {
        m_pCentralWidget->m_pTableView1->setItem(row, 1, new QTableWidgetItem(u8"是"));
    }

    QStringList resumedDirectories;
    foreach(const QString & directory, m_directoryWatchers.keys())
    {
        if (m_directoryWatchers.contains(directory))
        {
            KDirectoryWatcher* watcher = m_directoryWatchers.value(directory);
            watcher->startWatching();
            resumedDirectories.append(directory);
            qDebug() << "RemuseAll watching for:" << directory;
        }
    }

    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString operation = u8"恢复所有监听";
    QString details = QString(u8"恢复监听文件: %1").arg(resumedDirectories.join(", "));
    addUserOperationLog(currentTime, operation, details);
    updateStatusBar();
}

void KMainWindow::onResumeClicked()
{
    QList<QModelIndex> selectedRows = m_pCentralWidget->m_pTableView1->selectionModel()->selectedRows();

    if (selectedRows.isEmpty())
    {
        QMessageBox::warning(this, tr(u8"提示"), tr(u8"请先选中一行!"));
        return;
    }

    QSet<QString> directoriesToResume;
    QStringList resumedDirectories;

    // 遍历每个选中的行
    foreach(const QModelIndex & index, selectedRows)
    {
        int row = index.row();
        QString directory = m_pCentralWidget->m_pTableView1->item(row, 0)->text(); // 获取第一列（假设目录在第一列）
        directoriesToResume.insert(directory);

        int statusColumn = 1; // 假设状态在第二列
        m_pCentralWidget->m_pTableView1->setItem(row, statusColumn, new QTableWidgetItem(u8"是"));
    }

    // 恢复目录监控
    foreach(const QString & directory, directoriesToResume)
    {
        if (m_directoryWatchers.contains(directory))
        {
            KDirectoryWatcher* watcher = m_directoryWatchers[directory];
            watcher->startWatching();
            resumedDirectories.append(directory);
            qDebug() << "Resume watching for:" << directory;
        }
    }

    // 记录用户操作日志
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString operation = u8"恢复选中的监听";
    QString details = QString(u8"恢复监听的文件: %1").arg(resumedDirectories.join(", "));
    addUserOperationLog(currentTime, operation, details);
    updateStatusBar();

 
}

void KMainWindow::onStopClicked()
{
    QList<QModelIndex> selectedIndexes = m_pCentralWidget->m_pTableView1->selectionModel()->selectedRows();

    // 如果没有选中行，弹出提示
    if (selectedIndexes.isEmpty())
    {
        QMessageBox::warning(this, tr(u8"提示"), tr(u8"请先选中一行!"));
        return;
    }

    QSet<QString> directoriesToPause;
    QStringList pausedDirectories;

    foreach(QModelIndex index, selectedIndexes)
    {
        int row = index.row();
        QTableWidgetItem* directoryItem = m_pCentralWidget->m_pTableView1->item(row, 0);
        QTableWidgetItem* statusItem = m_pCentralWidget->m_pTableView1->item(row, 1);

        if (directoryItem && statusItem)
        {
            QString directory = directoryItem->text();
            statusItem->setText(u8"否");
            directoriesToPause.insert(directory);
            pausedDirectories.append(directory);
        }
    }

    foreach(const QString & directory, directoriesToPause)
    {
        if (m_directoryWatchers.contains(directory))
        {
            KDirectoryWatcher* watcher = m_directoryWatchers[directory];
            watcher->stopWatching();
            qDebug() << "Paused watching for:" << directory;
        }
    }

    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString operation = u8"停止选中的监听";
    QString details = QString(u8"停止监听文件: %1").arg(pausedDirectories.join(", "));
    addUserOperationLog(currentTime, operation, details);
    updateStatusBar();


}

void KMainWindow::onRemoveFileButtonClicked()
{
    // 获取所有选中的行的索引
    QList<QModelIndex> selectedRows = m_pCentralWidget->m_pTableView1->selectionModel()->selectedRows();

    if (selectedRows.isEmpty())
    {
        QMessageBox::warning(this, u8"无选择", u8"请先选择要删除的行。");
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, u8"确认删除", u8"确定要删除选中的行吗？", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        QSet<int> rowsToRemove;
        QStringList removedDirectories;

        foreach(const QModelIndex & index, selectedRows)
        {
            rowsToRemove.insert(index.row());
        }

        // 将行按降序排序，以避免删除时影响其他行的索引
        QList<int> sortedRows = rowsToRemove.values();
        std::sort(sortedRows.begin(), sortedRows.end(), std::greater<int>());

        foreach(int row, sortedRows)
        {
            QString directory = m_pCentralWidget->m_pTableView1->item(row, 0)->text();
            removedDirectories.append(directory);
            m_pCentralWidget->m_pTableView1->removeRow(row);

            // 停止监控并移除监控对象
            if (m_directoryWatchers.contains(directory))
            {
                KDirectoryWatcher* watcher = m_directoryWatchers[directory];
                watchedFiles.removeOne(directory);
                watcher->stopWatching();
                m_directoryWatchers.remove(directory);
            }
        }

        QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        QString operation = u8"移除选中的行";
        QString details = QString(u8"移除文件: %1").arg(removedDirectories.join(", "));
        addUserOperationLog(currentTime, operation, details);
    }
    updateStatusBar();

 
}

void KMainWindow::onRemoveAllFileButtonClicked()
{
    // 获取选中行
    QList<QModelIndex> selectedRows = m_pCentralWidget->m_pTableView1->selectionModel()->selectedRows();
    QStringList removedDirectories;

    if (selectedRows.isEmpty())
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, u8"确认删除",
            u8"没有选择任何行。是否删除所有行？", QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes)
        {
            int rowCount = m_pCentralWidget->m_pTableView1->rowCount();
            for (int row = rowCount - 1; row >= 0; --row)
            {
                QString directory = m_pCentralWidget->m_pTableView1->item(row, 0)->text();
                removedDirectories.append(directory);
                m_pCentralWidget->m_pTableView1->removeRow(row);

                if (m_directoryWatchers.contains(directory))
                {
                    KDirectoryWatcher* watcher = m_directoryWatchers[directory];
                    watchedFiles.removeOne(directory);
                    watcher->stopWatching();
                    m_directoryWatchers.remove(directory);
                }
            }

            QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
            QString operation = u8"移除所有行";
            QString details = QString(u8"移除文件: %1").arg(removedDirectories.join(", "));
            addUserOperationLog(currentTime, operation, details);
        }
    }
    updateStatusBar();
}

void KMainWindow::exportLogs(const QString& filePath)
{
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr(u8"无法打开文件"), tr(u8"无法打开文件进行写入"));
        return;
    }

    QTextStream out(&file);

    // 写入CSV头部
    out << u8"ID,Type,Time,Path\n";

    QSqlQuery query("SELECT * FROM FileChanges");

    while (query.next())
    {
        QString id = query.value(0).toString();
        QString messageType = query.value(1).toString();
        QString timestamp = query.value(2).toString();
        QString directory = query.value(3).toString();

        out << id << "," << messageType << "," << timestamp << "," << directory << "\n";
    }

    file.close();

    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString operation = u8"导出日志";
    QString details = QString(u8"导出日志: %1").arg(filePath);
    addUserOperationLog(currentTime, operation, details);

    QMessageBox::information(this, tr(u8"导出成功"), tr(u8"日志已成功导出到 ") + filePath);
}

void KMainWindow::onSaveLogButtonClicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr(u8"导出日志"), "", tr("CSV Files (*.csv);;Text Files (*.txt)"));

    if (filePath.isEmpty())
        return; 

    exportLogs(filePath);
}

void KMainWindow::clearLog()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(u8"清空日志");
    msgBox.setText(u8"此操作会清空所有历史记录，您确定要清空所有日志记录吗？");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    int reply = msgBox.exec();

    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString operation = u8"清空日志";
    QString details;

    if (reply == QMessageBox::Yes)
    {
        clearDatabase();
        m_pCentralWidget->m_pTableViewModel2->select();
        m_pCentralWidget->m_pTableViewModel3->select();

        details = u8"日志已成功清空。";
        QMessageBox::information(this, u8"清空日志", u8"日志已成功清空。");
    }
    else
    {
        details = u8"日志未被清空。";
        QMessageBox::information(this, u8"清空日志", u8"日志未被清空。");
    }
    addUserOperationLog(currentTime, operation, details);
}

void KMainWindow::init_systemTrayIcon()
{
    m_SystemTray = new QSystemTrayIcon(this);
    m_SystemTray->setIcon(QIcon(":/icons/normal/option.png"));
    m_SystemTray->setToolTip(u8"FileSystem");
    (void)connect(m_SystemTray, &QSystemTrayIcon::activated, this, &KMainWindow::systemTrayIcon_activated);

    QAction* action_systemTray_quit = new QAction(QIcon(":/icons/normal/quit.png"), u8"退出应用");
    (void)connect(action_systemTray_quit, &QAction::triggered, this, &KMainWindow::quitMusicPlayer);

    QMenu* pContextMenu = new QMenu(this);
    pContextMenu->addAction(action_systemTray_quit);
    m_SystemTray->setContextMenu(pContextMenu);
    m_SystemTray->show();
}

void KMainWindow::quitMusicPlayer()
{
    QCoreApplication::quit();
}

void KMainWindow::systemTrayIcon_activated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::DoubleClick:
        if (isHidden())
        {
            show();
        }
        else
        {
            hide();
        }
        break;
    default:
        break;
    }
}

void KMainWindow::createAction()
{
    m_pStatusBar->showMessage(u8"就绪"); 
}

void KMainWindow::updateStatusBar()
{
    int count = 0;
    for (int i = 0; i < m_pCentralWidget->m_pTableView1->rowCount(); ++i)
    {
        QTableWidgetItem* statusItem = m_pCentralWidget->m_pTableView1->item(i, 1);
        if (statusItem && statusItem->text() == u8"是")
        {
            ++count;
        }
    }
    m_pStatusBar->showMessage(QString(u8"正在监视 %1 个文件夹").arg(count));
}

void KMainWindow::contextMenuEvent(QContextMenuEvent* event)
{
    m_pContextMenu->exec(event->globalPos());
}

void KMainWindow::initContextMenu()
{
    m_pContextMenu = new QMenu(this);

    // 恢复
    QAction* resumeSelectAll = new QAction(this);
    resumeSelectAll->setText(u8"恢复");
    resumeSelectAll->setIcon(QIcon(":/icons/normal/play.png"));
    this->addAction(resumeSelectAll);
    m_pContextMenu->addAction(resumeSelectAll);
    (void)connect(resumeSelectAll, &QAction::triggered, this, &KMainWindow::onResumeClicked);

    // 暂停
    QAction* stopAction = new QAction(this);
    stopAction->setText(u8"暂停");
    stopAction->setIcon(QIcon(":/icons/normal/pause.png"));
    this->addAction(stopAction);
    m_pContextMenu->addAction(stopAction);
    (void)connect(stopAction, &QAction::triggered, this, &KMainWindow::onStopClicked);

    m_pContextMenu->addSeparator();

    // 全部暂停
    QAction* stopAllAction = new QAction(this);
    stopAllAction->setText(u8"全部暂停");
    stopAllAction->setIcon(QIcon(":/icons/normal/pauseall.png"));
    this->addAction(stopAllAction);
    m_pContextMenu->addAction(stopAllAction);
    (void)connect(stopAllAction, &QAction::triggered, this, &KMainWindow::onStopAllClicked);

    // 全部恢复
    QAction* resumeAllAction = new QAction(this);
    resumeAllAction->setText(u8"全部恢复");
    resumeAllAction->setIcon(QIcon(":/icons/normal/playall.png"));
    this->addAction(resumeAllAction);
    m_pContextMenu->addAction(resumeAllAction);
    (void)connect(resumeAllAction, &QAction::triggered, this, &KMainWindow::onResumeAllClicked);

    m_pContextMenu->addSeparator();

    // 编辑
    QAction* editAction = new QAction(this);
    editAction->setText(u8"编辑");
    editAction->setIcon(QIcon(":/icons/normal/edit.png"));
    this->addAction(editAction);
    m_pContextMenu->addAction(editAction);
    (void)connect(editAction, &QAction::triggered, this, &KMainWindow::onEditFileButtonClicked);

    // 打开
    QAction* openFileAction = new QAction(this);
    openFileAction->setText(u8"打开文件");
    openFileAction->setIcon(QIcon(":/icons/normal/open.png"));
    this->addAction(openFileAction);
    m_pContextMenu->addAction(openFileAction);
    (void)connect(openFileAction, &QAction::triggered, this, &KMainWindow::onOpenFileButtonClicked);

    m_pContextMenu->addSeparator();

    // 移除
    QAction* removeAction = new QAction(this);
    removeAction->setText(u8"移除");
    removeAction->setIcon(QIcon(":/icons/normal/move.png"));
    this->addAction(removeAction);
    m_pContextMenu->addAction(removeAction);
    (void)connect(removeAction, &QAction::triggered, this, &KMainWindow::onRemoveFileButtonClicked);

    // 全部移除
    QAction* removeAllAction = new QAction(this);
    removeAllAction->setText(u8"全部移除");
    removeAllAction->setIcon(QIcon(":/icons/normal/moveall.png"));
    this->addAction(removeAllAction);
    m_pContextMenu->addAction(removeAllAction);
    (void)connect(removeAllAction, &QAction::triggered, this, &KMainWindow::onRemoveAllFileButtonClicked);

    m_pContextMenu->addSeparator();

    // 导出日志
    QAction* saveLogAction = new QAction(this);
    saveLogAction->setText(u8"导出日志");
    saveLogAction->setIcon(QIcon(":/icons/normal/right.png"));
    this->addAction(saveLogAction);
    m_pContextMenu->addAction(saveLogAction);
    (void)connect(saveLogAction, &QAction::triggered, this, &KMainWindow::onSaveLogButtonClicked);

    // 清空日志
    QAction* clearAction = new QAction(this);
    clearAction->setText(u8"清空日志");
    clearAction->setIcon(QIcon(":/icons/normal/delete.png"));
    this->addAction(clearAction);
    m_pContextMenu->addAction(clearAction);
    (void)connect(clearAction, &QAction::triggered, this, &KMainWindow::clearLog);

}

void KMainWindow::onOpenFileButtonClicked()
{
    QList<QTableWidgetItem*> selectedItems = m_pCentralWidget->m_pTableView1->selectedItems();

    if (selectedItems.isEmpty())
    {
        QMessageBox::warning(this, tr(u8"提示"), tr(u8"请先选中一行!"));
        return;
    }

    QTableWidgetItem* item = selectedItems.first(); // 获取第一个选中的项
    int row = item->row();
    QString filePath = m_pCentralWidget->m_pTableView1->item(row, 0)->text(); 

    if (!filePath.isEmpty())
    {
        QUrl fileUrl = QUrl::fromLocalFile(filePath);
        if (!QDesktopServices::openUrl(fileUrl))
        {
            QMessageBox::warning(this, tr(u8"错误"), tr(u8"无法打开文件: %1").arg(filePath));
        }
    }
    else
    {
        QMessageBox::warning(this, tr(u8"错误"), tr(u8"选中的行没有文件路径!"));
    }
}

void KMainWindow::closeEvent(QCloseEvent* event)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(u8"确认退出");
    msgBox.setText(u8"您确定要退出程序吗？“Yes”确认退出，“No”最小化到托盘");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    QCheckBox* deleteHistoryCheckBox = new QCheckBox(u8"删除历史记录", &msgBox);
    deleteHistoryCheckBox->setChecked(false); 

    msgBox.setCheckBox(deleteHistoryCheckBox);

    QString fileName = "./config.xml";
    exportData(fileName);

    int reply = msgBox.exec();

    if (reply == QMessageBox::Yes)
    {
        onStopAllClicked();
        if (deleteHistoryCheckBox->isChecked())
        {
            clearDatabase();
        }
        event->accept();
    }
    else
    {
        this->hide();
        event->ignore();
    }
}

void KMainWindow::clearDatabase()
{
    QSqlQuery query;
    if (!query.exec("DELETE FROM FileChanges"))
    {
        qDebug() << u8"清理数据库失败." << query.lastError();
    }
    else
    {
        qDebug() << u8"数据库清除成功";
    }

    if (!query.exec("DELETE FROM UserOperation"))
    {
        qDebug() << u8"清理 userOperation 表失败:" << query.lastError();
    }
    else
    {
        qDebug() << u8"userOperation 表清除成功";
    }
}
