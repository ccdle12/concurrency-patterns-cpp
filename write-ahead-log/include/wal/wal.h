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

    Entry() = default;
    explicit Entry(uint64_t index, std::string data) :
        m_index{index}, m_data{data} {};

    friend std::ostream& operator<<(std::ostream& out, const Entry& entry)
    {
        out.write(reinterpret_cast<const char*>(&entry.m_index), sizeof(entry.m_index));

        size_t len = entry.m_data.size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(entry.m_data.c_str(), len);

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

        return in;
    }
};

class WriteAheadLog
{
  private:
    // TODO: Add a mutex
    std::fstream m_log_file;

    inline void assert_file_open() const
    {
        if (!m_log_file.is_open())
            throw std::runtime_error("Failed to open log file");
    }

  public:
    WriteAheadLog(const std::string& file)
    {
        m_log_file.open(file, std::ios::binary | std::ios::in | std::ios::out | std::ios::app);
        assert_file_open();
    }

    void Write(const Entry& entry)
    { 
        assert_file_open();
        m_log_file << entry;
    }

    std::vector<Entry> Read()
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
  };
};
