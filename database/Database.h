#pragma once

#include <Data.h>

#include <QSqlDatabase>

#include <deque>

class Database
{
public:
    explicit Database(const std::string& path);
    ~Database();

    void insertRecordToMessages(const Data& data) const;
    void insertRecordToConfiguration(const Config& config) const;
    void retrieveAndDisplayRecords() const;
    void countAndDisplayRecords() const;
    std::deque<Data> getLatestNRecords(int number);
    std::optional<Config> getConfiguration();

private:
    [[nodiscard]] QSqlDatabase openDatabase(const std::string& path) const;
    [[nodiscard]] bool createMessagesTable() const;
    [[nodiscard]] bool createConfigurationTable() const;
    [[nodiscard]] bool clearMessagesTable() const;

    QSqlDatabase db;
};
