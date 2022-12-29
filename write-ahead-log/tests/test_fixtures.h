#include "wal/wal.h"

class KVStore : public wal::WriteAheadLog
{
  private:
      std::map<std::string, std::string> m_cache;

  public:
      KVStore(const std::string& filename) : wal::WriteAheadLog(filename)
      {
          for (const auto& entry : Read())
          {
              const auto& data = entry.m_data;

              auto key = data.substr(4, data.find(",") - 4);
              auto value = data.substr(entry.m_data.find(",") + 2, data.find(")"));
              value = data.substr(data.find(value), value.size() - 1);

              m_cache.emplace(key, value);
          }
      };

      void Put(const std::string& key, const std::string& value)
      {
           std::ostringstream cmd;
           cmd << "Set(" << key << ", " << value << ")";

           // TODO: Time now.
           Write(wal::Entry(m_cache.size(), cmd.str(), 1670272110));

           m_cache.emplace(key, value);
      }

      std::string Get(const std::string& key) const
      {
          return m_cache.at(key);
      }
};
