# 从C++的视角看C#（零） —— Lambda表达式与闭包

## 微软不惜破坏向后兼容也要改掉的语义
许多被广泛依赖的软件都特别注重向后兼容性，就算最初的版本有BUG，也不会在小版本升级中修复。因为有很多第三方软件在开发的时候把BUG当成了FEATURE，要想让第三方软件以后也正常运行，就不得不保持这些“FEATURE”不变。而这类“FEATURE”往往要等到大版本升级的时候才会回归正常。

C#编程语言从1.0发展到现在的6.0，大多数语法/语义保持着向后兼容，但也有例外：

>我们正在采取破坏兼容的变化。在C# 5中，foreach循环的循环变量等效于在循环内部，因此闭包每次会捕获一个新复制的变量。“for”循环中的不会改变。
>
> —— Eric Lippert，2009

要理解这个“FEATURE”，首先要深入理解闭包——从C++的视角。

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
与上面C#代码等效的C++（C++14标准）代码如下：
```cpp
auto greetings = "Hello";
auto greet = [&](auto name) { cout << greetings << " " << name << "!"; };
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

C++的高度灵活可以用C++之父的一段话总结：
>Many C++ design decisions have their roots in my dislike for forcing people to do things in some particular way.
>In history, some of the worst disasters have been caused by idealists trying to force people into "doing what is good for them".
>Such idealism not only leads to suffering among its innocent victims, but also to delusion and corruption of the idealists applying the force.
>I also find idealists prone to ignore experience and experiment that inconventiently clashes with dogma or theory.
>Where ideals clash and sometimes even when pundits seem to agree, I prefer to provide support that gives the programmer a choice.
>
>I'm not willing to sacrifice people to ideas.
>In particular, I do not try to enforce a single style of design through a narrowly defined programming language.
>People's ways of thinking and working are so diverse that an attempt to force a single style would do more harm than good.
>Thus, C++ is deliberately designed to support a variety of styles rather than a would-be "one true way".
>
> —— Bjarne Stroustrup, 1994, *The Design and Evolution of C++*

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

