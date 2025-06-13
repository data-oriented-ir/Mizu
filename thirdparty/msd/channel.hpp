// From: https://github.com/andreiavrammsd/cpp-channel
// Copyright (c) 2023 Andrei Avram

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is furnished
// to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef MSD_CHANNEL_HPP_
#define MSD_CHANNEL_HPP_

#include <atomic>
#include <condition_variable>
#include <cstdlib>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "blocking_iterator.hpp"

namespace msd {

#if (__cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L))
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif

/**
 * @brief Exception thrown if trying to write on closed channel.
 */
class closed_channel : public std::runtime_error {
   public:
    explicit closed_channel(const char* msg) : std::runtime_error{msg} {}
};

/**
 * @brief Thread-safe container for sharing data between threads.
 *
 * Implements a blocking input iterator.
 *
 * @tparam T The type of the elements.
 */
template <typename T>
class channel {
   public:
    using value_type = T;
    using iterator = blocking_iterator<channel<T>>;
    using size_type = std::size_t;

    /**
     * Creates an unbuffered channel.
     */
    constexpr channel() = default;

    /**
     * Creates a buffered channel.
     *
     * @param capacity Number of elements the channel can store before blocking.
     */
    explicit constexpr channel(size_type capacity);

    /**
     * Pushes an element into the channel.
     *
     * @throws closed_channel if channel is closed.
     */
    template <typename Type>
    friend channel<typename std::decay<Type>::type>& operator<<(channel<typename std::decay<Type>::type>&, Type&&);

    /**
     * Pops an element from the channel.
     *
     * @tparam Type The type of the elements
     */
    template <typename Type>
    friend channel<Type>& operator>>(channel<Type>&, Type&);

    /**
     * Returns the number of elements in the channel.
     */
    NODISCARD inline size_type constexpr size() const noexcept;

    /**
     * Returns true if there are no elements in channel.
     */
    NODISCARD inline bool constexpr empty() const noexcept;

    /**
     * Closes the channel.
     */
    inline void close() noexcept;

    /**
     * Returns true if the channel is closed.
     */
    NODISCARD inline bool closed() const noexcept;

    /**
     * Iterator
     */
    iterator begin() noexcept;
    iterator end() noexcept;

    /**
     * Channel cannot be copied or moved.
     */
    channel(const channel&) = delete;
    channel& operator=(const channel&) = delete;
    channel(channel&&) = delete;
    channel& operator=(channel&&) = delete;
    virtual ~channel() = default;

   private:
    const size_type cap_{0};
    std::queue<T> queue_;
    std::atomic<std::size_t> size_{0};
    std::mutex mtx_;
    std::condition_variable cnd_;
    std::atomic<bool> is_closed_{false};

    inline void waitBeforeRead(std::unique_lock<std::mutex>&);
    inline void waitBeforeWrite(std::unique_lock<std::mutex>&);
    friend class blocking_iterator<channel>;
};

}  // namespace msd

#include "channel.inl"

#endif  // MSD_CHANNEL_HPP_
