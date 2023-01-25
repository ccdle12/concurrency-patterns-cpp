#ifndef WAL_H
#define WAL_H

// https://martinfowler.com/articles/patterns-of-distributed-systems/wal.html
#include <fstream>
#include <iostream>
#include <vector>

namespace wal {
class Entry
{
  public:
    uint64_t m_index;
    std::string m_data;
    uint64_t m_timestamp;

    Entry() = default;
    explicit Entry(uint64_t index, std::string data, uint64_t timestamp) :
        m_index{index}, m_data{data}, m_timestamp{timestamp} {};

    friend std::ostream& operator<<(std::ostream& out, const Entry& entry)
    {
        out.write(reinterpret_cast<const char*>(&entry.m_index), sizeof(entry.m_index));

        size_t len = entry.m_data.size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(entry.m_data.c_str(), len);

        out.write(reinterpret_cast<const char*>(&entry.m_timestamp), sizeof(entry.m_timestamp));

        return out;
    }

    friend std::istream& operator>>(std::istream& in, Entry& entry)
    {
        in.read(reinterpret_cast<char*>(&entry.m_index), sizeof(entry.m_index));

        size_t len;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));

        std::vector<char> buf;
        buf.reserve(len);
        in.read(&buf[0], len);
        entry.m_data = std::string{buf.data()};

        in.read(reinterpret_cast<char*>(&entry.m_timestamp), sizeof(entry.m_timestamp));

        return in;
    }

    bool operator==(const Entry& entry) const
    {
        return (m_index == entry.m_index)
            && (m_data == entry.m_data)
            && (m_timestamp == entry.m_timestamp);
    }
};

class IWAL
{
public:
    // Writes an entry to the log.
    virtual void Write(const Entry& entry) = 0;

    // Reads all entries from the log.
    virtual std::vector<Entry> Read() = 0;

    virtual ~IWAL() = default;
};

class WriteAheadLog : public IWAL
{
  private:
    // TODO: Add a mutex

    // The path to the log folder.
    std::string m_folder_path;

    // The current log file being used for the current segment.
    std::fstream m_log_file;

    // Method that throws if the the current log file is not open.
    void assert_file_open() const;

    // The current segment being used.
    std::vector<Entry> m_current_segment;

    // The max size of each segment.
    std::size_t m_max_segment_size;

    // The counter used to get the highest segment number and used in the logfile.
    std::size_t m_segment_counter;

    // Called by the constructor to create the log directory.
    void SetupLogDir(const std::string& folder_path);

  public:
    WriteAheadLog() = default;
    ~WriteAheadLog() override = default;

    WriteAheadLog(const std::string& file);
    WriteAheadLog(const std::string& file, std::size_t max_segment_size);

    void Write(const Entry& entry) override;
    std::vector<Entry> Read() override;
  };
}

#endif
