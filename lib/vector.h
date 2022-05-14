#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exception.h"

// #include <climits>
#include <cstddef>
#include <vector>

namespace ticket {
/**
 * @brief a data container like std::vector
 *        store data in a successive memory and support random access.
 */
template <typename T>
class vector {
private:
	// type traits supporter of iterator
	template <typename _Iter>
	class __traits {
	public:
		using value_type = typename _Iter::value_type;
		using pointer    = typename _Iter::pointer;
		using reference  = typename _Iter::reference;	
	};
	// ps: partial specification
	template <typename _Tp>
	class __traits <_Tp*> {
	public:
		using value_type = _Tp;
		using pointer    = _Tp*;
		using reference  = _Tp&;
	};
	template <typename _Tp>
	class __traits <const _Tp*> {
	public:
		using value_type = _Tp;
		using pointer    = const _Tp*;
		using reference  = const _Tp&;
	};

	// base class of iterator
	template <typename _Iter>
	class __iter {
	public:
		// type traits of vector iterator
		using iterator_category = std::random_access_iterator_tag;
		using value_type        = typename __traits<_Iter>::value_type;
		using pointer           = typename __traits<_Iter>::pointer;
		using reference         = typename __traits<_Iter>::reference;
		using difference_type   = std::ptrdiff_t;
	
	private:
		using container = const vector*;
		pointer   _cur; // the address that the iterator points at
		container _con; // the container that the iterator belongs to

	public:
		__iter(): _cur(pointer()), _con(container()) {}
		__iter(const pointer &_p, const container &_c): _cur(_p), _con(_c) {}
		template <typename _Iter1>
		__iter(const __iter<_Iter1> &_o): _cur(_o.base()), _con(_o.cont()) {}

		difference_type operator - (const __iter &_rhs) const {
			if(_con != _rhs._con) throw invalid_iterator();
			return _cur - _rhs._cur;
		}

		__iter  operator +  (const difference_type &_n) const {return __iter(_cur + _n, _con); }
		__iter  operator -  (const difference_type &_n) const {return __iter(_cur - _n, _con); }
		__iter& operator += (const difference_type &_n) {_cur += _n; return *this; }
		__iter& operator -= (const difference_type &_n) {_cur -= _n; return *this; }
	
		__iter  operator ++ (int) {return _iter(_cur++, _con); }
		__iter  operator -- (int) {return _iter(_cur--, _con); }
		__iter& operator ++ () {++_cur; return *this; }
		__iter& operator -- () {--_cur; return *this; }
		
		reference operator *  () const {return *_cur; }
		pointer   operator -> () const {return  _cur; }

		template <typename _Iter1>
		bool operator == (const __iter<_Iter1> &_rhs) const {
			return _cur == _rhs.base() && _con == _rhs.cont(); 
		}
		template <typename _Iter1>
		bool operator != (const __iter<_Iter1> &_rhs) const {
			return _cur != _rhs.base() || _con != _rhs.cont();
		}

		pointer   base() const {return _cur; }
		container cont() const {return _con; }
	};

public:
	// type traits of vector
	using value_type      = T;
	using pointer         = T*;
	using const_pointer   = const T*;
	using reference       = T&;
	using const_reference = const T&;
	using iterator        = __iter<pointer>;
	using const_iterator  = __iter<const_pointer>;
	using size_type       = size_t;
	using difference_type = ptrdiff_t;

private:
	const static size_type INITCAP = 4; // initial size for default construction 
	pointer   _arr; // data container
	size_type _siz; // real size of vector
	size_type _cap; // max capacity of vector

	// tool funtions for memory management
	static pointer allocate(size_type _siz) {
        return static_cast<pointer>(::operator new (_siz * sizeof(T)));
    }
    static void deallocate(pointer _ptr) {
        ::operator delete(_ptr);
    }
	// ps: using variable parameter for constructor
    template <typename... _Args>
    static void construct(pointer _ptr, _Args&&... _args) {
        ::new(_ptr) T(std::forward<_Args>(_args)...);
    }
    static void destroy(pointer _ptr) {
        _ptr->~T();
    }
	void double_space() {
		pointer _new_arr = allocate(_cap << 1);
		for(int i = 0; i < _siz; ++i) {
			construct(_new_arr + i, _arr[i]);
			destroy(_arr + i);
		}
		deallocate(_arr);
		_cap <<= 1;
		_arr = _new_arr; 
	}
		
public:
	vector() {
		_arr = allocate(INITCAP);
		_siz = 0, _cap = INITCAP;
	}
	vector(const vector &_o) {
		_arr = allocate(_o._cap);
		_siz = _o._siz, _cap = _o._cap;
		for(int i = 0; i < _siz; ++i)
			construct(_arr + i, _o._arr[i]);
	}
	
	~vector() {
		for(int i = 0; i < _siz; ++i) destroy(_arr + i);
		deallocate(_arr);
	}

