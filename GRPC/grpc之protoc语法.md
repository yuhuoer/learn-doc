## gRPC之proto语法

https://www.jianshu.com/p/da7ed5914088  
本文描述如何使用proto3语法去构造你的数据结构，对官方文档不完全译文，只是摘出本人需要的部分来简单翻译[官网地址](https://link.jianshu.com/?t=https://developers.google.com/protocol-buffers/docs/proto3)，如果你无法进入官网链接请自行"跳墙"-\_-.

##### 目录

+   1 定义消息类型
    +   1.1 指定字段类型
    +   1.2 分配标量
    +   1.3 指定属性规则
    +   1.4 添加更多的消息类型
    +   1.5 添加注释
    +   1.6 保留属性
+   2 数据类型
+   3 默认值
+   4 枚举
+   5 引用其他的消息类型
    +   5.1 导入其他proto中定义的消息
+   6 内嵌类型
+   8 包
+   9 服务定义
+   10 选项

## 1.定义消息类型

让我们先看一个 proto3 的查找请求参数的消息格式的例子，这个请求参数例子模仿分页查找请求，他有一个请求参数字符串，有一个当前页的参数还有一个每页返回数据大小的参数，proto文件内容如下：

```auto
syntax = "proto3";

message SearchRequest {
  string query = 1;
  int32 page_number = 2;
  int32 result_per_page = 3;
}
```

+   `第一行的含义是限定该文件使用的是proto3的语法，如果没有 syntax = "proto3";`
    
+   `SearchRequest定义有三个承载消息的属性，每一个被定义在SearchRequest消息体中的字段，都是由数据类型和属性名称组成。`
    

### 1.1 指定字段类型

在上面的例子中，所有的属性都是标量，两个整型(page\_number、result\_per\_page)和一个字符串(query)，你还可以在指定复合类型，包括枚举类型或者其他的消息类型。

### 1.2 分配标量

就像所看见的一样，每一个被定义在消息中的字段都会被分配给一个唯一的标量，这些标量用于标识你定义在二进制消息格式中的属性，标量一旦被定义就不允许在使用过程中再次被改变。标量的值在1～15的这个范围里占一个字节编码(详情请参看 [谷歌的 Protocol Buffer Encoding](https://link.jianshu.com/?t=https://developers.google.com/protocol-buffers/docs/encoding#structure) )。

### 1.3 指定属性规则

+   `singular: 一个正确的消息可以有零个或者多个这样的消息属性(但是不要超过一个).`
+   `repeated: 这个属性可以在一个正确的消息格式中重复任意次数(包括零次),`  
    在proto3中，标量数字类型的重复字段默认使用压缩编码

### 1.4 添加更多的消息类型

在一个proto文件中可以定义多个消息类型，你可以在一个文件中定义一些相关的消息类型，上面的例子proto文件中只有一个请求查找的消息类型，现在可以为他多添加一个响应的消息类型，具体如下：

```auto
syntax = "proto3";

message SearchRequest {
  string query = 1;
  int32 page_number = 2;
  int32 result_per_page = 3;
}

message SearchResponse {
    ....
}
```

### 1.5 添加注释

proto文件中的注释使用的是c/c++中的单行注释 `//` 语法风格。  
如下：

```auto
message SearchRequest {
  string query = 1;
  int32 page_number = 2;  // 当前页数
  int32 result_per_page = 3;  // 每页数据返回的数据量
```

### 1.6 保留属性

为了避免在加载相同的.proto的旧版本，包括数据损坏，隐含的错误等，这可能会导致严重的问题的方法是指定删除的字段的字段标签（和/或名称，也可能导致JSON序列化的问题）被保留。 如果将来的用户尝试使用这些字段标识符，协议缓冲区编译器将会报错。

保留字段的使用例子:

```auto
message Foo {
  reserved 2;
  reserved "foo", "bar";
}
```

上述例子定义保留属性为`"foo", "bar"`，定义保留属性位置为2，即在2这个位置上不可以定义属性，如:`string name=2;`是不允许的，编译器在编译proto文件的时候如果发现，2这个位置上有属性被定义则会报错。

## 2 数据类型

一个信息标量具有如下表格所示的数据类型，下表主要是对.proto文件的值类型和java的值类型的对照表

| .proto Type | Java Type |
| --- | --- |
| double | double |
| float | float |
| int32 | int |
| int64 | long |
| uint32 | int |
| uint64 | long |
| sint32 | int |
| sint64 | long |
| fixed32 | int |
| fixed64 | long |
| sfixed32 | int |
| sfixed64 | long |
| bool | boolean |
| string | String |
| bytes | ByteString |

[详情参看官方文档](https://link.jianshu.com/?t=https://developers.google.com/protocol-buffers/docs/proto3#scalar)

## 3 默认值

当proto消息被解析成具体的语言的时候，如果消息编码没包含特定的元素，则消息对象中的属性会被设置默认值，这些默认值具体如下：

+   `string`类型,默认值是空字符串,注意不是null
+   `bytes`类型,默认值是空bytes
+   `bool`类型，默认值是false
+   `数字`类型,默认值是0
+   `枚举`类型,默认值是第一个枚举值,即0
+   `repeated`修饰的属性，默认值是空（在相对应的编程语言中通常是一个空的list）.

## 4 枚举

proto允许你在定义的消息类型的时候定义枚举类型，如下例,在消息类型中定义并使用枚举类型：

```auto
message SearchRequest {
  string query = 1;
  int32 page_number = 2;
  int32 result_per_page = 3;
  enum Corpus {
    UNIVERSAL = 0;
    WEB = 1;
    IMAGES = 2;
    LOCAL = 3;
    NEWS = 4;
    PRODUCTS = 5;
    VIDEO = 6;
  }
  Corpus corpus = 4;
}
```

如上例中所示，`Corpus`枚举类型的第一个枚举值是0，每一个枚举值定义都会与一个常量映射，而这些常量的第一个常量值必须为0，原因如下：

+   `必须有一个0作为值，以至于我们可是使用0作为默认值`
+   `第一个元素的值取0，用于与第一个元素枚举值作为默认值的proto2语义兼容`
    
    枚举类型允许你定义别名，别名的作用是分配不中的标量，使用相同的常量值，使用别名只需要在定义枚举类型的第一行中添加`allow_alias`选项，并将值设置为true即可，如果没有设置该值就是用别名，在编译的时候会报错。
    
    官网例子如下：
    

```auto
enum EnumAllowingAlias {
  option allow_alias = true;
  UNKNOWN = 0;
  STARTED = 1;
  RUNNING = 1;
}
enum EnumNotAllowingAlias {
  UNKNOWN = 0;
  STARTED = 1;
  //如果解除这个注释编译器在编译该proto文的时候会报错
  // RUNNING = 1;  
}
```

proto支持的枚举值的范围是32位的整形，即Java 中的int类型,其他请参看官网。

## 5 引用其他的消息类型

你可以在定义消息类型的时候饮用其他已经定义好的消息类型作为新消息类型的属性，官网例子如下：

```auto
message SearchResponse {
  repeated Result results = 1;
}

message Result {
  string url = 1;
  string title = 2;
  repeated string snippets = 3;
}
```

在上面的消息例子中，SearchResponse这个响应消息类型的属性results，返回的是一个Result类型的消息列表。

### 5.1 导入其他proto中定义的消息

在上面的例子中，Result和SearchResponse消息类型被定义在同一个.proto文件中，如果把他们分成两个文件定义，应该如何引用呢？

proto中为我们提供了`import` 关键字用于引入不同`.proto`文件中的消息类型,你可以在你的`.proto`文件的顶部加入如下语句因为其他`.proto`文件的消息类型：  
`import "myproject/other_protos.proto";`  
例子：

+   文件名称`search_response.proto`

```auto
syntax = "proto3";
import "test/result.proto";
package test1;

message SearchResponse {
  //包名.消息名
  repeated test2.Result results = 1;
}
```

+   文件名称`result.proto,在与search_response.proto同级目录的test下`

```auto
syntax = "proto3";
package test2;

message Result {
  string url = 1;
  string title = 2;
  repeated string snippets = 3;
}
```

如果两个.proto文件在同一个目录下直接这样`import "result.proto";`倒入即可。

## 6 内嵌类型

我们还可以在消息类型中定义消息，例子如下：

```auto
message SearchResponse {
  message Result {
    string url = 1;
    string title = 2;
    repeated string snippets = 3;
  }
  repeated Result results = 1;
}
```

在上面的例子中在SearchResponse消息体中定义了一个Result消息并使用。

如果想在其他的消息体引用Result这个消息，可以`Parent.Type`这样引用，例子：

```auto
message SomeOtherMessage {
  SearchResponse.Result result = 1;
}
```

消息还可以深层的嵌套定义，如下例子：

```auto
message Outer {                  // Level 0
  message MiddleAA {  // Level 1
    message Inner {   // Level 2
      int64 ival = 1;
      bool  booly = 2;
    }
  }
  message MiddleBB {  // Level 1
    message Inner {   // Level 2
      int32 ival = 1;
      bool  booly = 2;
    }
  }
}
```

## 7 Map

proto支持map属性类型的定义，语法如下：  
`map<key_type,value_type> map_field = N;`  
key\_type可以是任何整数或字符串类型（除浮点类型和字节之外的任何标量类型,枚举类型也是不合法的key类型），value\_type可以是任何类型的数据。

map更具体的使用方式参看[API](https://link.jianshu.com/?t=https://developers.google.com/protocol-buffers/docs/reference/overview)

## 8 包

可以为`proto`文件指定包名，防止消息命名冲突。

例子如下：

```auto
package foo.bar;
message Open { ... }
```

当你在为消息类型定义属性的时候，你可以通过`命名.类型`的形式来使用已经定义好的消息类型，如下：

```auto
Message Foo {
  ...
  foo.bar.Open open = 1;
  ...
}
```

## 9 服务定义

如果你想在RPC中使用已经定义好的消息类型，你可以在`.proto`文件中定一个消息服务接口,protocol buffer编译器会生成对应语言的接口代码。

+   `接口定义例子：`

```auto
service SearchService {
    //  方法名  方法参数                 返回值
    rpc Search(SearchRequest) returns (SearchResponse); 
}
```

## 10 选项

下面只列出java的`.proto`文件常用的一下选贤，其他选项前参看[官网文档](https://link.jianshu.com/?t=https://developers.google.com/protocol-buffers/docs/proto3#options)  

+   `java_package`（文件选项）：指定生成的java类所在的包， 如果在.proto文件中没有提供明确的`java_package`选项，那么默认情况下，将使用`proto`包。如果没有生成java代码该选项默认是不生效的。  
    `option java_package = "org.example.foo";`
    
+   `java_multiple_files`（文件选项）：指定在proto文件中定义的所有消息、枚举和服务在生成java类的时候都会生成对应的java类文件，而不是以内部类的形式出现。  
    `option java_multiple_files = true;`
    
+   `java_outer_classname`（文件选项）：指定生成的java类文件名称，如果不指定则会默认使用`.proto`文件的文件名称，如果没有生成java类文件，则该选项不会生效 <span id="1">Hello World</span>。  
    `option java_outer_classname = "HelloWorld";`
    
