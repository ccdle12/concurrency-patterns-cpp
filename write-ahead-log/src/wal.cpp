#include "wal/wal.h"
#include <filesystem>
#include <ctype.h>
#include <bits/stdc++.h>
#include <iterator>

using namespace wal;
namespace fs = std::filesystem;

void WriteAheadLog::assert_file_open() const
{
    if (!m_log_file.is_open())
        throw std::runtime_error("Failed to open log file");
}

WriteAheadLog::WriteAheadLog(const std::string& folder_path) :
    m_folder_path{folder_path}, m_max_segment_size{3}
{
    SetupLogDir(folder_path);
}

WriteAheadLog::WriteAheadLog(const std::string& folder_path, std::size_t max_segment_size) :
    m_folder_path{folder_path}, m_max_segment_size{max_segment_size}
{
    SetupLogDir(folder_path);
}

void WriteAheadLog::SetupLogDir(const std::string& folder_path)
{
    // Doesn't override the existing directory if it exists.
    fs::create_directories(folder_path);

    // Find the most recent log file and open it.
    auto fs_iter = fs::recursive_directory_iterator(folder_path);
    auto highest_log = 0;
    for (auto const& log : fs_iter) {
        auto log_prefix = log.path().filename().string()[0];

        if (std::isdigit(log_prefix)) {
            int log_num = log_prefix - '0';
            if (log_num > highest_log)
                highest_log = log_num;
        }
    }

    std::stringstream log_file;
    log_file << folder_path
             << "/"
             << highest_log
             << "_log.dat";

    m_log_file.open(log_file.str(), std::ios::binary | std::ios::in | std::ios::out | std::ios::app);
}

void WriteAheadLog::Write(const Entry& entry)
{ 
    assert_file_open();

    // This changes the previously assumed contract of only writing to the disk
    // once the segment is full.
    m_current_segment.push_back(std::move(entry));

    if (m_current_segment.size() >= m_max_segment_size)
    {
        for (auto const& entry : m_current_segment)
            m_log_file << entry;

        m_current_segment.clear();
        m_log_file.close();

        m_segment_counter += 1;

        std::stringstream new_segment_file;
        new_segment_file << m_folder_path
                         << "/"
                         << m_segment_counter
                         << "_log.dat";

        m_log_file.open(new_segment_file.str(), std::ios::binary | std::ios::in | std::ios::out | std::ios::app);
    }

}

std::vector<Entry> WriteAheadLog::Read() 
{
    auto fs_iter = fs::recursive_directory_iterator(m_folder_path);

    std::vector<Entry> entries;
    for (auto const& log : fs_iter) {
        std::fstream log_file;
        log_file.open(log.path().string(), std::ios::binary | std::ios::in);

        log_file.seekg(0, std::ios::end);
        auto end = log_file.tellg();

        log_file.seekg(0);

        // TODO: This is horrible, but builds the vector of entries
        // in the correct historical order.
        std::vector<Entry> tmp_buf;
        for (int i = 0; i < end; i = log_file.tellg())
        {
            Entry entry;
            log_file >> entry;

            tmp_buf.push_back(std::move(entry));
        }

        std::reverse(tmp_buf.begin(), tmp_buf.end());
        for (auto& e : tmp_buf) {
            entries.push_back(std::move(e));
        }
    }

    std::reverse(entries.begin(), entries.end());
    return entries;
}
