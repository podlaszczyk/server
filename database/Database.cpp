#include "Database.h"

#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>

Database::Database()
{
    db = openDatabase();
    if (!db.isValid()) {
        return;
    }

    if (!createMessagesTable()) {
        db.close();
        return;
    }

    if (!createConfigurationTable()) {
        db.close();
        return;
    }

    if (!clearMessagesTable()) {
        db.close();
        return;
    }
}

Database::~Database()
{
    db.close();
}

QSqlDatabase Database::openDatabase() const
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database.db");

    if (!db.open()) {
        qDebug() << "Error: Failed to open database";
    }

    return db;
}

bool Database::createMessagesTable() const
{
    QSqlQuery createTableQuery(db);
    const QString createTableSql = R"(
        CREATE TABLE IF NOT EXISTS messages (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            pressure REAL,
            temperature REAL,
            velocity REAL,
            timestamp REAL
        )
    )";

    createTableQuery.prepare(createTableSql);

    if (!createTableQuery.exec()) {
        qDebug() << "Error: Failed to create 'messages' table";
        qDebug() << createTableQuery.lastError().text();
        return false;
    }

    qDebug() << "'messages' table created successfully";
    return true;
}

bool Database::createConfigurationTable() const
{
    QSqlQuery createTableQuery(db);
    const QString createTableSql = R"(
        CREATE TABLE IF NOT EXISTS configuration (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            frequency INTEGER,
            debug INTEGER
        )
    )";
    createTableQuery.prepare(createTableSql);

    if (!createTableQuery.exec()) {
        qDebug() << "Error: Failed to create 'configuration' table";
        qDebug() << createTableQuery.lastError().text();
        return false;
    }

    qDebug() << "'configuration' table created successfully";
    return true;
}

bool Database::clearMessagesTable() const
{
    QSqlQuery clearQuery("DELETE FROM messages", db);

    if (!clearQuery.exec()) {
        qDebug() << "Error: Failed to clear data from 'messages' table";
        qDebug() << clearQuery.lastError().text();
        return false;
    }

    qDebug() << "All data cleared from 'messages' table";
    return true;
}

void Database::insertRecordToMessages(const Data& data) const
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    qint64 millisecondsSinceEpoch = currentDateTime.toMSecsSinceEpoch();

    double currentTime = static_cast<double>(millisecondsSinceEpoch) / 1000.0;

    auto timestamp = QString::number(currentTime, 'f', 4).toDouble();

    QSqlQuery insertQuery(db);
    const QString insertQuerySql = R"(
        INSERT INTO messages (pressure, temperature, velocity, timestamp)
        VALUES (:pressure, :temperature, :velocity, :timestamp)
    )";

    insertQuery.prepare(insertQuerySql);

    insertQuery.bindValue(":pressure", data.pressure);
    insertQuery.bindValue(":temperature", data.temperature);
    insertQuery.bindValue(":velocity", data.velocity);
    insertQuery.bindValue(":timestamp", timestamp);

    if (!insertQuery.exec()) {
        qDebug() << "Error: Failed to insert record";
        qDebug() << insertQuery.lastError().text();
    }
    else {
        qDebug() << "Record inserted successfully";
    }
}

void Database::retrieveAndDisplayRecords() const
{
    QSqlQuery selectQuery("SELECT pressure, temperature, velocity, timestamp FROM messages", db);

    if (!selectQuery.exec()) {
        qDebug() << "Error: Failed to execute SELECT query";
        qDebug() << selectQuery.lastError().text();
        return;
    }

    while (selectQuery.next()) {
        double pressure = selectQuery.value(0).toDouble();
        double temperature = selectQuery.value(1).toDouble();
        double velocity = selectQuery.value(2).toDouble();
        double timestamp = selectQuery.value(3).toDouble();

        qDebug() << "Record:";
        qDebug() << "  Pressure:" << QString::number(pressure, 'f', 1).toDouble();
        qDebug() << "  Temperature:" << QString::number(temperature, 'f', 1).toDouble();
        qDebug() << "  Velocity:" << QString::number(velocity, 'f', 1).toDouble();
        qDebug() << qSetRealNumberPrecision(15) << "  Timestamp:" << QString::number(timestamp, 'f', 4).toDouble();
    }
}

void Database::countAndDisplayRecords() const
{
    QSqlQuery countQuery("SELECT COUNT(*) FROM messages", db);

    if (!countQuery.exec() || !countQuery.first()) {
        qDebug() << "Error: Failed to execute COUNT query";
        qDebug() << countQuery.lastError().text();
        return;
    }

    int recordCount = countQuery.value(0).toInt();
    qDebug() << "Number of records in 'messages' table:" << recordCount;
}

std::deque<Data> Database::getLatestNRecords(int number)
{
    QSqlQuery query(db);
    query.prepare(
        "SELECT pressure, temperature, velocity, timestamp FROM "
        "messages ORDER BY id DESC LIMIT "
        + QString::number(number));

    // Execute the query
    if (!query.exec()) {
        qDebug() << "Error: Failed to execute query";
        qDebug() << query.lastError().text();
        db.close();
    }
    std::deque<Data> container;
    qDebug() << "Last N added values in 'messages' table:";
    while (query.next()) {
        double pressure = query.value(0).toDouble();
        double temperature = query.value(1).toDouble();
        double velocity = query.value(2).toDouble();
        double timestamp = query.value(3).toDouble();

        qDebug() << "Pressure:" << pressure << ", Temperature:" << temperature << ", Velocity:" << velocity
                 << ", Timestamp:" << timestamp;

        Data data{.pressure = pressure, .temperature = temperature, .velocity = velocity};

        container.push_back(data);
    }
    return container;
}

void Database::insertRecordToConfiguration(const Config& config) const
{
    QSqlQuery query(db);
    const QString insertQuerySql = R"(
        INSERT OR REPLACE INTO configuration (id, frequency, debug)
        VALUES (1, :frequency, :debug)
    )";
    query.prepare(insertQuerySql);

    query.bindValue(":frequency", config.frequency);
    query.bindValue(":debug", config.debug ? 1 : 0);

    if (!query.exec()) {
        qDebug() << "Error: Failed to insert or replace record in 'configuration' table";
        qDebug() << query.lastError().text();
        return;
    }

    qDebug() << "Record inserted or replaced successfully";
}

std::optional<Config> Database::getConfiguration()
{
    QSqlQuery query(db);
    query.prepare("SELECT frequency, debug FROM configuration WHERE id = 1");

    if (!query.exec()) {
        qDebug() << "Error: Failed to execute query";
        qDebug() << query.lastError().text();
        return {};
    }

    if (query.next()) {
        int frequency = query.value(0).toInt();
        bool debug = query.value(1).toBool();

        qDebug() << "Configuration values:";
        qDebug() << "Frequency:" << frequency;
        qDebug() << "Debug:" << debug;

        Config config{.frequency = frequency, .debug = debug};
        return config;
    }
    qDebug() << "Error: No configuration record found with id = 1";

    return {};
}