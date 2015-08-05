#ifndef NON_COPYABLE_HPP
#define	NON_COPYABLE_HPP

/**
 * The base for all classes that should not be copy constructable.
 */
class NonCopyable
{
protected:
    /**
     * Default constructor.
     */
    NonCopyable() = default;

private:
    /**
     * Private (disabled) copy constructor.
     */
    NonCopyable(const NonCopyable&);

    /**
     * Private (disabled) assignment operator.
     */
    NonCopyable& operator =(const NonCopyable&);
};

#endif