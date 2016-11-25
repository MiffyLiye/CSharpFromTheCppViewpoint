# 从C++的视角看C#（零） —— Lambda表达式与闭包

## 微软不惜破坏向后兼容也要改掉的语义
许多被广泛依赖的软件都特别注重向后兼容性，就算最初的版本有BUG，也不会在小版本升级中修复。因为有很多第三方软件在开发的时候把BUG当成了FEATURE，要想让第三方软件以后也正常运行，就不得不保持这些“FEATURE”不变。而这类“FEATURE”往往要等到大版本升级的时候才会回归正常。

C#编程语言从1.0发展到现在的6.0，大多数语法/语义保持着向后兼容，但也有例外：

>我们正在采取破坏兼容的变化。在C# 5中，foreach循环的循环变量等效于在循环内部，因此闭包每次会捕获一个新复制的变量。“for”循环中的不会改变。
>
> —— Eric Lippert (2009), *Closing over the loop variable considered harmful*

要理解这个“FEATURE”，首先要深入理解闭包。
因为C#中的相关概念要少一些，为了能把事情解释清楚，以下将从概念更加完备的C++切入，从C++的视角看C#。

## Lambda与闭包
C# 2引入了闭包，在C# 3又可以用Lambda表达式来定义闭包，例如
```csharp
var greetings = "Hello";
Action<string> greet = (name) => { Console.WriteLine($"{greetings} {name}!"); };
greet("World");
```
那一串Lambda表达式```(...)=>{...}```定义了一个“函数”并赋值给了```greet```。给函数加上了引号是因为其中的变量```greetings```不是通过参数传进来的，而是非常霸道地直接捕获。

捕获了外部变量的匿名函数就是闭包。

C#里的Lambda表达式基本上就是```(parameters) => expression-or-statement-block```这种形式，而C++中的Lambda表达式就比C#灵活不少。

## C++中的Lambda与闭包
C++11引入了通过Lambda表达式定义匿名函数的特性，与上面C#代码等效的C++（C++14标准）代码如下：
```cpp
auto greetings = "Hello";
auto greet = [&](auto name) { cout << greetings << " " << name << "!" << endl; };
greet("World");
```
相比于C#的版本，C++版Lambda多了个```[...]```。它表示要捕获的外部变量列表。```[&]```表示所有变量按引用捕获，这种情况下，如果在Lambda内部给捕获变量重新赋值，闭包运行之后，外部变量的值也会跟着改变：
```cpp
auto m = string{"May"};
auto act = [&]() { m = string{"June"}; };

act();

REQUIRE(m == string{"June"});
```

如果不想改变外部变量的值，<del>你不在闭包里改它不就好了吗？</del>可以用```[=]```按值复制一份外部变量给闭包，这样闭包内部对捕获变量的修改，就不会影响到外部变量了：
```cpp
auto m = string{"May"};
auto act = [=]() mutable { m = string{"June"}; };

act();

REQUIRE(m == string{"May"});
```

当按引用捕获外部变量，且外部变量的值会发生变化的时候，闭包的执行时机对结果的影响就很大了：
```cpp
auto m = string{"May"};
auto n = string{""};
auto act = [&] { n = m; };

act();
REQUIRE(n == string{"May"});

m = string{"June"};

act();
REQUIRE(n == string{"June"});
```

当然，如果是按值捕获的话，就无所谓了，因为执行结果在定义闭包的时候就已经确定了：
```cpp
auto m = string{"May"};
auto n = string{""};
auto act = [m, &n] { n = m; };

m = string{"June"};
act();

REQUIRE(n == string{"May"});
```
```[m, &n]```表示```m```按值捕获，```n```按引用捕获。

C++的Lambda表达式设计成这样，其实从二十多年前C++之父写的一本书里就能预见到了：
>许多C++的设计决策根植于我对强迫他人以某些特定方式做事的厌恶。
>历史上，一些最糟的灾难起源于理想主义者试图强迫别人去做“对他们好的事”。
>这种理想主义不仅导致无辜的人遭受煎熬，并且会让采取这些措施的理想主义者产生幻觉和腐败。
>我还发现理想主义者容易忽略那些让教条或理论难堪的经验或实验。
>当理念崩塌，有时甚至专家也认可时，我更愿意为程序员提供选择的空间。
>
>……
>
>人们思考和工作的方式如此多样，试图强行推行单一的风格所带来的伤害会大于收益。
>因此，C++刻意设计成了支持多种风格，而不是只有一种（可能的）“最好的方式”。
>
> —— Bjarne Stroustrup (1994), *The Design and Evolution of C++*

