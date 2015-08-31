# cii
Codeigniter内置框架

### 简介

### 性能

# 伪继承

出于性能考虑，控制器与模型，不采用继承的方式，避免了多次哈希表合并操作。

控制器，模型类自动伪继承基类CII_Controller、CII_Model，不需要在控制器或模型脚本中显式继承，继承的工作由CII内核完成。脚本位于控制器目录下那它就是一个控制器文件，脚本位于模型目录下那它就是一个模型文件，也就是说不再根据继承的基类来判断是控制器或是模型文件，而是根据文件所在的目录判断。

注意CII_Controller、CII_Model基类实际上是不存在的，CII内核在控制器初始化过程中向控制器、模型注册了一些成员属性与方法，使得看起来控制器、模型像是继承了某个基类。

伪控制器基类定义：

```php
class CII_Controller{
      public $uri     = & object(CII_URI);
      public $router  = & object(CII_Router);
      public $lang    = & object(CII_Lang);
      public $config  = & object(CII_Config);
      public $load    = & object(CII_Load);
      
      public function __construct();
      public function & get_instance();
}
``` 
伪模型基类定义：

```php
class CII_Model{
      public function __construct();
      public function __get();
}
``` 
比如，在控制器目录下编写一个类home:

在定义类时注意不要重载了伪父类的成员属性与方法，如。

# 钩子 - 扩展框架核心

CII 的钩子特性提供了一种方法来修改框架的内部运作流程。CII 的运行遵循着一个特定的流程，你可以参考 CodeIgniter 的应用程序流程图 。但是，有些时候你可能希望在执行流程中的某些阶段添加一些动作，例如在控制器加载之前或之后执行一段脚本，或者在其他的某些位置触发你的脚本。

### 启用钩子

钩子特性可以在 config/config.php 文件中全局的启用或禁用，设置下面这个参数：

```php
$config['enable_hooks'] = TRUE;
```

### 定义钩子

钩子是在 application/config/hooks.php 文件中被定义的，每个钩子可以定义为下面这样的数组格式:

```php
$hook['pre_controller'] = array(
    'class'    => 'MyClass',
    'function' => 'Myfunction',
    'filename' => 'Myclass.php',
    'filepath' => '',
    'initparams' => array('a', 'b', 'c'),
    'funcparams' => array('beer', 'wine', 'snacks')
);
```

#### 注意：

数组的索引为你想使用的挂钩点名称，譬如上例中挂钩点为 <code>pre_controller</code> ， 下面会列出所有可用的挂钩点。钩子数组是一个关联数组，数组的键值可以是 下面这些：

* <code>class</code> 你希望调用的类名，如果你更喜欢使用过程式的函数的话，这一项可以留空。
* <code>function</code> 你希望调用的方法或函数的名称。
* <code>filename</code> 包含你的类或函数的文件名。
* <code>filepath</code> 包含你的脚本文件的目录名。 注意： 你的脚本必须放在 hooks/ 目录里面，所以 <code>filepath</code> 是相对 hooks/ 目录的路径，举例来说，如果你的脚本位于 hooks/myhooks/ ，那么 <code>filepath</code> 可以简单的设置为 'myhooks/' ，如果你的脚本位于 hooks/utilities/myhooks/ ， 那么 <code>filepath</code> 可以设置为 'utilities/myhooks/' ，路径后需要加斜线。如果你的脚本位于 hooks/ ，那么 <code>filepath</code> 可留空或省略此项。
* <code>initparams</code> 你希望传递给你脚本类的构造函数的任何参数，为数组形式，可选。
* <code>funcparams</code> 你希望传递给你脚本函数或方法的任何参数，为数组形式，可选。

### 多次调用同一个挂钩点

如果你想在同一个挂钩点处添加多个脚本，只需要将钩子数组变成二维数组即可，像这样:

```php
$hook['pre_controller'][] = array(
    'class'    => 'MyClass',
    'function' => 'MyMethod',
    'filename' => 'Myclass.php',
    'filepath' => '',
    'initparams' => array('a', 'b', 'c'),
    'funcparams' => array('beer', 'wine', 'snacks')
);

$hook['pre_controller'][] = array(
    'class'    => 'MyOtherClass',
    'function' => 'MyOtherMethod',
    'filename' => 'Myotherclass.php',
    'filepath' => '',
    'initparams' => array('a', 'b', 'c'),
    'funcparams'   => array('red', 'yellow', 'blue')
);
```