	vector& operator = (const vector &_o) {
		if(&_o != this) {
			for(int i = 0; i < _siz; ++i) destroy(_arr + i);
			deallocate(_arr);
			_arr = allocate(_o._cap);
			_siz = _o._siz, _cap = _o._cap;
			for(int i = 0; i < _siz; ++i)
				construct(_arr + i, _o._arr[i]);
		}
		return *this;
	}
	
	/**
	 * @brief assigns specified element with bounds checking
	 * @throw index_out_of_bound if pos is not in [0, size)
	 */
	T& at(const size_t &pos) {
		if(pos >= _siz) throw index_out_of_bound();
		return _arr[pos];
	}
	const T& at(const size_t &pos) const {
		if(pos >= _siz) throw index_out_of_bound();
		return _arr[pos];
	}
	/**
	 * @brief assigns specified element with bounds checking
	 * @throw index_out_of_bound if pos is not in [0, size)
	 */
	T& operator [] (const size_t &pos) {
		if(pos >= _siz) throw index_out_of_bound();
		return _arr[pos];
	}
	const T& operator [] (const size_t &pos) const {
		if(pos >= _siz) throw index_out_of_bound();
		return _arr[pos];
	}
	
	/**
	 * @brief access the first / last element.
	 * @throw container_is_empty if size == 0
	 */
	const T& front() const {
		if(_siz == 0) throw container_is_empty();
		return _arr[0];
	}
	const T& back() const {
		if(_siz == 0) throw container_is_empty();
		return _arr[_siz - 1];
	}
	
	// signals for range accesse
	iterator begin() {return iterator(_arr, this); }
	iterator end()   {return iterator(_arr + _siz, this); }
	const_iterator cbegin() const {return const_iterator(_arr, this); }
	const_iterator cend()   const {return const_iterator(_arr + _siz, this); }
	
	size_type size()     const {return _siz; }
	size_type capacity() const {return _cap; }
	bool empty() const {return _siz == 0; }
	void clear() {
		for(int i = 0; i < _siz; ++i) destroy(_arr + i);
		_siz = 0;
	}
	
	/**
	 * @brief inserts value before pos
	 * @return an iterator pointing to the inserted value.
	 */
	iterator insert(iterator pos, const T &value) {
		int idx = pos - begin();
		if(_siz == _cap) double_space();
		if(_siz) construct(_arr + _siz, _arr[_siz - 1]);
		// default shallow copy if operator= is not defined
		for(int i = _siz - 1; i > idx; --i) _arr[i] = _arr[i - 1];
		_arr[idx] = value, _siz++;
		return iterator(_arr + idx, this);
	}
	/**
	 * @brief inserts value at index idx.
	 *        after inserting, this->at(ind) == value
	 * @return an iterator pointing to the inserted value.
	 * @throw index_out_of_bound if idx > size 
	 *        (in this situation ind can be size because after inserting the size will increase 1.)
	 */
	iterator insert(const size_t &idx, const T &value) {
		if(idx > _siz) throw index_out_of_bound();
		if(_siz == _cap) double_space();
		if(_siz) construct(_arr + _siz, _arr[_siz - 1]);
		// default shallow copy if operator= is not defined
		for(int i = _siz - 1; i > idx; --i) _arr[i] = _arr[i - 1];
		_arr[idx] = value, _siz++;
		return iterator(_arr + idx, this);
	}
	/**
	 * @brief  removes the element at pos.
	 * @return an iterator pointing to the following element.
	 *         If the iterator pos refers the last element, the end() iterator is returned.
	 */
	iterator erase(iterator pos) {
		int idx = pos - begin(); _siz--;
		// default shallow copy if operator= is not defined
		for(int i = idx; i < _siz; ++i) _arr[i] = _arr[i + 1];
		destroy(_arr + _siz);
		return iterator(_arr + idx, this);
	}
	/**
	 * @brief removes the element with index idx.
	 * @return an iterator pointing to the following element.
	 * @throw index_out_of_bound if idx >= size
	 */
	iterator erase(const size_t &idx) {
		if(idx >= _siz) throw index_out_of_bound();
		_siz--;
		// default shallow copy if operator= is not defined
		for(int i = idx; i < _siz; ++i) _arr[i] = _arr[i + 1];
		destroy(_arr + _siz);
		return iterator(_arr + idx, this);
	}
	
	/**
	 * @brief add an element to the end.
	 */
	void push_back(const T &value) {
		if(_siz == _cap) double_space();
		construct(_arr + _siz, value);
		_siz++;
	}
	/**
	 * @brief remove the last element from the end.
	 * @throw container_is_empty if size() == 0
	 */
	void pop_back() {
		if(_siz == 0) throw container_is_empty();
		_siz--, destroy(_arr + _siz);
	}
	
	/**
	 * @brief shrink the capacity of vector to fit its size
	 */
	void shrink_to_fit() {
		pointer _new_arr = allocate(_siz + 1);
		for(int i = 0; i < _siz; ++i) {
			construct(_new_arr + i, _arr[i]);
			destroy(_arr + i);
		}
		deallocate(_arr);
		_arr = _new_arr;
	}

	void resize(size_type siz) {
		while(_cap < siz) double_space();
	}
};

}

#endif //SJTU_VECTOR_HPP
