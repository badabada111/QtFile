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
    init_systemTrayIcon();//ϵͳ����
    initContextMenu();
    (void)connect(m_pToolBar->m_pOptionFile, &QPushButton::clicked, this, &KMainWindow::onOptionFileButtonClicked);
    (void)connect(m_pToolBar->m_pAddFile, &QPushButton::clicked, this, &KMainWindow::onAddFileButtonClicked);
    (void)connect(m_pCentralWidget->m_pSearchButton, &QPushButton::clicked, this, &KMainWindow::onSearchButtonClicked);//�ؼ���
    (void)connect(m_pCentralWidget->m_pSortButton, &QPushButton::clicked, this, &KMainWindow::onResumeSearchClicked);//�ؼ���
    (void)connect(m_pCentralWidget->m_pInquireButton, &QPushButton::clicked, this, &KMainWindow::onInquireButtonClicked);//ʱ���
    (void)connect(m_pCentralWidget->m_pInquireRemuseButton, &QPushButton::clicked, this, &KMainWindow::onResumeSearchClicked);//�ָ�ʱ���

    (void)connect(m_pCentralWidget->m_pSearchButtonTwo, &QPushButton::clicked, this, &KMainWindow::onSearchButtonTwoClicked);//�ؼ���
    (void)connect(m_pCentralWidget->m_pSortButtonTwo, &QPushButton::clicked, this, &KMainWindow::onResumeSearchTwoClicked);//�ؼ���
    (void)connect(m_pCentralWidget->m_pInquireButtonTwo, &QPushButton::clicked, this, &KMainWindow::onInquireButtonTwoClicked);//ʱ���
    (void)connect(m_pCentralWidget->m_pInquireRemuseButtonTwo, &QPushButton::clicked, this, &KMainWindow::onResumeSearchTwoClicked);//�ָ�ʱ���

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
    m_pTimer->start(1000); // ÿ 5 ����һ��
    //loadTableViewData();
    setAcceptDrops(true);

    QString fileName = "./config.xml";  // Ҳ����ʹ�������̶�·��
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
            else if (col >= 2 && col <= 6) // QCheckBox ״̬
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
                    QMessageBox::warning(this, tr(u8"����"), tr(u8"��Ŀ¼�Ѿ��ڼ����б���."));
                    continue;
                }

                int row = m_pCentralWidget->m_pTableView1->rowCount();
                m_pCentralWidget->m_pTableView1->insertRow(row);
                m_pCentralWidget->m_pTableView1->setItem(row, 0, new QTableWidgetItem(directory));
                m_pCentralWidget->m_pTableView1->setItem(row, 1, new QTableWidgetItem(u8"��"));

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
        QMessageBox::warning(this, u8"��Ч����", u8"������һ����Ч��Ŀ¼·����");
        return;
    }

    QDir dir(directory);
    if (!dir.exists()) {
        QMessageBox::warning(this, u8"��Ч��Ŀ¼", u8"ָ����Ŀ¼�����ڡ�������һ����Ч��Ŀ¼��");
        return;
    }
    if (watchedFiles.contains(directory)) 
    {
        QMessageBox::warning(this, tr(u8"����"), tr(u8"��Ŀ¼�Ѿ��ڼ����б���"));
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
    m_pCentralWidget->m_pTableView1->setItem(row, 1, new QTableWidgetItem(u8"��"));

    QString operation = QString::fromLocal8Bit("��ʼ����");

    QString details = QString(u8"�ļ�: %1, ��ע�ĺ�׺: ��, ���Եĺ�׺: ��, ָ����ע�ļ�: ��, ����ָ�����ļ�: ��")
        .arg(directory);
    addUserOperationLog(currentTime, operation, details);
    updateStatusBar();
}

void KMainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction(); // �����Ϸ�
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
            qDebug() << u8"������ļ��У�" << filePath;
            addWatchDirectory(filePath);  // ����������ļ���
        }
        else {
            qDebug() << u8"������ļ���" << filePath;
        }
    }
}

