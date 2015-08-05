#ifndef EVENT_HPP
#define EVENT_HPP

#include "NonCopyable.hpp"

/**
 * The base from which all event classes are derived.
 */
class Event : private NonCopyable
{
public:
    /**
     * Constructor.
     */
    Event() = default;

    /**
     * Destructor.
     */
    virtual ~Event() = default;
};

#endif