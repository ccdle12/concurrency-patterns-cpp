#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include "wal/wal.h"
#include "test_fixtures.h"

TEST(WriteAheadLog, WriteAndRead)
{
    std::string test_file{"./test000.dat"};
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
    ASSERT_EQ(results.size(), input.size());
    for (int i = 0; i < input.size(); ++i)
    {
        ASSERT_EQ(input[i], results[i]);
    }

    std::remove(test_file.c_str());
}

TEST(WriteAheadLog, KVStore)
{
    std::string test_file{"./test001.dat"};

    {
        KVStore<std::string, int> kv{test_file};
        kv.Put("foo", 1);

        auto bar = kv.Get("foo");
        ASSERT_EQ(bar, 1);
    }

    // Should be able to rebuild the in-memory cache from the log file.
    KVStore<std::string, int> kv{test_file};
    auto bar = kv.Get("foo");
    ASSERT_EQ(bar, 1);

    std::remove(test_file.c_str());
}
