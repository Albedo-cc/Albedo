#pragma once

#include <AlbedoCore/Norm/concepts.h>

#include <list>
#include <memory>
#include <unordered_map>

namespace Albedo
{
	// Subscriber Interface
	struct Subscriber { virtual void WhenNotified() = 0; };
	template<typename T>
	concept SubscriberType = std::is_base_of_v<Subscriber, T>;

	// Publisher(Subject) Interface
	template<SubscriberType T, HashableType EventType = int>
	class Publisher
	{
	public:
		void Subscribe(EventType eventType, std::weak_ptr<T> subscriber)
		{
			m_subscriptions[eventType].emplace_back(std::move(subscriber));
		}

		void NotifySubscribers()
		{
			for (auto& [eventType, subscriptions] : m_subscriptions)
			{
				if (subscriptions.empty()) continue;

				for (auto subscription_iter = subscriptions.begin();
						  subscription_iter != subscriptions.end();)
				{
					if (!subscription_iter->expired()) // subscriber pointer
					{
						subscription_iter->lock()->WhenNotified();
						++subscription_iter;
					}
					else subscriptions.erase(subscription_iter++);
				}
			}
		}

	private:
		std::unordered_map<EventType, std::list<std::weak_ptr<T>>> m_subscriptions;
	};

} // namespace Albedo