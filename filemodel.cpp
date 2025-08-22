#include "filemodel.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <algorithm>
#include <stack>

FileModel::FileModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int FileModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_files.size();
}

void FileModel::loadDirectory(const QString& path)
{
    beginResetModel();
    m_files.clear();

    m_currentDirectory = path;
    emit currentDirectoryChanged();

    try {
        std::stack<std::pair<std::filesystem::path, QString>> directories;
        QDir rootDir(path);

        directories.push({path.toStdString(), ""});

        while (!directories.empty()) {
            auto [current_dir, relative_path] = directories.top();
            directories.pop();

            for (const auto& entry : std::filesystem::directory_iterator(current_dir)) {
                QFileInfo qFileInfo(QString::fromStdString(entry.path().string()));

                if (qFileInfo.fileName() == "." || qFileInfo.fileName() == "..") {
                    continue;
                }

                if (qFileInfo.isDir()) {
                    QString dirRelativePath = relative_path + (relative_path.isEmpty() ? "" : "/") + qFileInfo.fileName();
                    directories.push({entry.path(), dirRelativePath});
                    continue;
                }

                FileInfo info;
                info.name = qFileInfo.fileName();
                info.path = qFileInfo.filePath();
                info.size = qFileInfo.size();
                info.extension = qFileInfo.suffix().toLower();

                info.relativePath = relative_path;

                if (info.extension.isEmpty()) {
                    info.type = "File";
                } else {
                    info.type = info.extension.toUpper() + " File";
                }

                m_files.append(info);
            }
        }
    } catch (const std::exception& e) {
        qWarning() << "Error reading directory:" << e.what();
    }

    endResetModel();
}

void FileModel::sortByType()
{
    beginResetModel();

    std::sort(m_files.begin(), m_files.end(), [](const FileInfo& a, const FileInfo& b) {
        if (a.extension != b.extension) {
            return a.extension < b.extension;
        }
        return a.name.toLower() < b.name.toLower();
    });

    endResetModel();
}

void FileModel::sortByName()
{
    beginResetModel();

    std::sort(m_files.begin(), m_files.end(), [](const FileInfo& a, const FileInfo& b) {
        return a.name.toLower() < b.name.toLower();
    });

    endResetModel();
}

void FileModel::sortBySize()
{
    beginResetModel();

    std::sort(m_files.begin(), m_files.end(), [](const FileInfo& a, const FileInfo& b) {
        return a.size < b.size;
    });

    endResetModel();
}


QString FileModel::currentDirectory() const
{
    return m_currentDirectory;
}

void FileModel::setCurrentDirectory(const QString& directory)
{
    if (m_currentDirectory != directory) {
        m_currentDirectory = directory;
        loadDirectory(directory);
        emit currentDirectoryChanged();
    }
}

QVariant FileModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_files.size())
        return QVariant();

    const FileInfo &file = m_files.at(index.row());

    switch (role) {
    case NameRole: return file.name;
    case PathRole: return file.path;
    case RelativePathRole: return file.relativePath;
    case SizeRole: return file.size;
    case TypeRole: return file.type;
    case ExtensionRole: return file.extension;
    case DisplayNameRole:
        // Динамически выбираем что показывать
        return m_showFullPath ? file.path : file.name;
    default: return QVariant();
    }
}

QHash<int, QByteArray> FileModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {PathRole, "path"},
        {RelativePathRole, "relativePath"},
        {SizeRole, "size"},
        {TypeRole, "type"},
        {ExtensionRole, "extension"},
        {DisplayNameRole, "displayName"}  // Новая роль
    };
}

bool FileModel::showFullPath() const
{
    return m_showFullPath;
}

void FileModel::setShowFullPath(bool show)
{
    if (m_showFullPath != show) {
        m_showFullPath = show;
        emit showFullPathChanged();
        // Обновляем данные для отображения
        emit dataChanged(index(0), index(rowCount() - 1), {DisplayNameRole});
    }
}
