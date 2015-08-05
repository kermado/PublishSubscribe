#ifndef EVENT_DISPATCHER_HPP
#define EVENT_DISPATCHER_HPP

#include "NonCopyable.hpp"
#include "Subscription.hpp"
#include "Event.hpp"

#include <memory>
#include <functional>
#include <map>
#include <typeindex>

#include <cassert>

// Forward declarations.
class Event;

/**
 * The abstract base class from which templated `SpecificDispatcher` classes are derived.
 */
class SpecificDispatcherBase : private NonCopyable
{
public:
    /**
     * Constructor.
     */
    SpecificDispatcherBase() = default;

    /**
     * Destructor.
     */
    virtual ~SpecificDispatcherBase() = default;
};

/**
 * Event dispatcher for a specific templated event type.
 *
 * This class is instantiated for each type of event used. The `EventDispatcher` class manages a list of these specific
 * dispatchers for each event type and maps function calls to the appropriate instance.
 */
template<typename EventType>
class SpecificDispatcher : public SpecificDispatcherBase
{
    friend Subscription<EventType>;

private:
    /**
     * Callback function type definition.
     *
     * Callback functions should return void and accept a shared pointer to an event as their single argument.
     */
    typedef std::function<void(std::shared_ptr<EventType>)> CallbackFunction;

public:
    /**
     * Constructor.
     */
    SpecificDispatcher() = default;

    /**
     * Destructor.
     *
     * Cancels any remaining subscriptions for the templated event type.
     */
    ~SpecificDispatcher()
    {
        for (auto& kv : m_callbacks)
        {
            Subscription<EventType>* subscription = kv.first;
            subscription->cancel();
        }
    }

    /**
     * Immediately dispatches the provided event to all callback functions that are subscribed to the templated type.
     *
     * @param event Shared pointer to the event that is to be published.
     */
    void publish(std::shared_ptr<EventType> event)
    {
        static_assert(std::is_base_of<Event, EventType>::value,
                      "The shared_ptr to an event passed must be derived from Event");

        for (auto& kv : m_callbacks)
        {
            CallbackFunction& callback = kv.second;
            callback(event);
        }
    }

    /**
     * Subscribes to receive events of the templated type to the provided `callback` function.
     *
     * @param callback Callback function to receive events of the templated type.
     * @return Subscription object that manages the lifetime for which the provided `callback` function should
     * receive these events.
     */
    std::unique_ptr<Subscription<EventType>> subscribe(CallbackFunction callback)
    {
        std::unique_ptr<Subscription<EventType>> subscription(new Subscription<EventType>(this));
        m_callbacks.insert(std::make_pair(subscription.get(), callback));

        return subscription;
    }

protected:
    /**
     * Cancels the provided subscription.
     *
     * This function is called exclusively by the `Subscription` class. See the `Subscription` class for details on
     * how to unsubscribe for events.
     */
    void unsubscribe(Subscription<EventType>* subscription)
    {
        m_callbacks.erase(subscription);
    }

private:
    /**
     * Bijective map from subscription pointers to the callback functions.
     */
    std::map<Subscription<EventType>*, CallbackFunction> m_callbacks;
};

/**
 * Event dispatch centre that allows actors to publish events and subscribers to receive events of specific types.
 */
class EventDispatcher : private NonCopyable
{
public:
    /**
     * Constructor.
     */
    EventDispatcher() = default;

    /**
     * Destructor.
     */
    ~EventDispatcher() = default;

    /**
     * Immediately dispatches the provided event to all callback functions that are subscribed to receive events of
     * the templated type.
     *
     * @param event Shared pointer to the event that is to be passed to all interested subscribers.
     */
    template<typename EventType>
    void publish(std::shared_ptr<EventType> event)
    {
        return dispatcher<EventType>().publish(event);
    }

    /**
     * Subscribes to receive events of the templated type to the provided `callback` function.
     *
     * @param callback Callback function to receive events of the templated type.
     * @return Subscription object that manages the lifetime for which the provided `callback` function should receive
     * events of the templated type.
     */
    template<typename EventType>
    std::unique_ptr<Subscription<EventType>> subscribe(std::function<void(std::shared_ptr<EventType>)> callback)
    {
        return dispatcher<EventType>().subscribe(callback);
    }

private:
    /**
     * Accessor for the single specific dispatcher instance that handles events of the templated type.
     *
     * @return Reference to the specific dispatcher.
     */
    template<typename EventType>
    SpecificDispatcher<EventType>& dispatcher()
    {
        const std::type_index key = std::type_index(typeid(EventType));
        auto iter = m_dispatchers.find(key);
        if (iter == m_dispatchers.end())
        {
            // A dispatcher for the template type was not found. We better create one!
            SpecificDispatcher<EventType>* dispatcher = new SpecificDispatcher<EventType>();
            m_dispatchers.insert(std::make_pair(key, std::unique_ptr<SpecificDispatcher<EventType>>(dispatcher)));
            return *dispatcher;
        }
        else
        {
            // The type_index object mapping guarantees that the existing specific dispatcher is correct for the
            // templated event type. This allows us to avoid runtime type checking.
            return *static_cast<SpecificDispatcher<EventType>*>(iter->second.get());
        }
    }

private:
    /**
     * Dispatchers indexed by the `type_index` of the specific event type that they manage.
     */
    std::map<std::type_index, std::unique_ptr<SpecificDispatcherBase>> m_dispatchers;
};

#endif