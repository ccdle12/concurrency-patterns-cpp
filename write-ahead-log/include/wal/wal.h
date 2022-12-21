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

class WriteAheadLog
{
  private:
    // TODO: Add a mutex
    std::fstream m_log_file;
    void assert_file_open() const;

  public:
    WriteAheadLog(const std::string& file);

    void Write(const Entry& entry);
    std::vector<Entry> Read();
  };
}
