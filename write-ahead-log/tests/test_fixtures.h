#include "wal/wal.h"

template<typename K, typename V>
class KVStore
{
  private:
      std::unique_ptr<wal::IWAL> m_log;
      std::map<K, V> m_cache;

      std::map<K, V> unpack_log(const std::unique_ptr<wal::IWAL>& log) const
      {
          std::map<K, V> cache;
          for (const auto& entry : m_log->Read())
          {
              const auto& data = entry.m_data;

              auto key = data.substr(4, data.find(",") - 4);
              auto value = data.substr(entry.m_data.find(",") + 2, data.find(")"));
              value = data.substr(data.find(value), value.size() - 1);

              std::istringstream ss(value);
              V converted_value;
              ss >> converted_value;

              cache.emplace(key, converted_value);
          }

          return cache;
      }

  public:
      KVStore(std::unique_ptr<wal::IWAL>&& log)
      {
          m_log = std::move(log);
          m_cache = unpack_log(m_log);
      };

      void Put(const K& key, const V& value)
      {
           std::ostringstream cmd;
           cmd << "Set(" << key << ", " << value << ")";

           // TODO: Time now.
           m_log->Write(wal::Entry(m_cache.size(), cmd.str(), 1670272110));

           m_cache.emplace(key, value);
      }

      V Get(const K& key) const
      {
          return m_cache.at(key);
      }
};
