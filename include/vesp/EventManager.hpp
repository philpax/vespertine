#pragma once

#include "vesp/Containers.hpp"
#include "vesp/Types.hpp"
#include "vesp/util/GlobalSystem.hpp"

#include <functional>

namespace vesp
{
	class EventManager : public util::GlobalSystem<EventManager>
	{
	public:
		EventManager();

		typedef std::function<bool (void const*)> EventHandler;

		void Subscribe(RawStringPtr eventName, EventHandler function);
		bool Fire(RawStringPtr eventName, void const* argument = nullptr);
	
	private:
		UnorderedMap<U32, Vector<EventHandler>> events_;
	};
}