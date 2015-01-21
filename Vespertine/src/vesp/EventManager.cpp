#include "vesp/EventManager.hpp"
#include "vesp/util/MurmurHash.hpp"

namespace vesp
{
	EventManager::EventManager()
	{
	}

	void EventManager::Subscribe(StringPtr eventName, EventHandler function)
	{
		auto hash = util::MurmurHash(eventName);
		events_[hash].push_back(function);
	}

	bool EventManager::Fire(StringPtr eventName, void* argument)
	{
		auto hash = util::MurmurHash(eventName);
		auto it = events_.find(hash);

		if (it == events_.end())
			return true;

		bool ret = true;
		for (EventHandler& f : it->second)
			ret &= f(argument);

		return ret;
	}
}