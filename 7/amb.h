/*7:1*/

/*
 * 这次我们来解决这样的问题：
 *
 * 五个女生参加一个考试，每个姑娘对此都写一句真话和一句假话。如下：
 * Betty : Kitty考第二，我只考了第三
 * Ethel : 你们应该很高兴听到我考了第一，Joan第二
 * Joan  : 我考第三，可怜的Ethel垫底
 * Kitty : 我第二，Marry只考了第四
 * Marry : 我是第四，Betty的成绩最高。
 * 这五个姑娘的实际排名是什么？
 *
 * 这个问题本身不难，但是这里我们采用Amb函数法求解。
 * （参考页面：http://www.blogjava.net/killme2008/archive/2010/04/05/240672.html）
 * Amb函数即非确定性函数，即可能返回不同值的函数。为此，理想的主函数长这样：
 *
 * main(){
 * betty = choose({1, 2, 3, 4, 5})
 * ethel = choose({1, 2, 3, 4, 5})
 * joan = choose({1, 2, 3, 4, 5})
 * kitty = choose({1, 2, 3, 4, 5})
 * marry = choose({1, 2, 3, 4, 5})
 *
 * if(unique({betty, ..., marry}) != 5) fail()
 * if(!xor(kitty == 2, betty == 3)) fail()
 * .....
 * if(!xor(marry == 4, betty == 1)) fail()
 *
 * print({betty, ..., marry})
 * }
 *
 * 我们希望choose函数从表中选择一个值返回，并且当遇到fail时自动回到choose并换一个值返回。
 * 也就是说，choose到fail之间的代码会被执行多次，当最后的检查结束后，我们得到的值是有效结果。
 *
 * 当然在C语言中，最终我们没能这样写。（这点最后解释）但是我们可以写出逻辑类似的代码。
 * 分析如下：
 *
 * 函数为什么要有返回值？有返回值的函数一定意味着我们要拿一个值去“做什么”。
 * 不“做什么”的返回值没有意义。譬如说，如果一个程序只会成功不会失败，那么main的返回值就没有意义。
 * 假设我们有这样的代码：
 * int a = f(10);
 * printf("%d\n", a);
 * “做什么”也就意味着printf("%d\n", a);
 * 如果我们把“做什么”，抽象成一个函数，代码将变成这个样子：
 * void cont(int a){ printf("%d\n", a);}
 * cont(f(10));
 * 我们拿到值后要“做什么”，在编程中，称为“延续”(continuation)。
 * 如这里cont就是f(10)的延续。
 *
 * 之前我们看到，函数指针被广泛地用于处理回调。然而单独一个函数往往不能处理所有的问题。
 * 譬如，游戏中，当按键按下时做出攻击动作。这样的动作我们可以写在按钮的回调中。
 * 然而这个游戏存在等级制，等级越高攻击力越强。显然我们无法给不同的攻击力制定不同的回调函数。
 * 这启发我们，函数存在一定的“语境”，我们不能让按钮的回调变成这样：
 * if(onClicked) onClicked(self);
 * 它需要是这样的：
 * if(onClicked && env) onClicked(self, env);
 * 不同的回调函数可能需要不同的环境，而按钮无法知道这个环境是什么。
 * 为此我们只好把事件做成对象，这个对象本身的成员变量是环境，成员函数只有一个，就是调用回调：
 * if(onClidked) onClicked.run(&onClicked, self);
 * 这个环境或者语境，我们称为“上下文”(context)，它包括这个回调函数所有可能访问到的内容，
 * 包括全局变量、全局函数定义，以及所有把这个函数封装成事件交给Button时已经定义好的东西。
 *
 * 现在换个角度，我们来看onClicked的函数体，把函数头去掉是这样的：
 * if(button == BUTTON_ID_3) dealDamage(damages[cur_level]);
 * 容易看到其中出现的4个量：button，damages，cur_level，dealDamage中，button是函数参数，
 * 另外3个则不是；特别的，dealDamage是外部的函数。
 * 原始的计算机科学家们借用了数理逻辑的概念，把这个函数的模型写成这样的：
 * λ(button) . (button, dealDamage(damages, cur_level))
 * 这里我们把函数内的动作做了简化表述。
 * 如果我们把λ和谓词逻辑中的“量词”类比，把dealDamage看作是“谓词”，我们可以说：
 * 变元button是受λ约束的，变元damages和cur_level不受任何东西约束。
 * 因此我们管button称为“约束变元”，将damages和cur_level称为“自由变元”
 * 易见这个函数中出现的所有自由变元是上下文的子集。
 *
 * 一个函数，和它的上下文合并在一起形成的对象称为“闭包”(closure)。
 * 在实现上，我们并不需要给闭包封入所有的上下文，只需要封入其自由变元即可。
 * 简而言之也就是把函数用到的、不是函数参数的东西和函数指针一起封装成对象就好了。
 * 后面我们在代码中会结合具体实例更详细地解释这个问题，但是如果你希望了解更多，
 * 建议去学习一下函数式编程语言，或者看一看lambda验算。
 *
 * 到这里我们就可以想到解决方案了：把choose的延续封成闭包，交给Amb，让Amb在
 * choose到值得时候自己去调用好了。这就是我们要采用的解决方案。
 * 这种解决方案可能看起来非常丑，但是是非常重要的一种思想，在C#等现代编程语言中
 * 通过语法糖的形式原生支持，用于处理多线程并发等各种问题。
 * 因为这里我们不管做什么事，都不能直接获得返回值，而必须把延续(continuation)当
 * 参数传递(passing)给我们所调用的函数，因此称为“延续传递式”(CPS = continuation
 *  passing style)。而C#中的yeild return等语法糖必须想办法把本来是串行的程序转化
 *  成CPS的程序，称为“CPS变换”。
 *
 *  除函数式编程语言外，CPS变换只能依赖于编译器的语法糖，无法人工实现。因此这里
 *  我们只能手写CPS。这看上去的确很丑陋；在一个支持CPS变换的语言中，我们可以写出
 *  上面的所谓“理想的主函数”。但是请记住他们本质并无差别。
 */

