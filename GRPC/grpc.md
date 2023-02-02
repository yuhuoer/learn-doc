# GRPC简介

RPC，全称Remote Procedure Call，中文译为远程过程调用。通俗地讲，使用RPC进行通信，调用**远程函数**就像调用**本地函数**一样，RPC底层会做好数据的序列化与传输，从而能使我们更轻松地创建分布式应用和服务。

gRPC 是 RPC的一种，是Google 开源的一个高性能的 RPC(Remote Procedure Call) 框架，它具有如下的优点：

+ 提供高效的进程间通信。gRPC 没有使用 XML 或者 JSON 这种文本格式，而是采用了基于 protocol buffers 的二进制协议；同时，gRPC 采用了 HTTP/2 做为通信协议，从而能够快速的处理进程间通信。
+ 简单且良好的服务接口和模式。gRPC 为程序开发提供了一种契约优先的方式，必须首先定义服务接口，才能处理实现细节。
+ 支持多语言。gRPC 是语言中立的，我们可以选择任意一种编程语言，都能够与 gRPC 客户端或者服务端进行交互。
+ 成熟并且已被广泛使用。通过在 Google 的大量实战测试，gRPC 已经发展成熟。
  
gRPC的典型特征就是使用**protobuf**（全称protocol buffers）作为其**接口定义语言**（Interface Definition Language，缩写IDL），同时底层的消息交换格式也是使用protobuf。