void KMainWindow::addWatchDirectory(const QString& directory)
{
    if (directory.isEmpty()) 
        return;

    if (m_directoryWatchers.contains(directory))
    {
        QMessageBox::warning(this, tr(u8"����"), tr(u8"��Ŀ¼�Ѿ��ڼ����б���."));
        return;
    }

    m_pToolBar->m_pLineEdit->setText(directory);
    m_pDirectoryWatcher = new KDirectoryWatcher(directory);
    m_directoryWatchers[directory] = m_pDirectoryWatcher;
    watchedFiles.append(directory);

    // Ĭ�ϼ��������¼�
    QSet<int> allowedActions = { FILE_ACTION_ADDED, FILE_ACTION_REMOVED, FILE_ACTION_MODIFIED, FILE_ACTION_RENAMED_OLD_NAME, FILE_ACTION_RENAMED_NEW_NAME };
    m_pDirectoryWatcher->setAllowedActions(allowedActions);
    m_pDirectoryWatcher->setWatchSubdirectories(true);  // Ĭ�ϼ�����Ŀ¼

    QStringList watchExtensions, ignoreExtensions, watchFileExtensions, ignoreFileExtensions;
    m_pDirectoryWatcher->setExtensionsToWatch(watchExtensions);
    m_pDirectoryWatcher->setExtensionsToIgnore(ignoreExtensions);
    m_pDirectoryWatcher->setFilesToWatch(watchFileExtensions);
    m_pDirectoryWatcher->setFilesToIgnore(ignoreFileExtensions);

    m_pDirectoryWatcher->startWatching();
    (void)connect(m_pDirectoryWatcher, &KDirectoryWatcher::fileChanged, this, &KMainWindow::onDirectoryChanged);
    qDebug() << u8"������ļ���·��:" << directory;

    // ���뵽������
    int row = m_pCentralWidget->m_pTableView1->rowCount();
    m_pCentralWidget->m_pTableView1->insertRow(row);
    m_pCentralWidget->m_pTableView1->setItem(row, 0, new QTableWidgetItem(directory));
    m_pCentralWidget->m_pTableView1->setItem(row, 1, new QTableWidgetItem(u8"��"));

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

    QString operation = QString::fromLocal8Bit("��ʼ����");
    QString details = QString(u8"�ļ�: %1, ��ע�ĺ�׺: ��, ���Եĺ�׺: ��, ָ����ע�ļ�: ��, ����ָ�����ļ�: ��").arg(directory);
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
            qDebug() << u8"ѡ����ļ���·��:" << directory;

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
            m_pCentralWidget->m_pTableView1->setItem(row, 1, new QTableWidgetItem(u8"��"));
            QString operation = QString::fromLocal8Bit("��ʼ����");

            QString reWatchExtensions = watchExtensionsList.isEmpty() ? u8"��" : watchExtensionsList.join(", ");
            QString reIgnoreExtensions = ignoreExtensionsList.isEmpty() ? u8"��" : ignoreExtensionsList.join(", ");
            QString reWatchFileExtensions = watchFileExtensionsList.isEmpty() ? u8"��" : watchFileExtensionsList.join(", ");
            QString reIgnoreFileExtensions = ignoreFileExtensionsList.isEmpty() ? u8"��" : ignoreFileExtensionsList.join(", ");
            QString details = QString(u8"�ļ�: %1, ��ע�ĺ�׺: %2, ���Եĺ�׺: %3, ָ����ע�ļ�: %4, ����ָ�����ļ�: %5")
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
        QMessageBox::warning(this, tr(u8"����"), tr(u8"��ѡ��Ҫ�༭����Ŀ"));
        return;
    }

    QString directory = m_pCentralWidget->m_pTableView1->item(row, 0)->text();
    if (directory.isEmpty() || !m_directoryWatchers.contains(directory))
    {
        QMessageBox::warning(this, tr(u8"����"), tr(u8"��Ч��Ŀ¼"));
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

        QString operation = QString::fromLocal8Bit("�༭�ļ���������");
        QString reWatchExtensions = watchExtensions.isEmpty() ? u8"��" : watchExtensions.join(", ");
        QString reIgnoreExtensions = ignoreExtensions.isEmpty() ? u8"��" : ignoreExtensions.join(", ");
        QString reWatchFiles = watchFiles.isEmpty() ? u8"��" : watchFiles.join(", ");
        QString reIgnoreFiles = ignoreFiles.isEmpty() ? u8"��" : ignoreFiles.join(", ");

        QString details = QString(u8"Ŀ¼: %1, ��ע�ĺ�׺: %2, ���Եĺ�׺: %3, ָ����ע�ļ�: %4, ����ָ�����ļ�: %5")
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

        while (!m_fileChangeQueue.isEmpty() && count < 10) // ÿ����ദ��10���仯
        {
            changesSummary += m_fileChangeQueue.dequeue() + "\n";
            count++;
        }

        // ��ʾ��ʾ
        if (!changesSummary.isEmpty() && m_pToolBar->m_pEnableNotifications->isChecked())
        {
            QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information;
            m_SystemTray->showMessage(u8"�ļ��仯", changesSummary, icon);
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
        m_pCentralWidget->m_pTableView1->setItem(row, 1, new QTableWidgetItem(u8"��"));
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
    QString operation = u8"ֹͣ�����ļ�����";
    QString details = QString(u8"ֹͣ�����ļ�: %1").arg(stoppedDirectories.join(", "));
    addUserOperationLog(currentTime, operation, details);
    updateStatusBar();
}

void KMainWindow::onResumeAllClicked()
{
    for (int row = 0; row < m_pCentralWidget->m_pTableView1->rowCount(); ++row)
    {
        m_pCentralWidget->m_pTableView1->setItem(row, 1, new QTableWidgetItem(u8"��"));
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
    QString operation = u8"�ָ����м���";
    QString details = QString(u8"�ָ������ļ�: %1").arg(resumedDirectories.join(", "));
    addUserOperationLog(currentTime, operation, details);
    updateStatusBar();
}

void KMainWindow::onResumeClicked()
{
    QList<QModelIndex> selectedRows = m_pCentralWidget->m_pTableView1->selectionModel()->selectedRows();

    if (selectedRows.isEmpty())
    {
        QMessageBox::warning(this, tr(u8"��ʾ"), tr(u8"����ѡ��һ��!"));
        return;
    }

    QSet<QString> directoriesToResume;
    QStringList resumedDirectories;

    // ����ÿ��ѡ�е���
    foreach(const QModelIndex & index, selectedRows)
    {
        int row = index.row();
        QString directory = m_pCentralWidget->m_pTableView1->item(row, 0)->text(); // ��ȡ��һ�У�����Ŀ¼�ڵ�һ�У�
        directoriesToResume.insert(directory);

        int statusColumn = 1; // ����״̬�ڵڶ���
        m_pCentralWidget->m_pTableView1->setItem(row, statusColumn, new QTableWidgetItem(u8"��"));
    }

    // �ָ�Ŀ¼���
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

    // ��¼�û�������־
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString operation = u8"�ָ�ѡ�еļ���";
    QString details = QString(u8"�ָ��������ļ�: %1").arg(resumedDirectories.join(", "));
    addUserOperationLog(currentTime, operation, details);
    updateStatusBar();

 
}

void KMainWindow::onStopClicked()
{
    QList<QModelIndex> selectedIndexes = m_pCentralWidget->m_pTableView1->selectionModel()->selectedRows();

    // ���û��ѡ���У�������ʾ
    if (selectedIndexes.isEmpty())
    {
        QMessageBox::warning(this, tr(u8"��ʾ"), tr(u8"����ѡ��һ��!"));
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
            statusItem->setText(u8"��");
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
    QString operation = u8"ֹͣѡ�еļ���";
    QString details = QString(u8"ֹͣ�����ļ�: %1").arg(pausedDirectories.join(", "));
    addUserOperationLog(currentTime, operation, details);
    updateStatusBar();


}

void KMainWindow::onRemoveFileButtonClicked()
{
    // ��ȡ����ѡ�е��е�����
    QList<QModelIndex> selectedRows = m_pCentralWidget->m_pTableView1->selectionModel()->selectedRows();

    if (selectedRows.isEmpty())
    {
        QMessageBox::warning(this, u8"��ѡ��", u8"����ѡ��Ҫɾ�����С�");
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, u8"ȷ��ɾ��", u8"ȷ��Ҫɾ��ѡ�е�����", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        QSet<int> rowsToRemove;
        QStringList removedDirectories;

        foreach(const QModelIndex & index, selectedRows)
        {
            rowsToRemove.insert(index.row());
        }

        // ���а����������Ա���ɾ��ʱӰ�������е�����
        QList<int> sortedRows = rowsToRemove.values();
        std::sort(sortedRows.begin(), sortedRows.end(), std::greater<int>());

        foreach(int row, sortedRows)
        {
            QString directory = m_pCentralWidget->m_pTableView1->item(row, 0)->text();
            removedDirectories.append(directory);
            m_pCentralWidget->m_pTableView1->removeRow(row);

            // ֹͣ��ز��Ƴ���ض���
            if (m_directoryWatchers.contains(directory))
            {
                KDirectoryWatcher* watcher = m_directoryWatchers[directory];
                watchedFiles.removeOne(directory);
                watcher->stopWatching();
                m_directoryWatchers.remove(directory);
            }
        }

        QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        QString operation = u8"�Ƴ�ѡ�е���";
        QString details = QString(u8"�Ƴ��ļ�: %1").arg(removedDirectories.join(", "));
        addUserOperationLog(currentTime, operation, details);
    }
    updateStatusBar();

 
}

void KMainWindow::onRemoveAllFileButtonClicked()
{
    // ��ȡѡ����
    QList<QModelIndex> selectedRows = m_pCentralWidget->m_pTableView1->selectionModel()->selectedRows();
    QStringList removedDirectories;

    if (selectedRows.isEmpty())
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, u8"ȷ��ɾ��",
            u8"û��ѡ���κ��С��Ƿ�ɾ�������У�", QMessageBox::Yes | QMessageBox::No);

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
            QString operation = u8"�Ƴ�������";
            QString details = QString(u8"�Ƴ��ļ�: %1").arg(removedDirectories.join(", "));
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
        QMessageBox::warning(this, tr(u8"�޷����ļ�"), tr(u8"�޷����ļ�����д��"));
        return;
    }

    QTextStream out(&file);

    // д��CSVͷ��
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
    QString operation = u8"������־";
    QString details = QString(u8"������־: %1").arg(filePath);
    addUserOperationLog(currentTime, operation, details);

    QMessageBox::information(this, tr(u8"�����ɹ�"), tr(u8"��־�ѳɹ������� ") + filePath);
}

void KMainWindow::onSaveLogButtonClicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr(u8"������־"), "", tr("CSV Files (*.csv);;Text Files (*.txt)"));

    if (filePath.isEmpty())
        return; 

    exportLogs(filePath);
}

