#ifndef __MAP_UTILS_H__
#define __MAP_UTILS_H__

#include <map>

namespace watchly::utils::map
{
	template <typename K, typename V>
	V getValueForKeyWithDefault(const std::map<K, V>& map, const K& key, const V& defaultValue)
	{
	   typename std::map<K,V>::const_iterator it = map.find(key);
	   
	   if (it == map.end())
	   {
		  return defaultValue;
	   }
	   else
	   {
		  return it->second;
	   }
	}
}

#endif
