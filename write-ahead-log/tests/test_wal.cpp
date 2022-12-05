#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include "wal/wal.h"

TEST(WriteAheadLog, WriteAndRead)
{
    std::string test_file{"./test.dat"};
    wal::WriteAheadLog log{test_file};

    std::vector<wal::Entry> input
    {
        wal::Entry{5, "Hello, World", 1670272110},
        wal::Entry{7, "Set(Foo)", 1670272111},
        wal::Entry{8, "Get(Bar)", 1670272112},
    };

    for (const auto& entry : input)
        log.Write(entry);

    std::vector<wal::Entry> results = log.Read();
    for (int i = 0; i < input.size(); ++i)
    {
        ASSERT_EQ(input[i].m_index, results[i].m_index);
        ASSERT_EQ(input[i].m_data, results[i].m_data);
        ASSERT_EQ(input[i].m_timestamp, results[i].m_timestamp);
    }

    std::remove(test_file.c_str());
}
