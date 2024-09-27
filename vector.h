#include <iostream>
#ifndef VECTOR_H
#define VECTOR_H

template<typename T>
class Vector {
public:
    class ConstIterator;
    class Iterator;
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = Vector::Iterator;
    using const_iterator = Vector::ConstIterator;

private:
    size_type sz;
    size_type max_sz;
    pointer values; 
    static constexpr size_type min_capacity = 5;

public:
    Vector() : sz{0}, max_sz{min_capacity}, values{new value_type[min_capacity]} {}

    Vector(const Vector& vec) : sz{vec.sz}, max_sz{vec.max_sz}, values{new value_type[max_sz]} {
        for (size_type i = 0; i < sz; ++i) {
            values[i] = vec.values[i];
        }
    }
    
    Vector(size_type n) : sz{0}, max_sz{n}, values{new value_type[n]} {}

    Vector(std::initializer_list<value_type> content) : sz{content.size()}, max_sz{sz}, values{new value_type[max_sz]} {
        size_type i = 0;

        for (const auto& c : content) {
            values[i] = c;
            ++i;
        }
    }

    ~Vector() {
        delete [] values;
    }

    Vector& operator=(Vector v) {
        std::swap(sz, v.sz);
        std::swap(max_sz, v.max_sz);
        std::swap(values, v.values);
        return *this;
    }

    size_type size() const {
        return sz; 
    }

    bool empty() const {
      return sz == 0; 
    }

    void clear() {
        sz = 0;
    }

    void reserve(size_type n) {
        if (max_sz < n) {
            max_sz = n;
            pointer newVal = new value_type[max_sz];
            if (values) {
                for (size_type i = 0; i < sz; ++i) {
                    newVal[i] = values[i];
                }
                
            }
            delete[] values;
            values = newVal;
        }
    }

    void shrink_to_fit() {
        if (sz < max_sz) {
            max_sz = sz;
            reserve(sz);
        }
    }

    void push_back(value_type x) {
      if (sz >= max_sz) {
        reserve(max_sz*2+1);
      }
      values[sz] = x; 
      ++sz;
    }

    void pop_back() {
        if (empty()) {
            throw std::runtime_error("EMPTY VECTOR");
        }
        reserve(--sz);
    }

    reference operator[](size_type index) {
        if (index >= sz) {
            throw std::runtime_error("INDEX OUT OF RANGE");
        }
        return values[index];
    }

    const_reference operator[](size_type index) const {
        if (index >= sz) {
            throw std::runtime_error("INDEX OUT OF RANGE");
        }
        return values[index];
    }

    size_type capacity() const {
        return max_sz;
    }

    iterator begin() {
        if (empty()) {
            return end();
        }
        return iterator{values, values+sz};
    }

    iterator end() {
        return iterator{values+sz, values+sz};
    }

    const_iterator begin() const {
        if (empty()) {
            return end();
        }
        return const_iterator{values, values+sz};
    }

    const_iterator end() const{
        return const_iterator{values+sz, values+sz}; 
    }

    // Insert Erase kopiert
    iterator insert(const_iterator pos, const_reference val) {
		auto diff = pos - begin();
		if (diff < 0 || static_cast<size_type>(diff) > sz) {
			throw std::runtime_error("Iterator out of bounds");
		}
		size_type current{static_cast<size_type>(diff)};
		if (sz >= max_sz) {
			reserve(max_sz*2+1);
		}
		for(auto i{sz}; i-->current;) {
			values[i+1] = values[i];
		}
		values[current] = val;
		++sz;
		return iterator{values + current};
	}
    
	iterator erase(const_iterator pos) {
		auto diff = pos - begin();
		if(diff < 0|| static_cast<size_type>(diff) >= sz) {
			throw std::runtime_error("Iterator out of bounds");
		}
		size_type current{static_cast<size_type>(diff)};
		for(auto i{current}; i < sz-1; i++) {
			values[i] = values[i+1];
		}
		--sz;
		return iterator{values + current};
	}
     // output 
    friend std::ostream& operator<<(std::ostream& o, const Vector v) {
    o << "[";      
    bool first{true};
        for (size_type i{0}; i < v.sz; ++i) { 
            if (first) {
                first = false;
            } else {
                o << ", "; 
            }
           o << v.values[i];
        }
        return o << "]";
    }  

class Iterator {
public:
    using value_type = Vector::value_type;
    using reference = Vector::reference;
    using pointer = Vector::pointer;
    using difference_type = Vector::difference_type;
    using iterator_category = std::forward_iterator_tag;
private:
//Instanzvariablen
    pointer ptr;
    pointer endPtr; 
public:
//Methoden
    Iterator() : ptr{nullptr}, endPtr{nullptr} {}

    Iterator(pointer ptr) : ptr{ptr}, endPtr{nullptr} {}

    Iterator(pointer ptr, pointer endPtr) : ptr{ptr}, endPtr{endPtr} {}

    reference operator*() const {
        if (ptr == endPtr) {
            throw std::runtime_error("OUT OF RANGE");
        } 
        return *ptr;
    }
    
    pointer operator->() const {
        if (ptr == endPtr) {
            throw std::runtime_error("OUT OF RANGE");
        }
        return ptr;
    }

    bool operator== (const const_iterator& other) const {
        return static_cast<ConstIterator>(other) == ptr;
    }

    bool operator!= (const const_iterator& other) const {
        return static_cast<ConstIterator>(other) != ptr; 
    }

    iterator& operator++() {
        if (ptr != endPtr) {
            ++ptr;
        }
        return *this;
    }

    iterator operator++(int) {
        iterator oldPtr{*this}; 
        if (ptr != endPtr) {
            ptr++;
        }
        return oldPtr;
    }

    operator const_iterator() const {
        return ConstIterator(ptr);
    }

};

class ConstIterator {
public:
    using value_type = Vector::value_type;
    using reference = Vector::const_reference;
    using pointer = Vector::const_pointer;  
    using difference_type = Vector::difference_type;
    using iterator_category = std::forward_iterator_tag;
private:
//Instanzvariablen
    pointer ptr;
    pointer endPtr; 

public:
//Methoden
    // kopiert fuer Insert u Erase 
    friend Vector::difference_type operator-(const Vector::ConstIterator& lop, const Vector::ConstIterator& rop) {
		return (lop.ptr - rop.ptr);
    }

    ConstIterator() : ptr{nullptr}, endPtr{nullptr} {}

    ConstIterator(pointer ptr) : ptr{ptr}, endPtr{nullptr} {}

    ConstIterator(pointer ptr, pointer endPtr) : ptr{ptr}, endPtr{endPtr} {}

    reference operator*() {
        if (ptr == endPtr) {
            throw std::runtime_error("OUT OF RANGE");
        }
        return *ptr;
    }

    pointer operator->() {
        if (ptr == endPtr) {
            throw std::runtime_error("OUT OF RANGE");
        }
        return ptr;
    }

    bool operator==(const const_iterator& other) const {
        return ptr == other.ptr;
    }

    bool operator != (const const_iterator& other) const {
        return ptr != other.ptr; 
    }
    
    const_iterator& operator++() {
        if (ptr != endPtr) {
            ++ptr;
        }
        return *this;
    }

    const_iterator operator++(int) {
        const_iterator oldPtr{*this}; 
        if (ptr != endPtr) {
            ptr++;
        } 
        return oldPtr;
    }
}; 
};
#endif
