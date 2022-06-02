/**
 * implement a container like std::hashmap
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::equal_to<T> and std::hash<T>
#include <functional>
#include <cstddef>
#include "utility.h"
#include "exception.h"

namespace ticket {
    /**
     * In hashmap, iteration ordering is differ from map,
     * which is the order in which keys were inserted into the map.
     * You should maintain a doubly-linked list running through all
     * of its entries to keep the correct iteration order.
     *
     * Note that insertion order is not affected if a key is re-inserted
     * into the map.
     */

template <typename _Val> struct _Hashmap_node;
template <typename _Val> struct _List_node;

template <typename _Val>
struct _Hashmap_node {
	_Val data;
	_Hashmap_node *next;
	_List_node<_Val> *node;
	_Hashmap_node(const _Val &_data)
		: data(_data), next(nullptr), node(nullptr) {}
};

template <typename _Val>
struct _List_node {
	_List_node *prev;
	_List_node *next;
	_Hashmap_node<_Val> *node;

	_List_node(_Hashmap_node<_Val> *_node = nullptr)
		: prev(nullptr), next(nullptr), node(_node) {}
};

template <typename _Tp>
struct _Linked_Hashmap_iter_traits {
	using value_type = typename _Tp::value_type;
	using pointer    = typename _Tp::pointer;
	using reference  = typename _Tp::reference;
	using container  = _Tp*;
};

template <typename _Hashmap>
struct _Linked_Hashmap_iter_traits <_Hashmap*> {
	using value_type = typename _Hashmap::value_type;
	using pointer    = typename _Hashmap::pointer;
	using reference  = typename _Hashmap::reference;
	using container  = _Hashmap*;
};

template <typename _Hashmap>
struct _Linked_Hashmap_iter_traits <const _Hashmap*> {
	using value_type = typename _Hashmap::value_type;
	using pointer    = typename _Hashmap::const_pointer;
	using reference  = typename _Hashmap::const_reference;
	using container  = const _Hashmap*;
};

template <typename _Ptr>
struct _Linked_Hashmap_iter_base {
	using traits            = _Linked_Hashmap_iter_traits<_Ptr>;
	using value_type        = typename traits::value_type;
	using pointer           = typename traits::pointer;
	using reference         = typename traits::reference;
	using difference_type   = std::ptrdiff_t;
	using iterator_category = std::bidirectional_iterator_tag;

	using _Hashnode  = _Hashmap_node<value_type>*;
	using _Listnode  = _List_node<value_type>*;
	using _Container = typename traits::container;
	using _Self      = _Linked_Hashmap_iter_base<_Ptr>;

	_Listnode node;
	_Container mp;

	_Linked_Hashmap_iter_base(): node(nullptr), mp(nullptr) {}
	template <typename _Iter>
	_Linked_Hashmap_iter_base(const _Iter &o): node(o.node), mp(o.mp) {}
	_Linked_Hashmap_iter_base(_Container ct, _Listnode ptr): node(ptr), mp(ct) {}

	reference operator * () const {return node->node->data; }
	pointer operator -> () const noexcept {return &node->node->data; }

	_Self& operator ++ () {
		if(!node->next) throw invalid_iterator();
		node = node->next; 
		return *this; 
	}
	_Self  operator ++ (int) {
		if(!node->next) throw invalid_iterator();
		_Self tmp = *this; node = node->next; return tmp; 
	}

	_Self& operator -- () {
		if(!node->prev) throw invalid_iterator();
		node = node->prev; return *this; 
	}
	_Self  operator -- (int) {
		if(!node->prev) throw invalid_iterator();
		_Self tmp = *this; node = node->prev; return tmp; 
	}
	
	template <typename _Iter>
	bool operator == (const _Iter &o) const {
		return node == o.node && mp == o.mp;
	}
	template <typename _Iter>
	bool operator != (const _Iter &o) const {
		return node != o.node || mp != o.mp;
	}

};

#define PRIME_LENGTH 1

#if PRIME_LENGTH
	#define DEFAULT_INITCAP 17u
	#define DEFAULT_LOADFAC 1.25f
#else
	#define DEFAULT_INITCAP 16u
	#define DEFAULT_LOADFAC 1.25f
#endif

    
template <
	typename _Key, 
	typename _Tp, 
	typename _Hash = std::hash<_Key>, 
	typename _Equal = std::equal_to<_Key>
>
class hashmap {

public:
	/**
	 * the internal type of data.
	 * it should have a default constructor, a copy constructor.
	 * You can use sjtu::hashmap as value_type by typedef.
	 */
	using key_type        = _Key;
	using mapped_type     = _Tp;
	using value_type      = pair<const _Key, _Tp>;
	using key_hash        = _Hash;
	using key_equal       = _Equal;
	
