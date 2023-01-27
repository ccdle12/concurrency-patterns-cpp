#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include "wal/wal.h"
#include <filesystem>
#include "test_fixtures.h"

namespace fs = std::filesystem;
using WAL = wal::WriteAheadLog;
using Entry = wal::Entry;

// Test folder for the WAL. The folder should be deleted at the end of each test.
const std::string test_folder{"./test-log"};

TEST(WriteAheadLog, WriteAndRead)
{
    // Check that we can create a ptr and use the IWAL interface.
    auto log = WAL{test_folder};

    // TODO: The default max segment size is 3, so we need to add 6 entries to
    // flush to 2 log files.
    std::vector<Entry> input
    {
        Entry{5, "Hello, World", 1670272110},
        Entry{7, "Set(Foo)", 1670272111},
        Entry{8, "Get(Bar)", 1670272112},
        Entry{9, "Get(AGAIN1)", 1670272113},
        Entry{9, "Get(AGAIN2)", 1670272113},
        Entry{9, "Get(AGAIN3)", 1670272113},
    };

    for (const auto& entry : input)
        log.Write(entry);

    auto results = log.Read();
    ASSERT_EQ(results.size(), input.size());
    for (int i = 0; i < input.size(); ++i)
    {
        ASSERT_EQ(input[i], results[i]);
    }

    fs::remove_all(test_folder);
}

TEST(WriteAheadLog, KVStore)
{

    {
        auto log = std::make_unique<WAL>(test_folder);
        KVStore<std::string, int> kv{std::move(log)};

        kv.Put("foo", 1);
        kv.Put("foo2", 2);
        kv.Put("foo3", 3);

        auto bar = kv.Get("foo");
        ASSERT_EQ(bar, 1);
    }

    // Should be able to rebuild the in-memory cache from the WriteAheadLog.
    auto log = std::make_unique<WAL>(test_folder);
    KVStore<std::string, int> kv{std::move(log)};

    auto bar = kv.Get("foo");
    ASSERT_EQ(bar, 1);

    fs::remove_all(test_folder);
}