注意数组索引后面多了个中括号:

```php
$hook['pre_controller'][]
```

这可以让你在同一个挂钩点处执行多个脚本，多个脚本执行顺序就是你定义数组的顺序。

### 挂钩点

以下是所有可用挂钩点的一份列表：

* <code>pre_system</code> 在系统执行的早期调用，这个时候只有 <code>基准测试类</code> 和 <code>钩子类</code> 被加载了， 还没有执行到路由或其他的流程。
* <code>pre_controller</code> 在你的控制器调用之前执行，所有的基础类都已加载，路由和安全检查也已经完成。
* <code>post_controller_constructor</code> 在你的控制器实例化之后立即执行，控制器的任何方法都还尚未调用。
* <code>post_controller</code> 在你的控制器完全运行结束时执行。
* <code>display_override</code> 覆盖 _display() 方法，该方法用于在系统执行结束时向浏览器发送最终的页面结果。 这可以让你有自己的显示页面的方法。注意你可能需要使用 $this->CII =& cii_get_instance() 方法来获取 CII 超级对象，以及使用 $this->CII->output->get_output() 方法来 获取最终的显示数据。
* <code>cache_override</code> 使用你自己的方法来替代 输出类 中的 _display_cache() 方法，这让你有自己的缓存显示机制。
* <code>post_system</code> 在最终的页面发送到浏览器之后、在系统的最后期被调用。

# 基准测试类

CII 有一个一直都是启用状态的基准测试类，用于计算两个标记点之间的时间差。

> 该类是由系统自动加载，无需手动加载。

另外，基准测试总是在框架被调用的那一刻开始，在输出类向浏览器发送最终的视图之前结束。这样可以显示出整个系统执行的精确时间。

### 使用基准测试类

基准测试类可以在你的 <code>控制器</code>、<code>视图</code> 以及 <code>模型</code> 中使用。

使用流程如下：

1. 标记一个起始点
2. 标记一个结束点
3. 使用 elapsed_time 函数计算时间差。

这里是个真实的代码示例:

```php
$this->benchmark->mark('code_start');

// Some code happens here

$this->benchmark->mark('code_end');

echo $this->benchmark->elapsed_time('code_start', 'code_end');
```

<code>"code_start"</code> 和 <code>"code_end"</code> 这两个单词是随意的，它们只是两个用于标记的单词而已，你可以任意使用其他你想使用的单词，另外，你也可以设置多个标记点。 

看如下示例:

```php
$this->benchmark->mark('dog');

// Some code happens here

$this->benchmark->mark('cat');

// More code happens here

$this->benchmark->mark('bird');

echo $this->benchmark->elapsed_time('dog', 'cat');
echo $this->benchmark->elapsed_time('cat', 'bird');
echo $this->benchmark->elapsed_time('dog', 'bird');
```

### 在 性能分析器 中使用基准测试点

如果你希望你的基准测试数据显示在 <code>性能分析器</code> 中， 那么你的标记点就需要成对出现，而且标记点名称需要以 <code>_start</code> 和 <code>_end</code> 结束， 每一对的标记点名称应该一致。例如:

```php
$this->benchmark->mark('my_mark_start');

// Some code happens here...

$this->benchmark->mark('my_mark_end');

$this->benchmark->mark('another_mark_start');

// Some more code happens here...

$this->benchmark->mark('another_mark_end');
```

### 显示总执行时间

如果你想显示从 CII 运行开始到最终结果输出到浏览器之间花费的总时间，只需简单的将下面这行代码放入你的视图文件中:

```php
<?php echo $this->benchmark->elapsed_time();?>
```

你大概也注意到了，这个方法和上面例子中的介绍的那个计算两个标记点之间时间差的方法是一样的，只是不带任何参数。当不设参数时，CII 在向浏览器输出最终结果之前不会停止计时，所以无论你在哪里使用该方法，输出的计时结果都是总执行时间。

如果你不喜欢纯 PHP 语法的话，也可以在你的视图中使用另一种伪变量的方式来显示总执行时间:

```php
{elapsed_time}
```

