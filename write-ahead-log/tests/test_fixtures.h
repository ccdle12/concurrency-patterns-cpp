#include "wal/wal.h"

class KVStore
{
  private:
      wal::WriteAheadLog m_log;
      std::map<std::string, std::string> m_cache;

  public:
      // TODO:
      // enum Command
      // {
      //    Set
      // }
      //
      // struct KVEntry
      // {
      //    std::string key;
      //    std::string value;
      //    Command cmd;
      // }
      //
      // KVStore()
      // {
      //    m_log{"./kvstore0000.dat"};
      //
      //    for (const auto& entry : m_log.Read())
      //    {
      //        auto kv_entry = ParseKVEntry(entry.data);
      //
      //        if (kv_entry.cmd == Command::Set)
      //          m_cache.put(cmd.key, cmd.value);
      //    }
      // }
      //
      // put(key, value)
      // {
      //     m_log.Write(m_cache.size(), std::string{"Set(" << key << "," << value << ")", timenow})
      //     m_cache.put(key, value)
      // }
      //
      // get(key)
      // {
      //    return m_cache.get(key);
      // }
}