	using pointer         = value_type*;
	using const_pointer   = const value_type*;
	using reference       = value_type&;
	using const_reference = const value_type&;
	using size_type       = std::size_t;
	
	using Listnode = _List_node<value_type>;
	using Hashnode = _Hashmap_node<value_type>;
	using Self = hashmap<_Key, _Tp, _Hash, _Equal>;
	using iterator = _Linked_Hashmap_iter_base<Self*>;
	using const_iterator = _Linked_Hashmap_iter_base<const Self*>;

private:
	key_hash  hasher;
	key_equal equal;

	size_type size_;
	size_type length;

	Listnode *tail;
	Listnode *head;

	Hashnode **table;
	
	float loadfactor;
	size_type threshold;

#if PRIME_LENGTH
	inline size_type hash(const key_type &val) const {
		return hasher(val); 
	}
	inline size_type index(size_type key) const {
		return key % length;
	}
	inline static size_type enlarge_length(size_type len) {
		for(len <<= 1; ; ++len) {
			bool tag = 1;
			for(int i = 2; i * i <= len; ++i) {
				if(len % i == 0) {tag = 0; break; }  
			}
			if(tag) break;
		}
		return len;
	}
	inline static size_type get_fit_length(size_type siz) {
		size_type len = DEFAULT_INITCAP;
		for(; len < siz; len <<= 1);
		while(1) {
			bool tag = 1;
			for(int i = 2; i * i <= len; ++i) {
				if(len % i == 0) {tag = 0; break; }
			}
			if(tag) break;
		}
		return len;
	}
#else
	inline size_type hash(const key_type &val) const {
		size_type h = hasher(val);
		h ^= (h >> 20) ^ (h >> 12);
		return h ^ (h >> 7) ^ (h >> 4); 
	}
	inline size_type index(size_type key) const {
		return key & (length - 1);
	}
	inline static size_type enlarge_length(size_type len) {
		return len << 1;
	}
	inline static size_type get_fit_length(size_type siz) {
		size_type len = DEFAULT_INITCAP;
		for(; len < siz; len <<= 1);
		return len;
	}
#endif

	inline static Hashnode** allocate_table(size_type len) {
		auto tab = new Hashnode* [len];
		memset(tab, 0, sizeof(Hashnode*) * len);
		return tab;
	}
	inline static void deallocate_table(Hashnode **tab) {
		delete [] tab;
	}

	inline void expand() {
		length = enlarge_length(length);
		auto new_tab = allocate_table(length);
		threshold = loadfactor * length;
		for(auto u = head; u != tail; u = u->next) {
			u->node->next = nullptr;
			find_hashnode(new_tab, u->node->data.first) = u->node;
		}
		deallocate_table(table);
		table = new_tab;
	}

	Hashnode*& find_hashnode(Hashnode **tab, const key_type &key) {
		auto idx = index(hash(key));
		if(!tab[idx] || equal(key, tab[idx]->data.first)) return tab[idx];
		for(auto u = tab[idx]; u; u = u->next) {
			if(!u->next || equal(key, u->next->data.first)) return u->next;
		}
		return tab[0];
	}
	Hashnode* find_hashnode(Hashnode **tab, const key_type &key) const {
		auto idx = index(hash(key));
		if(!tab[idx] || equal(key, tab[idx]->data.first)) return tab[idx];
		for(auto u = tab[idx]; u; u = u->next) {
			if(!u->next || equal(key, u->next->data.first)) return u->next;
		}
		return tab[0];
	}
	void insert_listnode(Listnode *cur, Listnode *nex) {
		auto pre = nex->prev;
		if(!pre) head = cur;
		else pre->next = cur;
		nex->prev = cur;
		cur->next = nex, cur->prev = pre;
	}
	void delete_listnode(Listnode *cur) {
		if(!cur || tail == cur) return ;
		if(head == cur) head = cur->next;
		if(cur->prev) cur->prev->next = cur->next;
		if(cur->next) cur->next->prev = cur->prev;
	}

	inline void copy(const Self &o) {
		length = o.length;
		table = allocate_table(o.length);
		head = tail = new Listnode();
		for(auto u = o.head; u != o.tail; u = u->next) {
			auto cur_hash_node = new Hashnode(u->node->data);
			auto cur_list_node = new Listnode(cur_hash_node);
			insert_listnode(cur_list_node, tail);
			find_hashnode(table, u->node->data.first) = cur_hash_node;
			cur_hash_node->node = cur_list_node;
		}
	}

	inline void clear_node() {
		Listnode *u, *v;
		for(u = head; u != tail; u = v) {
			v = u->next;
			delete u->node; delete u;
		}
		delete tail;
		deallocate_table(table);
	}

public:
 