如果你想在你的控制器方法中进行基准测试，你需要设置你自己的标记起始点和结束点。

### 显示内存占用

如果你的 PHP 在安装时使用了 <code>--enable-memory-limit</code> 参数进行编译，你就可以在你的视图文件中使用下面这行代码来显示整个系统所占用的内存大小:

```php
<?php echo $this->benchmark->memory_usage();?>
```

这个方法只能在视图文件中使用，显示的结果代表整个应用所占用的内存大小。
如果你不喜欢纯 PHP 语法的话，也可以在你的视图中使用另一种伪变量的方式来显示占用的内存大小:

```php
{memory_usage}
```
# 语言类

语言类提供了一些方法用于获取语言文件和不同语言的文本来实现国际化。

在你的 CII 目录，有一个 language 子目录， 它用于存放一系列的语言文件。你可以创建属于你自己的语言文件，用于提供应用程序的错误消息和其他消息。你另加的消息应该放在 language/ 目录下，每种不同的语言都有相应的一个子目录（例如， 'french' 或者 'german'）。

每个语言都应该有它自己的目录，例如，英语语言文件位于：language/english ，中文语言文件位于：language/chinese。

### 处理多语言

如果你想让你的应用程序支持多语言，你就需要在 language/ 目录下提供不同语言的文件， 然后在 config/config.php 配置文件中指定默认语言。

你应该将你正在使用的语言保存到一个会话变量中。

语言文件的例子

```php
language/
  english/
      error_messages_lang.php
  chinese/
      ...
      email_lang.php
      error_messages_lang.php
      form_validation_lang.php
      ...
```

切换语言

```php
$idiom = $this->session->get_userdata('language');
$this->lang->load('error_messages', $idiom);
$oops = $this->lang->line('message_key');
```

### 国际化

CII 的语言类给你的应用程序提供了一种简单轻便的方式来实现多语言， 它并不是通常我们所说的 国际化与本地化 的完整实现。

我们可以给每一种语言一个别名，一个更通用的名字，而不是使用诸如 "en"、 "en-US"、"en-CA-x-ca" 这种国际标准的缩写名字。当然，你完全可以在你的程序中使用国际标准的缩写名字。

### 使用语言类

#### 创建语言文件

语言文件的命名可以任意，例如，你想创建一个包含错误消息的文件， 你可以把它命名为：error_lang.php 。

在此文件中，你可以在每行把一个字符串赋值给名为 $lang 的数组，例如:

```php
$lang['language_key'] = 'The actual message to be shown';
```

在每个文件中使用一个通用的前缀来避免和其他文件中的相似名称冲突是个好方法。 例如，如果你在创建错误消息你可以使用 error_ 前缀。

```php
$lang['error_email_missing'] = 'You must submit an email address';
$lang['error_url_missing'] = 'You must submit a URL';
$lang['error_username_missing'] = 'You must submit a username';
```

#### 加载语言文件

在使用语言文件之前，你必须先加载它。可以使用下面的代码:

```php
$this->lang->load('filename', 'language');
```

其中 filename 是你要加载的语言文件名（不带.php扩展名），language 是要加载哪种语言（比如，英语）。如果没有第二个参数，将会使用 config/config.php 中设置的默认语言。

你也可以通过传一个语言文件的数组给第一个参数来同时加载多个语言文件。

```php
$this->lang->load(array('filename1', 'filename2'), 'language');
```

language 参数只能包含字母。

#### 读取语言文本

当你的语言文件已经加载，你就可以通过下面的方法来访问任何一行语言文本:

```php
$this->lang->line('language_key');
```

其中，language_key 参数是你想显示的文本行所对应的数组的键名。

该方法只是简单的返回文本行，而不是显示出它。

#### 返回语言文本

如果需要返回语言文本而不是加载它，使用第三个参数is_return，将返回读取的语言文本：

```php
$lang = $this->lang->load(array('filename1', 'filename2'), 'language', 1);
```

变量 <code>$lang</code> 将会是包含了语言文本的数组。

### 自动加载语言文件

如果你发现你需要在整个应用程序中使用某个语言文件，你可以让 CII 在系统初始化的时候 自动加载 该语言文件。 可以打开 config/autoload.php 文件，把语言放在 autoload 数组中。
