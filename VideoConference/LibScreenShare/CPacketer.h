#pragma once
#include <stdint.h>
#include <iostream>
#include <memory>

class CPacketer
{
public:
	CPacketer()
	{
	
	}
	CPacketer&		Init(std::string src = "")
	{
		data = src;
		return *this;
	}
	template <typename T>
	CPacketer&		pull(_Out_ T *dst)
	{
		uint32_t size = sizeof(T);
		memcpy(dst, data.c_str(), size);
		if (data.size() > size)
			data = std::string(data.c_str() + size, data.size() - size);
		else
			data.clear();
		return *this;
	}
	template <typename T>
	CPacketer&		pull(_Out_ std::unique_ptr<T[]> *dst, _Out_ int *count)
	{
		int size = sizeof(T);
		int total_len;
		
		T *_dst;

		*count = *(int*)data.c_str();//get count
		total_len = *count * size + 4;

		
		if (*dst != nullptr)
			dst->release();

		*dst = std::make_unique<T[]>(*count);


		_dst = dst->get();

		for (int i = 0; i < *count; i++)
		{

			memcpy(&_dst[i], data.c_str() + 4 + size * i, size);
		}

		if (int(data.size()) > total_len)
			data = std::string(data.c_str() + total_len, data.size() - total_len);
		else
			data.clear();
		return *this;
	}
	template <typename T>
	CPacketer&		insert(_In_ T *src, _In_ int count = 1, bool sign = false)
	{
		int type_len = sizeof(T);

		char *temp = new char[type_len];

		if (count != 1 || sign)
		{
			u.value = count;
			this->data += std::string(u.bytes, 4);
		}

		for (int i = 0; i < count; i++)
		{
			memcpy(temp, &src[i], type_len);
			this->data += std::string(temp, type_len);
		}
		//delete[]temp;
		return *this;
	}
	
	bool			hasData()
	{
		return data.size() > 0;
	}
	std::string		getData()
	{
		return std::string(data.c_str(), data.size());
	}
private:
	std::string data;
	union { char bytes[4]; int value; } u;
};