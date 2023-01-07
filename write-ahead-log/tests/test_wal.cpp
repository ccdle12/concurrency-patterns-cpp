#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include "wal/wal.h"
#include <filesystem>
#include "test_fixtures.h"

namespace fs = std::filesystem;

// Test folder for the WAL. The folder should be deleted at the end of each test.
const std::string test_folder{"./test-log"};

TEST(WriteAheadLog, WriteAndRead)
{
    // Check that we can create a ptr and use the IWAL interface.
    std::unique_ptr<wal::IWAL> log = std::make_unique<wal::WriteAheadLog>(test_folder);

    std::vector<wal::Entry> input
    {
        wal::Entry{5, "Hello, World", 1670272110},
        wal::Entry{7, "Set(Foo)", 1670272111},
        wal::Entry{8, "Get(Bar)", 1670272112},
    };

    for (const auto& entry : input)
        log->Write(entry);

    auto results = log->Read();
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
        KVStore<std::string, int> kv{test_folder};
        kv.Put("foo", 1);

        auto bar = kv.Get("foo");
        ASSERT_EQ(bar, 1);
    }

    // Should be able to rebuild the in-memory cache from the log file.
    KVStore<std::string, int> kv{test_folder};
    auto bar = kv.Get("foo");
    ASSERT_EQ(bar, 1);

    fs::remove_all(test_folder);
}
