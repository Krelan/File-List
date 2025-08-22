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
    case IsDirRole: return file.isDirectory;
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
        {IsDirRole, "isDirectory"}
    };
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

                FileInfo info;
                info.name = qFileInfo.fileName();
                info.path = qFileInfo.filePath();
                info.size = qFileInfo.size();
                info.isDirectory = qFileInfo.isDir();
                info.extension = qFileInfo.suffix().toLower();

                info.relativePath = relative_path + (relative_path.isEmpty() ? "" : "/") + info.name;

                if (info.isDirectory) {
                    info.type = "Directory";
                    directories.push({entry.path(), info.relativePath});
                } else if (info.extension.isEmpty()) {
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
        if (a.isDirectory != b.isDirectory) {
            return a.isDirectory > b.isDirectory;
        }
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
        if (a.isDirectory != b.isDirectory) {
            return a.isDirectory > b.isDirectory;
        }
        return a.name.toLower() < b.name.toLower();
    });

    endResetModel();
}

void FileModel::sortBySize()
{
    beginResetModel();

    std::sort(m_files.begin(), m_files.end(), [](const FileInfo& a, const FileInfo& b) {
        if (a.isDirectory != b.isDirectory) {
            return a.isDirectory > b.isDirectory;
        }
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
