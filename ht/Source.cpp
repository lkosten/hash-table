#include<iostream>
#include<list>
#include<iterator>
#include<vector>
#include<clocale>

using namespace std;

namespace std
{
	template<class T1, class T2>
	istream& operator>>(istream& in, pair<T1, T2> &p)
	{
		in >> p.first >> p.second;
		return in;
	}

	template<class T1, class T2>
	ostream& operator<<(ostream& in, pair<T1, T2> &p)
	{
		in << p.first << ' ' << p.second;
		return in;
	}
}


template<class key, class value>
class hash_table
{
	const int hash_key = 7;
	vector<list<pair<key, value>>> ht;
	int hf(key k)
	{
		int h = (sizeof(k) - 1) * 8;
		char c = abs((k << h) >> h);

		c %= hash_key;

		return c;
	}

	
public:
	hash_table()
	{
		ht.resize(hash_key);
	}
	hash_table(const hash_table &c)
	{
		for (int i = 0; i < hash_key; i++)
		{
			ht[i] = c.ht[i];
		}
	}
	hash_table(const hash_table &&sw)
	{
		ht = sw.ht;
		sw.ht.clear();
	}
	hash_table(initializer_list<pair<key, value>> list)
	{
		ht.resize(hash_key);
		for (auto j : list) 
		{
			int index = hf(j.first);
			auto i = ht[index].end();
			for (auto it = ht[index].begin(); it != ht[index].end(); ++it)
			{
				if ((*it).first == j.first)
				{
					i = it;
					break;
				}
			}
			if (i == ht[index].end())
			{
				ht[index].push_back(make_pair(j.first, j.second));
				--i;
			}
		}
	}
	

	~hash_table()
	{
		ht.clear();
	}

	class Iterator
	{
		friend hash_table;
		typename list<pair<key, value>>::iterator it;
		int num;
		hash_table *h;

		class Error {};
		void operator=(typename const list<pair<key, value>>::iterator &check)
		{
			it = check.it;
		}
		
	public:
		Iterator() : num(0), h(nullptr){}
		Iterator(hash_table &h) : it(h.ht[0].begin()), num(0), h(&h) {};
		Iterator(const Iterator &cop) : it(cop.it), num(cop.num), h(cop.h) {}
		
		bool operator==(const Iterator &check)
		{
			return (check.it == it && num == check.num);
		}
		bool operator!=(const Iterator &check)
		{
			if (check.it == (check.h->ht[check.num]).end() && it != (h->ht[num]).end() || check.it != (check.h->ht[check.num]).end() && it == (h->ht[num]).end()) return true;
			return (check.it != it || num != check.num);
		}
		Iterator operator=(const Iterator &check)
		{
			if (this == &check)
				return *this;

			it = check.it;
			num = check.num;
			h = check.h;
			return *this;
		}

		pair<key, value>& operator*()
		{
			return (*it);
		}

		Iterator& operator++()
		{
			if (it == h->ht[num].end())
				throw Error();
			++it;
			while (it == (h->ht[num]).end() && num != h->hash_key - 1)
			{
				it = (h->ht[++num]).begin();
			}
			return *this;
		}

		Iterator operator++(int)
		{
			Iterator cop(*this);
			++(*this);
			return cop;
		}
	};

	Iterator begin()
	{
		Iterator a;

		a.h = this;
		a.num = 0;
		a.it = (ht[a.num]).begin();
		while (a.it == (ht[a.num]).end() && a.num != hash_key - 1)
		{
			a.it = (ht[++a.num]).begin();
		}

		return a;
	}
	Iterator end()
	{
		Iterator a;
		a.h = this;
		a.num = hash_key - 1;
		a.it = (ht[a.num]).end();
		return a;
	}
	Iterator Find(const key &k, const value &v)
	{
		Iterator it(*this);
		it = this->end();


		for (auto i = ht[hf(k)].begin(); i != ht[hf(k)].end(); ++i)
		{
			if ((*i) == make_pair(k, v))
			{
				it.num = hf(k);
				it.it = i;
			}
		}

		return it;
	}

	void Clear()
	{
		ht.clear();
		ht.resize(hash_key);
	}
	int Size()
	{
		int counter = 0;
		for (auto &i : ht) counter += i.size();
		return counter;
	}

	template<typename ... Args>
	void Emplace(const key &k, Args ... val)
	{
		int index = hf(k);
		bool check = false;
		for (auto it = ht[index].begin(); it != ht[index].end(); ++it)
		{
			if ((*it).first == k)
			{
				(*it).second = value(val...);
				check = true;
				break;
			}
		}
		if (!check)
		{
			ht[index].emplace_back(make_pair(k, value(val...)));
		}
	}
	bool isEmpty()
	{
		bool ans = false;
		for (auto &i : ht) if (!i.empty()) ans = true;
		return ans;
	}
	bool isFull()
	{
		return !isEmpty();
	}

	void operator=(const hash_table &c)
	{
		for (int i = 0; i < hash_key; i++)
		{
			ht[i] = c.ht[i];
		}
	}
	void Delete(const key &k)
	{
		int index = hf(k);
		for (auto it = ht[index].begin(); it != ht[index].end(); ++it)
		{
			if ((*it).first == k)
			{
				ht[index].erase(it);
				break;
			}
		}
	}
	value& operator[](const key &k)
	{
		int index = hf(k);
		auto i = ht[index].end();
		for (auto it = ht[index].begin(); it != ht[index].end(); ++it)
		{
			if ((*it).first == k)
			{
				i = it;
				break;
			}
		}
		if (i == ht[index].end())
		{
			ht[index].push_back(make_pair(k, (value)nullptr));
			--i;
		}
		return (*i).second;
	}

	friend istream& operator>> (istream &in, hash_table &htable)
	{
		pair<key, value> p;
		in >> p.first >> p.second;
		htable[p.first] = p.second;
		return in;
	}
	friend ostream& operator<< (ostream &out, const hash_table &htable)
	{
		for (int i = 0; i < htable.hash_key; i++)
		{
			for (auto i : htable.ht[i]) out << i.first << ' ' << i.second << endl;
		}
		return out;
	}
	void swap(hash_table &sw)
	{
		hash_table *k = this;


		this = &sw;
		sw = k;
	}

};

template<class T>
class greater
{
public:
	bool operator()(const T &x1, const T &x2)
	{
		return (x2 < x1);
	}
};

int main()
{
	hash_table<int, int> a;
	hash_table<int, int> b({ { 1, 1 }, { 2, 2 } });
	
	cin >> a;
	a.Emplace(100, 50);
	auto it = b.Find(1, 1);
	auto it1 = b.Find(1, 2);
	if (it1 == b.end()) cout << "Ok" << endl;


	for (auto i : a)
	{
		cout << i << endl;
		cout << i.first << ' ' << i.second << endl;
	}
	
	for (auto it = b.begin(); it != b.end(); ++it)
	{
		cout << (*it).first << ' ' << (*it).second << ' ';
	}

	cout << endl << b;


	cout << endl;
	system("PAUSE");
	return 0;
}