	/**
	 * TODO two constructors
	 */
	hashmap(): hasher(), equal()
	{
		length = DEFAULT_INITCAP;
		loadfactor = DEFAULT_LOADFAC;
		threshold = loadfactor * length;
		table = allocate_table(length);
		size_ = 0, head = tail = new Listnode();
	}
	hashmap(const hashmap &o): hasher(o.hasher), equal(o.equal)
	{
		loadfactor = o.loadfactor;
		threshold = o.threshold;
		size_ = o.size_, copy(o);
	}
 
	/**
	 * TODO assignment operator
	 */
	hashmap& operator= (const hashmap &o) {
		if(&o == this) return *this;
		clear_node(), copy(o);
		hasher = o.hasher, equal = o.equal;
		loadfactor = o.loadfactor;
		threshold = o.threshold;
		size_ = o.size_;
		return *this;
	}
 
	/**
	 * TODO Destructors
	 */
	~hashmap() {
		clear_node();
	}
 
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	mapped_type& at(const key_type &key) {
		auto cur = find_hashnode(table, key);
		if(!cur) throw index_out_of_bound();
		return cur->data.second;
	}
	const mapped_type& at(const key_type &key) const {
		auto cur = find_hashnode(table, key);
		if(!cur) throw index_out_of_bound();
		return cur->data.second;
	}
 
	/**
	 * TODO
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	mapped_type& operator[] (const key_type &key) {
		if(size_ + 1 == threshold) expand();
		Hashnode* &cur = find_hashnode(table, key);
		if(cur) return cur->data.second;
		size_++;
		auto cur_hash_node = new Hashnode(value_type(key, mapped_type()));
		auto cur_list_node = new Listnode(cur_hash_node);
		insert_listnode(cur_list_node, tail);
		cur = cur_hash_node;
		cur_hash_node->node = cur_list_node;
		return cur->data.second;
	}
 
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const mapped_type& operator[] (const key_type &key) const {
		auto cur = find_hashnode(table, key);
		if(!cur) throw index_out_of_bound();
		return cur->data.second;
	}
 
	/**
	 * return a iterator to the beginning
	 */
	iterator begin() {return iterator(this, head); }
	const_iterator cbegin() const {return const_iterator(this, head); }
 
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() {return iterator(this, tail); }
	const_iterator cend() const {return const_iterator(this, tail); }
 
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const {return size_ == 0; }
 
	/**
	 * returns the number of elements.
	 */
	size_t size() const {return size_; }
 
	/**
	 * clears the contents
	 */
	void clear() {
		clear_node(); 
		length = DEFAULT_INITCAP, size_ = 0;
		threshold = loadfactor * length;
		table = allocate_table(length);
		head = tail = new Listnode();
	}

	void shrink_to_fit() {
		length = get_fit_length(size_);
		auto new_tab = allocate_table(length);
		threshold = loadfactor * length;
		for(auto u = head; u != tail; u = u->next) {
			u->node->next = nullptr;
			find_hashnode(new_tab, u->node->data.first) = u->node;
		}
		deallocate_table(table);
		table = new_tab;
	}
 
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {
		if(size_ + 1 == threshold) expand();
		Hashnode* &cur = find_hashnode(table, value.first);
		if(cur) return pair<iterator, bool>(iterator(this, cur->node), false);
		size_++;
		auto cur_hash_node = new Hashnode(value);
		auto cur_list_node = new Listnode(cur_hash_node);
		cur = cur_hash_node;
		insert_listnode(cur_list_node, tail);
		cur_hash_node->node = cur_list_node;
		return pair<iterator, bool>(iterator(this, cur_list_node), true);
	}
 
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	iterator erase(iterator pos) {
		if(pos.mp != this || pos.node == tail) throw invalid_iterator();
		auto ret = pos; ++ret;
		auto cur_list_node = pos.node;
		auto cur_hash_node = cur_list_node->node;
		find_hashnode(table, cur_hash_node->data.first) = cur_hash_node->next;
		delete_listnode(cur_list_node);
		delete cur_hash_node; 
		delete cur_list_node;
		size_--; return ret;
	}
 
	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0
	 *     since this container does not allow duplicates.
	 */
	size_type count(const key_type &key) const {
		auto cur = find_hashnode(table, key);
		return cur? 1u: 0u;
	}
 
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const key_type &key) {
		auto cur = find_hashnode(table, key);
		return iterator(this, (cur? cur->node: tail));
	}
	const_iterator find(const key_type &key) const {
		auto cur = find_hashnode(table, key);
		return const_iterator(this, (cur? cur->node: tail));
	}
};


}

#endif