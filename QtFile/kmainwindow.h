#ifndef __K_MAINWINDOW_H_
#define __K_MAINWINDOW_H_

#include <QtWidgets/QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "ui_kmainwindow.h"
#include "ktoolbar.h"
#include "kcentralwidget.h"
#include "kmenubar.h"
#include <kcentralwidget.h>
#include <kselectedbar.h>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QLineEdit>
#include <QPushButton>
#include <QStatusBar>
#include <QSystemTrayIcon>
#include <QMap>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QTimer>
#include <QQueue>
#include <QListWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>

class KMainWindow : public QWidget
{
    Q_OBJECT

public:
    KMainWindow(QWidget *parent = Q_NULLPTR);
    void initWindows();
    void onAddFileButtonClicked();
    void onOptionFileButtonClicked();
    void onEditFileButtonClicked();
    void onDirectoryChanged(const QString& directory, const QString& action);
    void onStopClicked();
    void onResumeClicked();
    void onStopAllClicked();
    void onResumeAllClicked();
    void onRemoveFileButtonClicked();
    void onRemoveAllFileButtonClicked();
    void onSaveLogButtonClicked();
    void updateStatusBar();
    void initContextMenu();
    const QList<QString>& getWatchedFiles() const;
    QList<QString> watchedFiles;

protected:
    virtual void contextMenuEvent(QContextMenuEvent* event) override;
    virtual void closeEvent(QCloseEvent* event) override;
    virtual void dragEnterEvent(QDragEnterEvent* event) override;
    virtual void dropEvent(QDropEvent* event) override;

private slots:
    void createAction();
    void clearLog();

private:
    Ui::KMainWindowClass ui;
    QVBoxLayout *m_pVLayout;
    QHBoxLayout *m_pHLayout;
    KMenuBar* m_pMenuBar;
    KToolBar* m_pToolBar;
    KCentralWidget* m_pCentralWidget;
    QStatusBar* m_pStatusBar;
    KDirectoryWatcher* m_pDirectoryWatcher;
    QMap<QString, KDirectoryWatcher*> m_directoryWatchers;
    QString m_Directory;
    QMenu* m_pContextMenu;
    QSystemTrayIcon* m_SystemTray;//系统托盘
    void init_systemTrayIcon();//系统托盘初始化
    QTimer* m_pTimer; // 新增的 QTimer 对象
    QQueue<QString> m_fileChangeQueue; // 用于存储文件变化信息的队列
    QListWidget* m_pSubDirListWidget;  // 用于显示子目录
    QString m_previousFilter; // 保存之前的过滤条件
    QString m_previousTwoFilter; // 保存之前的过滤条件
    void systemTrayIcon_activated(QSystemTrayIcon::ActivationReason reason);//响应系统托盘的动作（双击等）
    void quitMusicPlayer();
    void onTimerTimeout();
    void onDirectoryTextChanged(const QString& directory);
    void onSubDirItemClicked(QListWidgetItem* item);
    void onSearchButtonClicked();
    void onResumeSearchClicked();
    void onInquireButtonClicked();
    void onInquireResumeButtonClicked();
    void onSearchButtonTwoClicked();
    void onResumeSearchTwoClicked();
    void onInquireButtonTwoClicked();
    void onInquireResumeButtonTwoClicked();
    void clearDatabase();
    void exportLogs(const QString& filePath);
    void addUserOperationLog(const QString& operation, const QString& time, const QString& details);
    void onCheckboxStateChanged(int row, int col);
    void onImportButtonClicked();
    void onExportButtonClicked();
    void importData(const QString& filePath);
    void exportData(const QString& filePath);
    void onOpenFileButtonClicked();
    void addWatchDirectory(const QString& directory);

};

#endif
