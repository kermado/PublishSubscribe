#ifndef SUBSCRIPTION_HPP
#define SUBSCRIPTION_HPP

#include "NonCopyable.hpp"

// Forward declarations.
template<class T>
class SpecificDispatcher;

/**
 * Manages the lifetime of an event subscription.
 *
 * The subscription is cancelled automatically upon destruction. Alternatively, the subscription can be manually
 * cancelled by calling `unsubscribe`. In both cases you must keep the subscription object alive as long as you want
 * to remain subscribed.
 */
template<typename EventType>
class Subscription : private NonCopyable
{
    friend SpecificDispatcher<EventType>;

public:
    /**
     * Default constructor removed.
     *
     * Subscriptions can only be created through the `EventDispatcher` class's `subscribe` function.
     */
    Subscription() = delete;

    /**
     * Destructor.
     *
     * Cancels the subscription automatically.
     */
    ~Subscription()
    {
        cancel();
    }

    /**
     * Whether the subscription is active.
     *
     * @return True if the subscription is currently active, false otherwise.
     */
    bool active() const
    {
        return m_specific_dispatcher != nullptr;
    }

    /**
     * Cancels the subscription so that the managed callback function no longer receives events.
     */
    void cancel()
    {
        m_specific_dispatcher->unsubscribe(this);
        m_specific_dispatcher = nullptr;
    }

private:
    /**
     * Constructor.
     *
     * This constructor is available exclusively to the `SpecificDispatcher<EventType>` class.
     *
     * @param specific_dispatcher Pointer to the dispatcher for the specific event type.
     */
    Subscription(SpecificDispatcher<EventType>* specific_dispatcher)
    : m_specific_dispatcher(specific_dispatcher)
    {
        // Nothing to do.
    }

private:
    /**
     * Pointer to the dispatcher for the specific event type. Will be a nullptr if the subscription has been cancelled.
     */
    SpecificDispatcher<EventType>* m_specific_dispatcher;
};

#endif