void KMainWindow::clearLog()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(u8"�����־");
    msgBox.setText(u8"�˲��������������ʷ��¼����ȷ��Ҫ���������־��¼��");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    int reply = msgBox.exec();

    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString operation = u8"�����־";
    QString details;

    if (reply == QMessageBox::Yes)
    {
        clearDatabase();
        m_pCentralWidget->m_pTableViewModel2->select();
        m_pCentralWidget->m_pTableViewModel3->select();

        details = u8"��־�ѳɹ���ա�";
        QMessageBox::information(this, u8"�����־", u8"��־�ѳɹ���ա�");
    }
    else
    {
        details = u8"��־δ����ա�";
        QMessageBox::information(this, u8"�����־", u8"��־δ����ա�");
    }
    addUserOperationLog(currentTime, operation, details);
}

void KMainWindow::init_systemTrayIcon()
{
    m_SystemTray = new QSystemTrayIcon(this);
    m_SystemTray->setIcon(QIcon(":/icons/normal/option.png"));
    m_SystemTray->setToolTip(u8"FileSystem");
    (void)connect(m_SystemTray, &QSystemTrayIcon::activated, this, &KMainWindow::systemTrayIcon_activated);

    QAction* action_systemTray_quit = new QAction(QIcon(":/icons/normal/quit.png"), u8"�˳�Ӧ��");
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
    m_pStatusBar->showMessage(u8"����"); 
}

