#include <cstdlib>
#include "SharedPtr.h"
#include <cassert>
#include <iostream>
#include <utility>

struct Point {
   int x, y;
   Point(int x, int y) : x(x), y(y) {}
};

int main() {

   // defualt construction 
   {
      SharedPtr<int> p;
      assert(!p);
      assert(p.get() == nullptr);
      assert(p.useCount() == 0);
   }

   // construct from raw T*
   {
      SharedPtr<int> p(new int(5));
      assert(p);
      assert(*p == 5);
      assert(p.useCount() == 1);
    }

   // destructor frees memory when it goes out of scope
   {
      SharedPtr<int> p(new int(42));
   }

   // copy constructor
   {
      SharedPtr<int> a(new int(5));
      SharedPtr<int> b = a;
      assert(a.useCount() == 2);
      assert(b.useCount() == 2);
      assert(a == b);
      assert(*a == *b);
   }

   // move constructor
   {
      SharedPtr<int> a(new int(5));
      SharedPtr<int> b = std::move(a);
      assert(!a);
      assert(b);
      assert(*b == 5);
      assert(b.useCount() == 1);
   }

   // copy assignment
   {
      SharedPtr<int> a(new int(1));
      SharedPtr<int> b(new int(2));
      b = a;
      assert(*b == 1);
      assert(a.useCount() == 2);
      assert(b.useCount() == 2);
   }

   // move assignment
   {
      SharedPtr<int> a(new int(1));
      SharedPtr<int> b(new int(2));
      b = std::move(a);
      assert(!a);
      assert(*b == 1);
      assert(b.useCount() == 1);
   }

   // self-assignment
   {
      SharedPtr<int> a(new int(5));
      SharedPtr<int>& aRef = a;
      a = aRef;
      assert(*a == 5);
      assert(a.useCount() == 1);
   }

   // reset()
   {
      SharedPtr<int> a(new int(5));
      a.reset();
      assert(!a);
      assert(a.useCount() == 0);
   }

   // reset(T*)
   {
      SharedPtr<int> a(new int(5));
      a.reset(new int(10));
      assert(*a == 10);
      assert(a.useCount() == 1);
   }

   // reset(get())
   {
      SharedPtr<int> a(new int(5));
      a.reset(a.get());
      assert(*a == 5);
      assert(a.useCount() == 1);
   }

   // operator->
   {
      SharedPtr<Point> p(new Point(3, 4));
      assert(p->x == 3);
      assert(p->y == 4);
   }

   // makeSharedBasic
   {
      auto p = makeSharedBasic<Point>(7, 8);
      assert(p->x == 7);
      assert(p->y == 8);
      assert(p.useCount() == 1);
   }

   std::cout << "all tests passed\n";

   return EXIT_SUCCESS;
}