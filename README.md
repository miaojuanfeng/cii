# cii
Codeigniter内置框架

### 伪继承

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
