#pragma once

#include <Data.h>

#include <QSqlDatabase>

#include <deque>

class Database
{
public:
    Database();
    ~Database();

    void insertRecordToMessages(const Data& data) const;
    void insertRecordToConfiguration(const Config& config) const;
    void retrieveAndDisplayRecords() const;
    void countAndDisplayRecords() const;
    std::deque<Data> getLatestNRecords(int number);
    std::optional<Config> getConfiguration();

private:
    QSqlDatabase openDatabase() const;
    bool createMessagesTable() const;
    bool createConfigurationTable() const;
    bool clearMessagesTable() const;

    QSqlDatabase db;
};
