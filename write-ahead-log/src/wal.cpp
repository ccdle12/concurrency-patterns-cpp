#include "wal/wal.h"

using namespace wal;

void WriteAheadLog::assert_file_open() const
{
    if (!m_log_file.is_open())
        throw std::runtime_error("Failed to open log file");
}

WriteAheadLog::WriteAheadLog(const std::string& file)
{
    m_log_file.open(file, std::ios::binary | std::ios::in | std::ios::out | std::ios::app);
    assert_file_open();
}

void WriteAheadLog::Write(const Entry& entry)
{ 
    assert_file_open();
    m_log_file << entry;
}

std::vector<Entry> WriteAheadLog::Read() 
{
    assert_file_open();

    m_log_file.seekg(0, std::ios::end);
    auto end = m_log_file.tellg();

    m_log_file.seekg(0);

    std::vector<Entry> entries;
    for (int i = 0; i < end; i = m_log_file.tellg())
    {
        Entry entry;
        m_log_file >> entry;

        entries.push_back(entry);
    }

    return entries;
}
