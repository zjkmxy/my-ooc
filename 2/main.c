/*2:9*/
#include "sequence.h"
#include "list.h"
#include "array.h"
#include "stream.h"

int main()
{
  /* 于是我们可以自由地使用所有的Sequence，不论是以基类还是子类的形式 */
  PSequence list = (PSequence)List_create();
  PSequence array = (PSequence)Array_create(10);
  PStream stream = Stream_create(1, 1);

  list->append(list, 1);
  list->append(list, 2);
  list->append(list, 3);
  list->append(list, 4);
  list->append(list, 5);
  list->append(list, 6);

  array->append(array, 2);
  array->append(array, 3);
  array->append(array, 5);
  array->append(array, 7);
  array->append(array, 11);
  array->append(array, 13);

  Seq_print(list, 10);
  Seq_print(array, 4);
  Seq_print((PSequence)stream, 10);

  list->free(list);
  array->free(array);
  stream->base_seq.free((PSequence)(stream));

  return 0;
}

/*
 * 1.关于多继承：多继承最大的麻烦在于菱形继承：当A是B、C的基类，而D同时继承自B和C的
 * 时候，D中会同时存在A的两个副本。
 *
 * 2.关于构造函数：子类的构造函数已经负责了内存的分配，然而子类可能需要调用基类的
 * 构造函数来初始化基类的成员，因而基类的构造函数不应该分配内存。或者另一种策略是
 * 所有的主要构造函数都不分配内存，另实现一个子构造函数，只分配内存和调用主构造函数。
 * 当然原生支持OO的语言通过语法糖解决了这个问题。
 *
 * 3.关于析构函数：析构函数除了面对和构造函数相似的问题外，还涉及到一个问题：
 * 当我们需要释放一个对象的时候，我们可能拿到的是被转化成基类的指针，
 * 这样我们既不能直接使用基类的析构，也并不知道应该调用哪个子类的析构。
 * 于是大部分情况下，我们应该采用虚函数作为析构函数。
 * 注意构造函数并不是“成员函数”，因而也不涉及到虚函数的问题。
 *
 * 4.关于虚拟基类：事实上除了在子类中直接包含基类外，还有一种方法，即包括一个基类的
 * 指针。这种情况下从子类转化到基类是不可逆的，因为我们无法从基类成员的地址转化为
 * 子类的地址。这样其实基类结构和子类中的成员已经没有任何差别了，属于has-a模式，而
 * 真正的继承是is-a模式。当然我们可以通过在基类中增加一个指向子类的指针解决这个问题。
 * 关于is-a和has-a的区别不再详述。
 */

