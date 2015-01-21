#pragma once

#include "vesp/Types.hpp"
#include "vesp/util/GlobalSystem.hpp"

#include <unordered_map>
#include <vector>
#include <functional>

namespace vesp
{
	class EventManager : public util::GlobalSystem<EventManager>
	{
	public:
		EventManager();

		typedef std::function<bool (void*)> EventHandler;

		void Subscribe(StringPtr eventName, EventHandler function);
		bool Fire(StringPtr eventName, void* argument = nullptr);
	
	private:
		std::unordered_map<U32, std::vector<EventHandler>> events_;
	};
}