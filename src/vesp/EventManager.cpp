#include "vesp/EventManager.hpp"
#include "vesp/String.hpp"

#include "vesp/util/MurmurHash.hpp"

namespace vesp
{
	EventManager::EventManager()
	{
	}

	void EventManager::Subscribe(RawStringPtr eventName, EventHandler function)
	{
		auto hash = util::MurmurHash(eventName);
		this->events_[hash].push_back(function);
	}

	bool EventManager::Fire(RawStringPtr eventName, void const* argument)
	{
		auto hash = util::MurmurHash(eventName);
		auto it = this->events_.find(hash);

		if (it == this->events_.end())
			return true;

		bool ret = true;
		for (EventHandler& f : it->second)
			ret &= f(argument);

		return ret;
	}
}