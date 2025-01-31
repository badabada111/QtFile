#include "KDirectoryWatcher.h"  
#include <QDebug>  
#include <QFileInfo>  
#include <QFile>  
#include <QThread> 
#include <QMutexLocker>
#include <QDebug>

KDirectoryWatcher::KDirectoryWatcher(const QString& directory)
    : m_directory(directory)
    , m_running(false)
{
}

void KDirectoryWatcher::run()
{
    // 打开目录句柄  
    std::wstring wStr = m_directory.toStdWString();

    m_hDir = CreateFileW(wStr.c_str(), FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr);

    if (m_hDir == INVALID_HANDLE_VALUE) {
        qDebug() << "Failed to open directory handle";
        return;
    }

    m_running = true;
    char buffer[1024];
    DWORD bytesReturned;

    while (m_running)
    {
        {
            QMutexLocker locker(&m_mutex);
            while (m_stopWatching) 
            {
                m_condition.wait(&m_mutex); 
            }
        } 
        // 监控目录  
        if (ReadDirectoryChangesW(
            m_hDir,
            &buffer,
            sizeof(buffer),
            TRUE, // 监控子目录  
            //m_watchSubdirectories,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE |
            FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SECURITY,
            &bytesReturned,
            nullptr,
            nullptr
        ))
        {
            qDebug() << "m_watchSubdirectories" << m_watchSubdirectories;
            while (m_stopWatching)
            {
                return;
            }
            FILE_NOTIFY_INFORMATION* pNotify;
            int offset = 0;

            do {
                pNotify = (FILE_NOTIFY_INFORMATION*)(&buffer[offset]);
                QString fileName = QString::fromWCharArray(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));
                QString action;
                switch (pNotify->Action) {
                case FILE_ACTION_ADDED: 
                    action = u8"新增文件";
                    break;
                case FILE_ACTION_REMOVED: 
                    action = u8"删除文件"; 
                    break;
                case FILE_ACTION_MODIFIED: 
                    action = u8"修改文件"; 
                    break;
                case FILE_ACTION_RENAMED_OLD_NAME: 
                    action = u8"重命名（旧）"; 
                    break;
                case FILE_ACTION_RENAMED_NEW_NAME: 
                    action = u8"重命名（新）"; 
                    break;
                   
                default: 
                    action = u8"未知"; 
                    break;
                }

                // 检查文件扩展名  
                QFileInfo fileInfo(m_directory + "\\" + fileName);//获取文件相关信息
                QString fileExtension = fileInfo.suffix();//获取选中文件拓展名
                QString fullPath = fileInfo.absoluteFilePath();//获取选中文件的相对路径

                bool isInSubdirectory = fileInfo.absolutePath() != m_directory;

                if (isInSubdirectory && !m_watchSubdirectories) 
                {
                    offset += pNotify->NextEntryOffset;
                    continue;
                }
                bool shouldWatch = false;

                if (m_extensionsToWatch.isEmpty() || m_extensionsToWatch.contains(fileExtension, Qt::CaseInsensitive)) 
                {
                    if (!m_extensionsToIgnore.contains(fileExtension, Qt::CaseInsensitive)) 
                    {
                        shouldWatch = true;
                    }
                }
                qDebug() << "shouldWatch" << shouldWatch;

                if (m_filesToWatch.contains(fullPath, Qt::CaseInsensitive)) 
                {
                    shouldWatch = true;
                }

                else if (m_filesToIgnore.contains(fullPath, Qt::CaseInsensitive)) 
                {
                    shouldWatch = false;
                }
                qDebug() << "shouldWatch" << shouldWatch;

                if (!m_filesToWatch.isEmpty() && !m_filesToWatch.contains(fullPath, Qt::CaseInsensitive)) 
                {
                    shouldWatch = false;
                }
                qDebug() << "shouldWatch" << shouldWatch;
                if (shouldWatch) 
                {
                    if (m_allowedActions.contains(pNotify->Action))
                    {
                        qDebug() << "Emitting fileChanged for:" << fullPath << "Action:" << action;
                        emit fileChanged(fullPath, action);
                    }

                }

                offset += pNotify->NextEntryOffset;
            } while (pNotify->NextEntryOffset != 0);
        }
    }

    // 关闭句柄  
    CloseHandle(m_hDir);
}

void KDirectoryWatcher::startWatching() 
{
    start();
    m_stopWatching = false;
}

void KDirectoryWatcher::stopWatching() 
{

    m_stopWatching = true; 
    m_running = false;
    m_condition.wakeAll();
    quit(); 
}

QSet<int> KDirectoryWatcher::getAllowedActions() const
{
    return m_allowedActions;
}

void KDirectoryWatcher::setWatchSubdirectories(bool watchSubdirs)
{
    m_watchSubdirectories = watchSubdirs;

}

bool KDirectoryWatcher::isWatchSubdirectories() const
{
    return m_watchSubdirectories;
}

void KDirectoryWatcher::setExtensionsToWatch(const QStringList& extensions) 
{
    m_extensionsToWatch = extensions;
}

void KDirectoryWatcher::setExtensionsToIgnore(const QStringList& extensions) 
{
    m_extensionsToIgnore = extensions;
}

void KDirectoryWatcher::setFilesToWatch(const QStringList& files)
{
    m_filesToWatch = files;
}

void KDirectoryWatcher::setFilesToIgnore(const QStringList& files)
{
    m_filesToIgnore = files;
}


QStringList KDirectoryWatcher::getExtensionsToWatch()
{
    return m_extensionsToWatch;
}

QStringList KDirectoryWatcher::getExtensionsToIgnore()
{
    return m_extensionsToIgnore;
}

QStringList KDirectoryWatcher::getFilesToWatch()
{
    return m_filesToWatch;
}

QStringList KDirectoryWatcher::getFilesToIgnore()
{
    return m_filesToIgnore;
}

void KDirectoryWatcher::clearExtensionsToWatch()
{
    m_extensionsToWatch.clear();
}

void KDirectoryWatcher::clearExtensionsToIgnore()
{
    m_extensionsToIgnore.clear();
}

void KDirectoryWatcher::clearFilesToWatch()
{
    m_filesToWatch.clear();
}

void KDirectoryWatcher::clearFilesToIgnore()
{
    m_filesToIgnore.clear();
}

void KDirectoryWatcher::setAllowedActions(const QSet<int>& actions)
{
    m_allowedActions = actions;
}

void KDirectoryWatcher::updateDirectory(const QString& newDirectory)
{
    QMutexLocker locker(&m_mutex); 
    m_directory = newDirectory;

    if (m_running) 
    {
        startWatching();
    }
}