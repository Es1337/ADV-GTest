#include <gtest/gtest.h>
#include <gmock/gmock.h>

template<typename T>
class CBuffer
{
public:
	CBuffer(size_t size) : buffer{ std::unique_ptr<T[]>(new T[size]) }, maxSize{ size } {}

	void add(T item);
	T get();

	bool isEmpty() const;
	bool isFull() const;

	size_t capacity() const;
	size_t currSize() const;

	void clear();

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
	CBuffer<int> cb;

	void SetUp() override
	{
		cb = CBuffer<int>(10);
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