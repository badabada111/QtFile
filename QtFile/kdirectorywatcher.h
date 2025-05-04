#ifndef FILEWATCHER_H  
#define FILEWATCHER_H  

#include <windows.h>  
#include <QString>  
#include <QThread>  
#include <QDateTime>
#include <QWaitCondition>  
#include <QMutex> 
#include <QSet>
#include <QFileInfo>
#include <QMap>


class KDirectoryWatcher : public QThread {
    Q_OBJECT

public:
    KDirectoryWatcher(const QString& directory);
    void run() override;

    void startWatching();
    void stopWatching();
    void setExtensionsToWatch(const QStringList& extensions);
    void setExtensionsToIgnore(const QStringList& extensions);
    void setFilesToWatch(const QStringList& files);
    void setFilesToIgnore(const QStringList& files);
    QStringList getExtensionsToWatch();
    QStringList getExtensionsToIgnore();
    QStringList getFilesToWatch();
    QStringList getFilesToIgnore();
    void clearExtensionsToWatch();
    void clearExtensionsToIgnore();
    void clearFilesToWatch();
    void clearFilesToIgnore();
    void updateDirectory(const QString& newDirectory);
    void setAllowedActions(const QSet<int>& actions);
    QSet<int> KDirectoryWatcher::getAllowedActions() const;
    void setWatchSubdirectories(bool watchSubdirs);
    bool isWatchSubdirectories() const;
    bool isFileModified(const QFileInfo& fileInfo);

signals:
    void fileChanged(const QString& fileName, const QString& changeType);

private:
    QString m_directory;
    HANDLE m_hDir;
    bool m_running;
    bool isRenaming;
    bool m_stopWatching; // 新增停止标志
    bool m_watchSubdirectories = true;
    QWaitCondition m_condition;
    QMutex m_mutex;
    QStringList m_extensionsToWatch;
    QStringList m_extensionsToIgnore;
    QStringList m_filesToWatch;
    QStringList m_filesToIgnore;
    QSet<int> m_allowedActions;
    
};

#endif // FILEWATCHER_H