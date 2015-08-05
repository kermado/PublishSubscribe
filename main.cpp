#include "EventDispatcher.hpp"
#include "Event.hpp"

#include <memory>
#include <iostream>

/**
 * A simple event type that actors can both publish and subscribe to receive.
 */
struct SomeEvent : public Event
{
    /**
     * Constructor.
     *
     * @param number Integral value to be stored.
     */
    SomeEvent(int number = 0)
    : value(number)
    {
        // Nothing to do.
    }

    /**
     * Integral value stored in the event.
     */
    int value;
};

int main()
{
    // The EventDispatcher instance provides a central point through which events can be published and actors can
    // subscribe to receive those events.
    EventDispatcher dispatcher;

    // Local scope to test subscription lifetime management.
    {
        // Subscribe to receive events of type `SomeEvent` to the lambda callback function. We could also use free
        // functions and member functions to serve as callback (see: `std::bind`).
        auto subscription = dispatcher.subscribe<SomeEvent>([](std::shared_ptr<SomeEvent> event) {
            std::cout << "(Callback 1) Event value: " << event->value << std::endl;
        });

        // Now publish a few events.
        for (int i = 0; i < 3; ++i)
        {
            dispatcher.publish(std::make_shared<SomeEvent>(i));
        }
    }

    // Subscription is cancelled before this is called!
    dispatcher.publish(std::make_shared<SomeEvent>(3));

    return 0;
}