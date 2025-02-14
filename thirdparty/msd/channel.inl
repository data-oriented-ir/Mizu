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

namespace msd {

template <typename T>
constexpr channel<T>::channel(const size_type capacity) : cap_{capacity}
{
}

template <typename T>
channel<typename std::decay<T>::type>& operator<<(channel<typename std::decay<T>::type>& ch, T&& in)
{
    if (ch.closed()) {
        throw closed_channel{"cannot write on closed channel"};
    }

    {
        std::unique_lock<std::mutex> lock{ch.mtx_};
        ch.waitBeforeWrite(lock);

        ch.queue_.push(std::forward<T>(in));
        ++ch.size_;
    }

    ch.cnd_.notify_one();

    return ch;
}

template <typename T>
channel<T>& operator>>(channel<T>& ch, T& out)
{
    if (ch.closed() && ch.empty()) {
        return ch;
    }

    {
        std::unique_lock<std::mutex> lock{ch.mtx_};
        ch.waitBeforeRead(lock);

        if (!ch.empty()) {
            out = std::move(ch.queue_.front());
            ch.queue_.pop();
            --ch.size_;
        }
    }

    ch.cnd_.notify_one();

    return ch;
}

template <typename T>
constexpr typename channel<T>::size_type channel<T>::size() const noexcept
{
    return size_;
}

template <typename T>
constexpr bool channel<T>::empty() const noexcept
{
    return size_ == 0;
}

template <typename T>
void channel<T>::close() noexcept
{
    {
        std::unique_lock<std::mutex> lock{mtx_};
        is_closed_.store(true);
    }
    cnd_.notify_all();
}

template <typename T>
bool channel<T>::closed() const noexcept
{
    return is_closed_.load();
}

template <typename T>
blocking_iterator<channel<T>> channel<T>::begin() noexcept
{
    return blocking_iterator<channel<T>>{*this};
}

template <typename T>
blocking_iterator<channel<T>> channel<T>::end() noexcept
{
    return blocking_iterator<channel<T>>{*this};
}

template <typename T>
void channel<T>::waitBeforeRead(std::unique_lock<std::mutex>& lock)
{
    cnd_.wait(lock, [this]() { return !empty() || closed(); });
}

template <typename T>
void channel<T>::waitBeforeWrite(std::unique_lock<std::mutex>& lock)
{
    if (cap_ > 0 && size_ == cap_) {
        cnd_.wait(lock, [this]() { return size_ < cap_; });
    }
}

}  // namespace msd
