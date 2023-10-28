#pragma once

#include <AlbedoCore/Norm/concepts.h>

#include <list>
#include <memory>
#include <unordered_map>

namespace Albedo { namespace Pattern 
{

	// Subscriber Interface
	struct Subscriber { /*virtual void WhenNotified() = 0; As an alternative, using Lambda Function when subscribe.*/ };
	template<typename T>
	concept SubscriberType = std::is_class_v<Subscriber> || std::is_base_of_v<Subscriber, T>;

	// Publisher(Subject) Interface
	template<HashableType EventType, SubscriberType T = Subscriber>
	class Publisher
	{
	public:
		using Reaction = std::function<void()>;

		virtual void AddSubscriber(EventType eventType, std::weak_ptr<T> subscriber, Reaction reaction)
		{
			m_subscriptions[eventType].emplace_back(std::move(subscriber), std::move(reaction));
		}

		virtual void NotifySubscribers(EventType eventType)
		{
			auto& subscriptions = m_subscriptions[eventType];
			if (subscriptions.empty()) return;

			for (auto subscription_iter = subscriptions.begin();
					  subscription_iter != subscriptions.end();)
			{
				if (!subscription_iter->first.expired()) // subscriber pointer
				{
					subscription_iter->second();
					++subscription_iter;
				}
				else subscriptions.erase(subscription_iter++);
			}
		}

	private:
		using Subscription = std::pair<std::weak_ptr<T>, Reaction>;
		std::unordered_map<EventType, std::list<Subscription>> m_subscriptions;
	};

}} // namespace Albedo::Pattern