## C#中的闭包
C#的Lambda表达式不支持列出捕获变量的列表```[...]```，那它会按值捕获还是按引用捕获呢？
* A. 按值捕获
* B. 按引用捕获
* C. 都有可能

运行下面的程序就知道答案了：
```csharp
var m = "May";
Action act = () => { m = "June"; };

act();

Assert.True(m == "June");
```
答案是**B**。

C#全部按引用捕获外部变量，就会出现闭包运行结果对运行时机的依赖问题：
```csharp
var m = "May";
var n = "";
Action act = () => { n = m; };

act();
Assert.True(n == "May");

m = "June";

act();
Assert.True(n == "June");
```

看上去没什么毛病，但和循环变量结合在一起……就爆出了微软不惜破坏向后兼容也要改掉的语义。

## 捕获循环变量
如果在```for```循环中定义闭包捕获循环变量，那么每次捕获到的变量会是同一个吗？
* A. 是同一个
* B. 不是同一个
* C. 都有可能

运行下面的程序就知道答案了：
```csharp
var months = new[] {"May", "June"};
var n = "";
var acts = new List<Action>();

for (var i = 0; i < 1; i++)
{
    acts.Add(() => { n = months[i]; });
}
acts[0]();

Check.True(n == "June");
```
答案是**A**。INTERESTING!

如果不想捕获同一个循环变量，<del>可以用C++!</del>可以在循环体中把变量复制一份再传给闭包：
```csharp
for (var i = 0; i < 1; i++)
{
    var t = i;
    acts.Add(() => { n = months[t]; });
}
```

那```foreach```循环呢？
```csharp
var months = new[] {"May", "June"};
var n = "";
var acts = new List<Action>();

foreach (var month in months)
{
    acts.Add(() => { n = month; });
}
acts[0]();

Check.True(n == "June");
```
在C# 5之前的版本，多次循环共用了同一个循环变量，所以……EXCITED!

这就是微软当年不惜破坏向后兼容也要改掉的语义。

到了C# 5，```foreach```循环内部就每次都会复制一个新的循环变量，上面的结果就成了
```csharp
Check.True(n == "May");
```


C++面对这个问题的解决方式就是按值捕获循环变量。
但如果C++也按引用捕获循环变量会怎么样呢?
* A. 结果和C#一样
* B. 结果和C#不一样
* C. 都有可能

运行了下面的程序也不会知道正确答案是什么：
```cpp
auto months = vector<string>{"May", "June"};
auto n = string{""};
auto acts = vector<function<void()>>{};

for (auto i = 0; i < 1; i++)
{
    acts.push_back([&] { n = months[i]; });
}
acts[0]();

UNDEFINED(n == string{"May"});
```

因为```i```的生命周期只在循环内部，捕获之后在循环外部运行属于未定义行为。
所以正确答案是**C**。

## 尾声
微软发现了```foreach```的闭包捕获问题后，考虑过很多修复方案，当年还和C++界的领袖人物Herb Sutter打听过C++里类似问题的解决方案。
但是最后为了兼容，没有对语法进行大改，也没有改```for```循环的语义，只把```foreach```的语义给改正了。

反观C++，大多数新功能一开始都可以通过引用第三方库（如Boost）来使用，通过了实践考验的功能才能进入语言标准，例如```unique_ptr```,```shared_ptr```,和```weak_ptr```。
那些进入了标准库却被实践证明不好用的功能，如```auto_ptr```，则会在大版本升级中逐步废弃（```auto_ptr```在C++11中被标记为deprecated，即不推荐使用，在C++17中被完全移除）。

## 下期预告
看到这里还没有感到头晕的话，可以期待下一篇：
* 从C++的视角看C#（承） — 值类型与引用类型

## 附件
文中提到的代码可以到[CSharpFromTheCppViewpoint
](https://github.com/MiffyLiye/CSharpFromTheCppViewpoint)下载。

## 扩展阅读
* [Eric Lippert (2009), Closing over the loop variable considered harmful, part one](https://ericlippert.com/2009/11/12/closing-over-the-loop-variable-considered-harmful-part-one/)
* [Eric Lippert (2009), Closing over the loop variable considered harmful, part two](https://ericlippert.com/2009/11/16/closing-over-the-loop-variable-considered-harmful-part-two/)
