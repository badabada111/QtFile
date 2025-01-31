#include "kcustomsortfilterproxymodel.h"
#include <QCollator>

KCustomSortFilterProxyModel::KCustomSortFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent), m_filterText(QString())
{
    
}

void KCustomSortFilterProxyModel::setFilterText(const QString& text)
{
    m_filterText = text;
    invalidateFilter(); 
}

void KCustomSortFilterProxyModel::setDateTimeRange(const QDateTime& begin, const QDateTime& end)
{
    m_dateTimeBegin = begin;
    m_dateTimeEnd = end;
    invalidateFilter(); 
}

bool KCustomSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    bool withinDateTimeRange = true;
    bool matchesFilterText = true;

    // 检查时间段
    if (m_dateTimeBegin.isValid() && m_dateTimeEnd.isValid())
    {
        QModelIndex timeIndex = sourceModel()->index(sourceRow, 1, sourceParent); 
        QString timeStr = sourceModel()->data(timeIndex).toString();
        QDateTime timestamp = QDateTime::fromString(timeStr, "yyyy-MM-dd HH:mm:ss");

        withinDateTimeRange = timestamp.isValid() && timestamp >= m_dateTimeBegin && timestamp <= m_dateTimeEnd;
    }

    // 检查关键字
    if (!m_filterText.isEmpty())
    {
        matchesFilterText = false;
        for (int column = 0; column < sourceModel()->columnCount(); ++column)
        {
            QModelIndex index = sourceModel()->index(sourceRow, column, sourceParent);
            QString data = sourceModel()->data(index).toString();
            if (data.contains(m_filterText, Qt::CaseInsensitive))
            {
                matchesFilterText = true;
                break;
            }
        }
    }

    return withinDateTimeRange && matchesFilterText;
}

void KCustomSortFilterProxyModel::clearFilters()
{
    m_filterText.clear();
    m_dateTimeBegin = QDateTime();
    m_dateTimeEnd = QDateTime();
    invalidateFilter(); 
}

bool KCustomSortFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const 
{
    QString leftData = sourceModel()->data(left).toString();
    QString rightData = sourceModel()->data(right).toString();

    // 按照字符串的长度排序，按汉字的拼音排序等
    return QString::localeAwareCompare(leftData, rightData) < 0;
}
