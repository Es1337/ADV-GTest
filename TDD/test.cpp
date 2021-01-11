#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <mutex>

template<typename T>
class CBuffer
{
public:
	CBuffer(size_t size) : buffer{ std::unique_ptr<T[]>(new T[size]) }, maxSize{ size } {}

	void add(T item) {
		std::lock_guard<std::mutex> lock(mu);
		buffer[head] = item;

		if (full)
		{
			tail = (tail + 1) % maxSize;
		}

		head = (head + 1) % maxSize;
		full = (head == tail);
	}

	T get() { 
		std::lock_guard<std::mutex> lock(mu);

		if (isEmpty())
		{
			throw std::exception();
		}

		T val = buffer[tail];
		full = false;
		tail = (tail + 1) % maxSize;

		return val;
	}

	bool isEmpty() const { return (!full && (head == tail)); }
	bool isFull() const { return full; }

	size_t getAllocatedSize() const { return maxSize; }
	size_t getSize() const { 
		size_t size = maxSize;

		if (!full)
		{
			if (head >= tail)
			{
				size = head - tail;
			}
			else
			{
				size = maxSize + head - tail;
			}
		}

		return size;
	}

	void clear() {
		std::lock_guard<std::mutex> lock(mu);
		head = tail;
		full = false;
	}

private:
	std::unique_ptr<T[]> buffer;
	size_t head = 0;
	size_t tail = 0;
	const size_t maxSize;
	bool full;
	std::mutex mu;
};

class BufferTest : public ::testing::Test
{
public:
	CBuffer<int> cb{10};

	void SetUp() override
	{
		int testValues[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		for (int& val : testValues)
		{
			cb.add(val);
		}
	}

	void TearDown() override
	{

	}
};

TEST_F(BufferTest, ConstructorTest) {
  ASSERT_EQ(10, cb.getAllocatedSize());
}

TEST_F(BufferTest, SizeTest) {
	cb.get();
	cb.get();
	ASSERT_EQ(8, cb.getSize());
}

TEST_F(BufferTest, AddTest) {
	cb.add(2137);
	for (int i = 0; i < 9; i++)
	{
		cb.get();
	}
	ASSERT_EQ(2137, cb.get());
}

TEST_F(BufferTest, GetTest) {
	ASSERT_EQ(1, cb.get());
}

TEST_F(BufferTest, GetEmptyTest) {
	for (int i = 0; i < 10; i++)
	{
		cb.get();
	}
	ASSERT_THROW(cb.get(), std::exception);
}