//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <set>

// class set

// pair<iterator,iterator>             equal_range(const key_type& k);
// pair<const_iterator,const_iterator> equal_range(const key_type& k) const;

#include <set>
#include <cassert>

#include "../../min_allocator.h"

int main()
{
    {
    typedef int V;
    typedef std::set<int> M;
    {
        typedef std::pair<M::iterator, M::iterator> R;
        V ar[] =
        {
            5,
            7,
            9,
            11,
            13,
            15,
            17,
            19
        };
        M m(ar, ar+sizeof(ar)/sizeof(ar[0]));
        R r = m.equal_range(5);
        assert(r.first == next(m.begin(), 0));
        assert(r.second == next(m.begin(), 1));
        r = m.equal_range(7);
        assert(r.first == next(m.begin(), 1));
        assert(r.second == next(m.begin(), 2));
        r = m.equal_range(9);
        assert(r.first == next(m.begin(), 2));
        assert(r.second == next(m.begin(), 3));
        r = m.equal_range(11);
        assert(r.first == next(m.begin(), 3));
        assert(r.second == next(m.begin(), 4));
        r = m.equal_range(13);
        assert(r.first == next(m.begin(), 4));
        assert(r.second == next(m.begin(), 5));
        r = m.equal_range(15);
        assert(r.first == next(m.begin(), 5));
        assert(r.second == next(m.begin(), 6));
        r = m.equal_range(17);
        assert(r.first == next(m.begin(), 6));
        assert(r.second == next(m.begin(), 7));
        r = m.equal_range(19);
        assert(r.first == next(m.begin(), 7));
        assert(r.second == next(m.begin(), 8));
        r = m.equal_range(4);
        assert(r.first == next(m.begin(), 0));
        assert(r.second == next(m.begin(), 0));
        r = m.equal_range(6);
        assert(r.first == next(m.begin(), 1));
        assert(r.second == next(m.begin(), 1));
        r = m.equal_range(8);
        assert(r.first == next(m.begin(), 2));
        assert(r.second == next(m.begin(), 2));
        r = m.equal_range(10);
        assert(r.first == next(m.begin(), 3));
        assert(r.second == next(m.begin(), 3));
        r = m.equal_range(12);
        assert(r.first == next(m.begin(), 4));
        assert(r.second == next(m.begin(), 4));
        r = m.equal_range(14);
        assert(r.first == next(m.begin(), 5));
        assert(r.second == next(m.begin(), 5));
        r = m.equal_range(16);
        assert(r.first == next(m.begin(), 6));
        assert(r.second == next(m.begin(), 6));
        r = m.equal_range(18);
        assert(r.first == next(m.begin(), 7));
        assert(r.second == next(m.begin(), 7));
        r = m.equal_range(20);
        assert(r.first == next(m.begin(), 8));
        assert(r.second == next(m.begin(), 8));
    }
    {
        typedef std::pair<M::const_iterator, M::const_iterator> R;
        V ar[] =
        {
            5,
            7,
            9,
            11,
            13,
            15,
            17,
            19
        };
        const M m(ar, ar+sizeof(ar)/sizeof(ar[0]));
        R r = m.equal_range(5);
        assert(r.first == next(m.begin(), 0));
        assert(r.second == next(m.begin(), 1));
        r = m.equal_range(7);
        assert(r.first == next(m.begin(), 1));
        assert(r.second == next(m.begin(), 2));
        r = m.equal_range(9);
        assert(r.first == next(m.begin(), 2));
        assert(r.second == next(m.begin(), 3));
        r = m.equal_range(11);
        assert(r.first == next(m.begin(), 3));
        assert(r.second == next(m.begin(), 4));
        r = m.equal_range(13);
        assert(r.first == next(m.begin(), 4));
        assert(r.second == next(m.begin(), 5));
        r = m.equal_range(15);
        assert(r.first == next(m.begin(), 5));
        assert(r.second == next(m.begin(), 6));
        r = m.equal_range(17);
        assert(r.first == next(m.begin(), 6));
        assert(r.second == next(m.begin(), 7));
        r = m.equal_range(19);
        assert(r.first == next(m.begin(), 7));
        assert(r.second == next(m.begin(), 8));
        r = m.equal_range(4);
        assert(r.first == next(m.begin(), 0));
        assert(r.second == next(m.begin(), 0));
        r = m.equal_range(6);
        assert(r.first == next(m.begin(), 1));
        assert(r.second == next(m.begin(), 1));
        r = m.equal_range(8);
        assert(r.first == next(m.begin(), 2));
        assert(r.second == next(m.begin(), 2));
        r = m.equal_range(10);
        assert(r.first == next(m.begin(), 3));
        assert(r.second == next(m.begin(), 3));
        r = m.equal_range(12);
        assert(r.first == next(m.begin(), 4));
        assert(r.second == next(m.begin(), 4));
        r = m.equal_range(14);
        assert(r.first == next(m.begin(), 5));
        assert(r.second == next(m.begin(), 5));
        r = m.equal_range(16);
        assert(r.first == next(m.begin(), 6));
        assert(r.second == next(m.begin(), 6));
        r = m.equal_range(18);
        assert(r.first == next(m.begin(), 7));
        assert(r.second == next(m.begin(), 7));
        r = m.equal_range(20);
        assert(r.first == next(m.begin(), 8));
        assert(r.second == next(m.begin(), 8));
    }
    }
#if __cplusplus >= 201103L
    {
    typedef int V;
    typedef std::set<int, std::less<int>, min_allocator<int>> M;
    typedef std::pair<M::iterator, M::iterator> R;
    V ar[] =
    {
        5,
        7,
        9,
        11,
        13,
        15,
        17,
        19
    };
    M m(ar, ar+sizeof(ar)/sizeof(ar[0]));
    R r = m.equal_range(5);
    assert(r.first == next(m.begin(), 0));
    assert(r.second == next(m.begin(), 1));
    r = m.equal_range(7);
    assert(r.first == next(m.begin(), 1));
    assert(r.second == next(m.begin(), 2));
    r = m.equal_range(9);
    assert(r.first == next(m.begin(), 2));
    assert(r.second == next(m.begin(), 3));
    r = m.equal_range(11);
    assert(r.first == next(m.begin(), 3));
    assert(r.second == next(m.begin(), 4));
    r = m.equal_range(13);
    assert(r.first == next(m.begin(), 4));
    assert(r.second == next(m.begin(), 5));
    r = m.equal_range(15);
    assert(r.first == next(m.begin(), 5));
    assert(r.second == next(m.begin(), 6));
    r = m.equal_range(17);
    assert(r.first == next(m.begin(), 6));
    assert(r.second == next(m.begin(), 7));
    r = m.equal_range(19);
    assert(r.first == next(m.begin(), 7));
    assert(r.second == next(m.begin(), 8));
    r = m.equal_range(4);
    assert(r.first == next(m.begin(), 0));
    assert(r.second == next(m.begin(), 0));
    r = m.equal_range(6);
    assert(r.first == next(m.begin(), 1));
    assert(r.second == next(m.begin(), 1));
    r = m.equal_range(8);
    assert(r.first == next(m.begin(), 2));
    assert(r.second == next(m.begin(), 2));
    r = m.equal_range(10);
    assert(r.first == next(m.begin(), 3));
    assert(r.second == next(m.begin(), 3));
    r = m.equal_range(12);
    assert(r.first == next(m.begin(), 4));
    assert(r.second == next(m.begin(), 4));
    r = m.equal_range(14);
    assert(r.first == next(m.begin(), 5));
    assert(r.second == next(m.begin(), 5));
    r = m.equal_range(16);
    assert(r.first == next(m.begin(), 6));
    assert(r.second == next(m.begin(), 6));
    r = m.equal_range(18);
    assert(r.first == next(m.begin(), 7));
    assert(r.second == next(m.begin(), 7));
    r = m.equal_range(20);
    assert(r.first == next(m.begin(), 8));
    assert(r.second == next(m.begin(), 8));
    }
#endif
}
