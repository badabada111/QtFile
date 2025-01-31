#include "kselectedbar.h"

CustomDialog::CustomDialog(QWidget* parent)
    : QDialog(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout;

    // Watch extensions
    QHBoxLayout* watchLayout = new QHBoxLayout;
    QLabel* watchLabel = new QLabel(tr(u8"��ע�ĺ�׺:"));
    m_pWatchExtensionsLineEdit = new QLineEdit;
    m_pWatchExtensionsLineEdit->setPlaceholderText(tr(u8"������Ҫ��ע�ĺ�׺���ö��ŷָ�����txt,h")); 
    watchLayout->addWidget(watchLabel);
    watchLayout->addWidget(m_pWatchExtensionsLineEdit);
    mainLayout->addLayout(watchLayout);

    // Ignore extensions
    QHBoxLayout* ignoreLayout = new QHBoxLayout; 
    QLabel* ignoreLabel = new QLabel(tr(u8"���Եĺ�׺:"));
    m_pIgnoreExtensionsLineEdit = new QLineEdit;
    m_pIgnoreExtensionsLineEdit->setPlaceholderText(tr(u8"������Ҫ���Եĺ�׺���ö��ŷָ�����txt,h"));
    ignoreLayout->addWidget(ignoreLabel);
    ignoreLayout->addWidget(m_pIgnoreExtensionsLineEdit);
    mainLayout->addLayout(ignoreLayout);

    // Watch files
    QHBoxLayout* watchFileLayout = new QHBoxLayout;
    QLabel* watchFileLabel = new QLabel(tr(u8"ָ���ļ����:"));
    m_pWatchExtensionsFileLineEdit = new QLineEdit;
    m_pWatchFileButton = new QPushButton(tr(u8"���ָ���ļ�"));
    watchFileLayout->addWidget(watchFileLabel);
    watchFileLayout->addWidget(m_pWatchExtensionsFileLineEdit);
    watchFileLayout->addWidget(m_pWatchFileButton); // ��Ӱ�ť
    mainLayout->addLayout(watchFileLayout);
    (void)connect(m_pWatchFileButton, &QPushButton::clicked, this, &CustomDialog::openWatchFileButtonClicked);

     // Ignore files
    QHBoxLayout* ignoreFileLayout = new QHBoxLayout;
    QLabel* ignoreFileLabel = new QLabel(tr(u8"����ָ���ļ����:"));
    m_pIgnoreExtensionsFileLineEdit = new QLineEdit;
    m_pIgnoreFileButton = new QPushButton(tr(u8"��Ӻ��Ե�ָ���ļ�"));
    ignoreFileLayout->addWidget(ignoreFileLabel);
    ignoreFileLayout->addWidget(m_pIgnoreExtensionsFileLineEdit);
    ignoreFileLayout->addWidget(m_pIgnoreFileButton); 
    mainLayout->addLayout(ignoreFileLayout);
    (void)connect(m_pIgnoreFileButton, &QPushButton::clicked, this, &CustomDialog::openIgnoreFileButtonClicked);

    // File path
    QHBoxLayout* fileLayout = new QHBoxLayout;
    QLabel* fileLabel = new QLabel(tr(u8"�ļ�·��:"));
    m_pFilePathLineEdit = new QLineEdit;
    fileLayout->addWidget(fileLabel);
    fileLayout->addWidget(m_pFilePathLineEdit);
    mainLayout->addLayout(fileLayout);

    // Checkboxes
    QVBoxLayout* checkBoxLayout = new QVBoxLayout;
    QStringList checkBoxTexts = { u8"��Ŀ¼",u8"����", u8"ɾ��", u8"������", u8"�޸�" };
    for (const QString& text : checkBoxTexts)
    {
        QCheckBox* checkBox = new QCheckBox(text,this);
        checkBox->setChecked(true);
        checkBoxes.append(checkBox);
        checkBoxLayout->addWidget(checkBox);
    }
    mainLayout->addLayout(checkBoxLayout);

    // Open file button
    m_pOpenFileButton = new QPushButton(tr(u8"���ļ�"));
    m_pOpenFileButton->setStyleSheet("background-color: #3498db; color: white;");
    (void)connect(m_pOpenFileButton, &QPushButton::clicked, this, &CustomDialog::openFileButtonClicked);
    mainLayout->addWidget(m_pOpenFileButton);

    // OK and Cancel buttons
    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    QPushButton* okButton = new QPushButton(tr(u8"ȷ��"));
    QPushButton* cancelButton = new QPushButton(tr(u8"ȡ��"));
    (void)connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    (void)connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonsLayout);

    setLayout(mainLayout);
    setWindowTitle(tr(u8"����"));
}

QStringList CustomDialog::getWatchExtensions() const
{
    return m_pWatchExtensionsLineEdit->text().split(',', QString::SkipEmptyParts);
}

