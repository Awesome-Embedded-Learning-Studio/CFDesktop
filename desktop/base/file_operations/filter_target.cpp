#include "filter_target.h"
#include <QDir>
#include <QFileInfo>
#include <QMap>

namespace cf::desktop::base::filesystem {

namespace {

class FilterPool {
  public:
    static FilterPool& instance() {
        static FilterPool pool;
        return pool;
    }

    QString filter_string(FilterType type) const { return m_filterStrings.value(type); }

    QStringList& filter_list(FilterType type) { return m_filterLists[type]; }

  private:
    FilterPool() {
        // Pictures
        m_filterLists[FilterType::Pictures] = {"*.png",  "*.jpg", "*.jpeg", "*.bmp", "*.gif",
                                               "*.webp", "*.svg", "*.tiff", "*.ico"};
        m_filterStrings[FilterType::Pictures] =
            QString("Images (%1)").arg(m_filterLists[FilterType::Pictures].join(' '));

        // AllFiles (files only)
        m_filterLists[FilterType::AllFiles] = {"*"};
        m_filterStrings[FilterType::AllFiles] = QString("All Files (*)");

        // All (files + directories)
        m_filterLists[FilterType::All] = {"*"};
        m_filterStrings[FilterType::All] = QString("All (*)");

        // Dirent
        m_filterLists[FilterType::Dirent] = {};
        m_filterStrings[FilterType::Dirent] = QString("Directories");
    }

    QMap<FilterType, QStringList> m_filterLists;
    QMap<FilterType, QString> m_filterStrings;
};

} // namespace

QString request_filter(const FilterType what) {
    return FilterPool::instance().filter_string(what);
}

QStringList& request_filterlist(const FilterType what) {
    return FilterPool::instance().filter_list(what);
}

QStringList filter_target(const QString& dirent_path, const QStringList& filters) {
    QStringList result;
    QDir dir(dirent_path);

    if (!dir.exists()) {
        return result;
    }

    const QFileInfoList entries = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);
    for (const auto& entry : entries) {
        result.append(entry.absoluteFilePath());
    }

    return result;
}

} // namespace cf::desktop::base::filesystem
