#ifndef NOT_STD_SCOPE_HXX
#define NOT_STD_SCOPE_HXX

#include <type_traits>
#include <utility>

namespace notstd
{
namespace details
{
/**
 * @brief Executes a given lambda when destroyed.
 * 
 * @tparam TLambda 
 */
template <typename TLambda>
class lambda_call
{
public:
    lambda_call(const lambda_call&) = delete;
    lambda_call&
    operator=(const lambda_call&) = delete;
    lambda_call&
    operator=(lambda_call&& other) = delete;

    /**
     * @brief Construct a new lambda call object.
     * 
     * @param lambda 
     */
    explicit lambda_call(TLambda&& lambda) noexcept :
        m_lambda(std::move(lambda))
    {
        static_assert(std::is_same<decltype(lambda()), void>::value, "scope_exit lambdas must not have a return value");
        static_assert(!std::is_lvalue_reference<TLambda>::value && !std::is_rvalue_reference<TLambda>::value,
            "scope_exit should only be directly used with a lambda");
    }

    /**
     * @brief Construct a new lambda call object.
     * 
     * @param other 
     */
    lambda_call(lambda_call&& other) noexcept :
        m_lambda(std::move(other.m_lambda)),
        m_call(other.m_call)
    {
        other.m_call = false;
    }

    /**
     * @brief Destroy the lambda call object.
     */
    ~lambda_call() noexcept
    {
        reset();
    }

    /**
     * @brief Ensures the scope_exit lambda will not be called.
     */
    void
    release() noexcept
    {
        m_call = false;
    }

    /**
     * @brief Executes the scope_exit lambda immediately if not yet run; ensures
     * it will not run again.
     */
    void
    reset() noexcept
    {
        if (m_call) {
            m_call = false;
            m_lambda();
        }
    }

    /**
     * @brief Returns true if the scope_exit lambda is still going to be
     * executed.
     * 
     * @return true 
     * @return false 
     */
    explicit operator bool() const noexcept
    {
        return m_call;
    }

private:
    TLambda m_lambda;
    bool m_call = true;
};
} // namespace details

/**
 * @brief Returns an object that executes the given lambda when destroyed. 
 * 
 * @tparam TLambda 
 * @param lambda 
 * @return auto 
 */
template <typename TLambda>
[[nodiscard]] inline auto
scope_exit(TLambda&& lambda) noexcept
{
    return details::lambda_call<TLambda>(std::forward<TLambda>(lambda));
}

/**
 * @brief Name alias for scope_exit.
 * 
 * @tparam TLambda 
 * @param lambda 
 * @return auto 
 */
template <typename TLambda>
[[nodiscard]] inline auto
ScopeExit(TLambda&& lambda) noexcept
{
    return scope_exit(std::forward<TLambda>(lambda));
}

} // namespace notstd

#endif // NOT_STD_SCOPE_HXX