QStringList CustomDialog::getIgnoreExtensions() const
{
    return m_pIgnoreExtensionsLineEdit->text().split(',', QString::SkipEmptyParts);
}

QStringList CustomDialog::getWatchFileExtensions() const
{
    return m_pWatchExtensionsFileLineEdit->text().split(',', QString::SkipEmptyParts);
}

QStringList CustomDialog::getIgnoreFileExtensions() const
{
    return m_pIgnoreExtensionsFileLineEdit->text().split(',', QString::SkipEmptyParts);
}

QString CustomDialog::getFilePath() const
{
    return m_pFilePathLineEdit->text();
}

QList<QCheckBox*> CustomDialog::getCheckBoxes() const
{
    return checkBoxes;
}

void CustomDialog::openFileButtonClicked()
{

    QString directory = QFileDialog::getExistingDirectory(
        this,
        u8"ѡ���ļ���",
        QDir::homePath()
    );
    if (!directory.isEmpty())
    {
        if (isFileBeingWatched(directory))
        {
            QMessageBox::warning(this, tr(u8"����"), tr(u8"��Ŀ¼�Ѿ��ڼ����б���"));
            return; 
        }

        m_pFilePathLineEdit->setText(directory);
    }
}

bool CustomDialog::isFileBeingWatched(const QString& filePath) const
{
    KMainWindow* mainWindow = qobject_cast<KMainWindow*>(parent());
    if (mainWindow)
    {
        return mainWindow->getWatchedFiles().contains(filePath);
    }
    return false;
}

void CustomDialog::openWatchFileButtonClicked()
{
    QString defaultDirectory = m_pFilePathLineEdit->text();
    if (defaultDirectory.isEmpty())
    {
        defaultDirectory = QDir::homePath();
    }
    QString selectedFilter;
    QString filename = QFileDialog::getOpenFileName(
        this,
        u8"ѡ���ļ�",
        defaultDirectory,
        "All Files (*)",
        &selectedFilter
    );

    if (!filename.isEmpty())
    {
        if (isFileBeingWatched(filename))
        {
            QMessageBox::warning(this, tr(u8"����"), tr(u8"���ļ��Ѿ��ڼ����б���"));
            return; 
        }

        m_pWatchExtensionsFileLineEdit->setText(filename);
    }
}

void CustomDialog::openIgnoreFileButtonClicked()
{
    QString defaultDirectory = m_pFilePathLineEdit->text();
    if (defaultDirectory.isEmpty())
    {
        defaultDirectory = QDir::homePath();
    }
    QString selectedFilter;
    QString filename = QFileDialog::getOpenFileName(
        this,
        u8"ѡ���ļ�",
        defaultDirectory,
        "All Files (*)",
        &selectedFilter
    );

    if (!filename.isEmpty())
    {
        if (isFileBeingWatched(filename))
        {
            QMessageBox::warning(this, tr(u8"����"), tr(u8"���ļ��Ѿ��ڼ����б���"));
            return; 
        }

        m_pIgnoreExtensionsFileLineEdit->setText(filename);
    }
}

void CustomDialog::setFilePath(const QString& filePath)
{
    m_pFilePathLineEdit->setText(filePath);
}

void CustomDialog::setWatchExtensions(const QStringList& extensions)
{
    m_pWatchExtensionsLineEdit->setText(extensions.join(','));
}

void CustomDialog::setIgnoreExtensions(const QStringList& extensions)
{
    m_pIgnoreExtensionsLineEdit->setText(extensions.join(','));
}

void CustomDialog::setWatchFiles(const QStringList& files)
{
    m_pWatchExtensionsFileLineEdit->setText(files.join(','));
}

void CustomDialog::setIgnoreFiles(const QStringList& files)
{
    m_pIgnoreExtensionsFileLineEdit->setText(files.join(','));
}

bool CustomDialog::isSubdirectoryWatched() const
{
    return checkBoxes[0]->isChecked();
}

void CustomDialog::setSubdirectoryWatched(bool watched)
{
    checkBoxes[0]->setChecked(watched);
}

QSet<int> CustomDialog::getAllowedActions() const
{
    QSet<int> allowedActions;
    if (checkBoxes[1]->isChecked()) allowedActions.insert(FILE_ACTION_ADDED);
    if (checkBoxes[2]->isChecked()) allowedActions.insert(FILE_ACTION_REMOVED);
    if (checkBoxes[3]->isChecked()) allowedActions.insert(FILE_ACTION_RENAMED_OLD_NAME);
    if (checkBoxes[3]->isChecked()) allowedActions.insert(FILE_ACTION_RENAMED_NEW_NAME);
    if (checkBoxes[4]->isChecked()) allowedActions.insert(FILE_ACTION_MODIFIED);
    return allowedActions;
}

void CustomDialog::setDirectoryLineEditReadOnly(bool readOnly)
{
    m_pFilePathLineEdit->setReadOnly(readOnly);
}