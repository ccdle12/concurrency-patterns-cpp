// TODO:
// https://martinfowler.com/articles/patterns-of-distributed-systems/wal.html
//
// 1. What should the log format be?
// 2. Input is a fd
// 3. Open fd
// 4. Write, what is the flushing strategy?
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

        // TODO: I want to remove the manual allocation and deallocation.
        char* data = new char[len];
        in.read(data, len);

        entry.m_data = std::string{data};
        delete []data;

        in.read(reinterpret_cast<char*>(&entry.m_timestamp), sizeof(entry.m_timestamp));

        return in;
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