#ifndef _AMB_H
#define _AMB_H

#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <stdarg.h>
#include <setjmp.h>

struct Amb;
typedef struct Amb Amb;
typedef struct Amb *PAmb;

struct AmbCont;
typedef struct AmbCont AmbCont;
typedef struct AmbCont *PAmbCont;

/*
 * C++提供了throw/catch，Windows也提供了结构化异常处理SEH可以跳脱函数调用
 * 但是纯C语言并没有提供异常处理机制，所以我们用前向跳转setjmp来模拟：
 * if(setjmp(buf) == JMP_OK){
 * ...
 * {(子函数中)longjmp(buf, JMP_FAIL)}
 * }else{
 * ...
 * }
 * 这段代码相当于：
 * try{
 * ...
 * {(子函数中)throw JMP_FAIL;}
 * }catch(...){
 * ...
 * }
 * 前向跳转使用注意事项见后
 */

#define JMP_OK    0
#define JMP_FAIL  1
#define OPTIONAL

/* choose延续闭包的核 */
struct AmbCont
{
  /*
   * self : 延续闭包本身，通过self找到其他所需量的值。
   * sender : 调用者(Amb函数体)。
   * 当然，本例中amb是上下文的一部分，但是它同时也是调用者Amb
   * 所知的内容，因此这里将其作为约束变元而不是自由变元出现。
   * ret : choose的返回值。
   * ret在构造闭包、调用choose时是不存在的，因而它不是上下文的
   * 一部分，只能作为约束变元出现。
   */
  void(*run)(PAmbCont self, PAmb sender, int ret);
};

/*
 * 构造Amb函数，fail是彻底失败时候跳转到的调用点
 */
PAmb Amb_create(jmp_buf *fail);

/*
 * 选择函数，“返回”(延续)本次选择的结果
 * cont : choose的延续
 * cnt  : 选择肢的个数
 * ...  : 各个选择肢
 */
bool Amb_choose(PAmb self, PAmbCont cont, int cnt, ...);

/*
 * 选择函数的va_list版本，供数组调用
 */
bool Amb_choosev(PAmb self, PAmbCont cont, int cnt, va_list ap);

/*
 * 强制失败，回滚到上一个位置
 */
void Amb_fail(PAmb self);


void Amb_free(PAmb self);

#endif