void KMainWindow::updateStatusBar()
{
    int count = 0;
    for (int i = 0; i < m_pCentralWidget->m_pTableView1->rowCount(); ++i)
    {
        QTableWidgetItem* statusItem = m_pCentralWidget->m_pTableView1->item(i, 1);
        if (statusItem && statusItem->text() == u8"��")
        {
            ++count;
        }
    }
    m_pStatusBar->showMessage(QString(u8"���ڼ��� %1 ���ļ���").arg(count));
}

void KMainWindow::contextMenuEvent(QContextMenuEvent* event)
{
    m_pContextMenu->exec(event->globalPos());
}

void KMainWindow::initContextMenu()
{
    m_pContextMenu = new QMenu(this);

    // �ָ�
    QAction* resumeSelectAll = new QAction(this);
    resumeSelectAll->setText(u8"�ָ�");
    resumeSelectAll->setIcon(QIcon(":/icons/normal/play.png"));
    this->addAction(resumeSelectAll);
    m_pContextMenu->addAction(resumeSelectAll);
    (void)connect(resumeSelectAll, &QAction::triggered, this, &KMainWindow::onResumeClicked);

    // ��ͣ
    QAction* stopAction = new QAction(this);
    stopAction->setText(u8"��ͣ");
    stopAction->setIcon(QIcon(":/icons/normal/pause.png"));
    this->addAction(stopAction);
    m_pContextMenu->addAction(stopAction);
    (void)connect(stopAction, &QAction::triggered, this, &KMainWindow::onStopClicked);

    m_pContextMenu->addSeparator();

    // ȫ����ͣ
    QAction* stopAllAction = new QAction(this);
    stopAllAction->setText(u8"ȫ����ͣ");
    stopAllAction->setIcon(QIcon(":/icons/normal/pauseall.png"));
    this->addAction(stopAllAction);
    m_pContextMenu->addAction(stopAllAction);
    (void)connect(stopAllAction, &QAction::triggered, this, &KMainWindow::onStopAllClicked);

    // ȫ���ָ�
    QAction* resumeAllAction = new QAction(this);
    resumeAllAction->setText(u8"ȫ���ָ�");
    resumeAllAction->setIcon(QIcon(":/icons/normal/playall.png"));
    this->addAction(resumeAllAction);
    m_pContextMenu->addAction(resumeAllAction);
    (void)connect(resumeAllAction, &QAction::triggered, this, &KMainWindow::onResumeAllClicked);

    m_pContextMenu->addSeparator();

    // �༭
    QAction* editAction = new QAction(this);
    editAction->setText(u8"�༭");
    editAction->setIcon(QIcon(":/icons/normal/edit.png"));
    this->addAction(editAction);
    m_pContextMenu->addAction(editAction);
    (void)connect(editAction, &QAction::triggered, this, &KMainWindow::onEditFileButtonClicked);

    // ��
    QAction* openFileAction = new QAction(this);
    openFileAction->setText(u8"���ļ�");
    openFileAction->setIcon(QIcon(":/icons/normal/open.png"));
    this->addAction(openFileAction);
    m_pContextMenu->addAction(openFileAction);
    (void)connect(openFileAction, &QAction::triggered, this, &KMainWindow::onOpenFileButtonClicked);

    m_pContextMenu->addSeparator();

    // �Ƴ�
    QAction* removeAction = new QAction(this);
    removeAction->setText(u8"�Ƴ�");
    removeAction->setIcon(QIcon(":/icons/normal/move.png"));
    this->addAction(removeAction);
    m_pContextMenu->addAction(removeAction);
    (void)connect(removeAction, &QAction::triggered, this, &KMainWindow::onRemoveFileButtonClicked);

    // ȫ���Ƴ�
    QAction* removeAllAction = new QAction(this);
    removeAllAction->setText(u8"ȫ���Ƴ�");
    removeAllAction->setIcon(QIcon(":/icons/normal/moveall.png"));
    this->addAction(removeAllAction);
    m_pContextMenu->addAction(removeAllAction);
    (void)connect(removeAllAction, &QAction::triggered, this, &KMainWindow::onRemoveAllFileButtonClicked);

    m_pContextMenu->addSeparator();

    // ������־
    QAction* saveLogAction = new QAction(this);
    saveLogAction->setText(u8"������־");
    saveLogAction->setIcon(QIcon(":/icons/normal/right.png"));
    this->addAction(saveLogAction);
    m_pContextMenu->addAction(saveLogAction);
    (void)connect(saveLogAction, &QAction::triggered, this, &KMainWindow::onSaveLogButtonClicked);

    // �����־
    QAction* clearAction = new QAction(this);
    clearAction->setText(u8"�����־");
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
        QMessageBox::warning(this, tr(u8"��ʾ"), tr(u8"����ѡ��һ��!"));
        return;
    }

    QTableWidgetItem* item = selectedItems.first(); // ��ȡ��һ��ѡ�е���
    int row = item->row();
    QString filePath = m_pCentralWidget->m_pTableView1->item(row, 0)->text(); 

    if (!filePath.isEmpty())
    {
        QUrl fileUrl = QUrl::fromLocalFile(filePath);
        if (!QDesktopServices::openUrl(fileUrl))
        {
            QMessageBox::warning(this, tr(u8"����"), tr(u8"�޷����ļ�: %1").arg(filePath));
        }
    }
    else
    {
        QMessageBox::warning(this, tr(u8"����"), tr(u8"ѡ�е���û���ļ�·��!"));
    }
}

void KMainWindow::closeEvent(QCloseEvent* event)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(u8"ȷ���˳�");
    msgBox.setText(u8"��ȷ��Ҫ�˳������𣿡�Yes��ȷ���˳�����No����С��������");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    QCheckBox* deleteHistoryCheckBox = new QCheckBox(u8"ɾ����ʷ��¼", &msgBox);
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
        qDebug() << u8"�������ݿ�ʧ��." << query.lastError();
    }
    else
    {
        qDebug() << u8"���ݿ�����ɹ�";
    }

    if (!query.exec("DELETE FROM UserOperation"))
    {
        qDebug() << u8"���� userOperation ��ʧ��:" << query.lastError();
    }
    else
    {
        qDebug() << u8"userOperation ������ɹ�";
    }
}
