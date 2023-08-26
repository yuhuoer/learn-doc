#### FastDFS是什么

Fastdfs是使用C语言编写的高性能分布式文件系统。

用来对文件进行管理，功能包括文件同步，文件访问，解决大容量存储和负载均衡的问题。Fastdfs考虑了冗余备份，负载均衡，在线扩容等机制。

fastdfs特别适合中小文件为载体的在线服务，如相册网站、视频网站、电商网站。

#### Fastdfs原理

fastdfs系统分为三个角色，跟踪服务器（tracker server）,存储服务器（storage server），客户端（client）。

##### tracker server

- 主要负责协调调用工作，并对storage server起到负载均衡的作用
- 负责管理所有的storage server和group，每个storage server启动后会连接tracker，并告知自己的所属group信息，并保持周期性心跳（报告状态，包括磁盘空间，文件同步状态，文件上传下载次数统计等信息）。
- tracker server可以有多台，tracker server之间互相平等，客户端请求tracker server采用轮询的方式，如果请求的tracker server无法提供服务，则换一台请求。

##### storage server

- 主要提供容量和备份服务
- 以group为单位，每一个group里有多个storage server,数据互为备份，各个group互相独立。
- 采用分组存储方式好处是灵活和可控性强，比如上传文件可以由客户端指定，也可以由tracker server进行调度选择。
- 一个group的存储压力过大，可以改组增加存储服务器扩充服务能力，当容量不足时，可以增加组扩充容量。

##### client

上传下载的数据服务器

<img src="./assets/v2-670cf3cf0e1a6f138977af28fe6e7ee7_r.jpg" alt="img" style="zoom:67%;" />

#### 存储策略

冗余备份：为了支持大容量，存储节点采用分组的组织方式，存储系统由一个或多个组组成，组与组之间文件相互独立，所有组加起来就是存储系统的容量。一个组可以由一个或者多个存储服务器组成，一个组下的存储服务器文件都是相同的，组中的多台服务器起到了冗余备份和负载均衡的作用。

动态扩容：在组中新增加服务器时，同步已有的文件由系统自动完成，同步完成后，系统将自动将新增服务器切换到线上提供服务，当存储空间不足或者消耗完时，可以动态加组，只需要增加一台或者多台服务器，并将他们组成一个新组，这样就扩大了存储系统的容量。



#### 文件上传流程

<img src="./assets/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQyOTUzNTI5,size_16,color_FFFFFF,t_70.png" alt="img"  />

1. tracker server收集storage server的状态信息

   storage server定期向tracker server发送磁盘剩余空间，文件同步状况，上传和下载次数的能够信息。因此tracker server中存有各个storage server的状态信息。

2. 选择tracker server

   当集群中不止一个·tracker server时，由于tracker server都是对等的，因此客户端在upload文件时可以任意选择一个tracker server进行上传。

3. 选择存储group

   当tracker server接收到upload文件的请求时，会为该文件分配一个可以存储该文件的group。

   （分配策略：1.所有group轮询 2.指定某一group 3.选择空间更多的group）

4. 选择storage server

   选择group后，tracker server会在group内选择一个storage server给客户端。（分配策略：1.在group中的storage轮询 2.按ip进行排序 3.按优先级选择）

5. 选择storage path

   当分配好storage server后，客户端要向storage发送写文件请求，storage将会分配一个存储目录。（分配策略：1.多个存储目录轮询 2.剩余空间最多优先）

6. 生成文件名

   选定文件存储目录后，storage会为文件生成一个文件名称，由源storage server ip，文件创建时间，文件大小，文件crc32和一个随机数拼接而成，然后将这个二进制串进行base64编码，转换为字符串。（crc32：循环冗余校验，用于检测数据损坏的错误检测码。这里将文件转换为8个字符的字符串用作哈希函数。）

7. 选择两级目录

   当选定存储目录后，storage会为文件分配一个file id，每个存储目录下由两级256*256的子目录，storage会按照文件名称进行两次哈希，路由到其中一个目录下，然后将文件存储在该子目录下。

8. 生成file id

   当文件存储到文件目录后，即认为文件存储成功，接下来就会为该文件生成一个文件id，文件id由group,存储目录，两级目录，文件名和文件后缀名拼接醉成。将文件的file id返回给客户端，

9. 客户端保存该文件的存储信息。

#### 文件同步分析

写文件时，客户端将问及那写到group的一个storage server即认为写入成功，storage  server写完文件后，会由后台线程将文件同步到同group的其他storage server中。

每个storage server写文件后，同时会写一份binlog, binlog中不包含文件数据，只包含文件名信息。binlog用于同步，storage会记录group内其他storage同步的进度，以便重启之后接着上次的进度继续同步，进度以时间戳的方式记录，所以最好将集群中所有server的时钟保持同步。

storage的同步进度会作为元数据的一部分汇报到tracker上，tracker在选择读storage的时候会将已同步进度作为参考。

#### 文件下载流程

![img](./assets/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQyOTUzNTI5,size_16,color_FFFFFF,t_70-1692693139982-5.png)

1. tracker server收集storage server的状态信息

   storage server定期向tracker server发送磁盘剩余空间，文件同步状况，上传和下载次数的能够信息。因此tracker server中存有各个storage server的状态信息。

2. 选择tracker server

   当集群中不止一个·tracker server时，由于tracker server都是对等的，因此客户端在upload文件时可以任意选择一个tracker server进行下载。

3. 选择可用的storage server

   客户端下载请求给某个tracker，必须附带上传文件名信息。tracker从文件名中解析group、路径信息、大小信息、创建信息、源storage server ip等信息。然后选择一个storage进行服务读请求。返回该storage的ip和端口。

   由于group同步是后台的一个异步线程进行的，所以可能出现在读的时候文件还没有同步，为了避免这种情况，tracker会按照如下规则选择group中可读storage:

   - 该文件上传的源storage只要存活，选择该节点。文件名信息中包含源storage的IP信息。
   - 当前时间-文件创建时间>文件同步最大时间。即认为经过文件最大同步时间后，文件肯定完成同步。
   - 文件创建时间<文件同步时间戳，即文件同步时间戳之前的文件已经同步。
   - 当前时间-文件创建时间>同步延迟阈值，即经过同步延迟阈值，认为文件肯定同步了。

4. 访问storage中的文件

   找到对应storage中的目录和文件名，查找对应的文件，返回。



#### 优缺点

优点：

缺点：

- 通过API下载，存在单点的性能瓶颈。
- 不支持断点续传，不适用于大文件的传输
- 同步机制不校验文件正确性



#### Fastdfs怎么实现负载均衡的

- Tracker会记录每个group内每个storage server的负载状况，如当前磁盘利用率、带宽利用率等。
- 客户端上传或下载文件时，会先询问tracker寻找一个负载较低的group（分配策略可以进行配置）
- 选定group后会在group内选择一个storage server给客户端，可以按照优先级排序（比如选择带宽高的storage，分配策略可以进行配置）
- 选择storage后，客户端向storage发送写文件请求，storage将为文件分配一个数据存储目录，可以选择剩余空间多的优先（分配策略可以进行配置）



#### 为什么要搭配使用nginx

Fastdfs作为分布式文件系统，主要功能是提供高容量的文件存储。它本身没有提供HTTP/HTTPS协议的访问接口，无法直接响应浏览器或者客户端的HTTP/HTTPS请求。

解决方案是使用Nginx这样的反向代理服务器来实现对文件的访问。

1. 对tracker做负载均衡

   为了承载更高的并发量，tracker可以做成集群，通过nginx可以实现负载均衡。

2. 避免同步延迟带来的错误

   文件存放在  



代理方式：

##### **正向代理**

正向代理部署在客户端，由客户端使用。隐藏客户端真实IP地址，代替客户端对外发送请求，实现对客户端的保护和访问控制。

工作原理：

1. 客户端将一个请求发送给正向代理服务器
2. 代理服务器封装并隐藏客户端IP地址信息
3. 代理服务器向真实服务器发送已封装的请求
4. 服务器处理完毕返回结果给代理服务器
5. 代理服务器将结果传回客户端。

主要功能:

1. 缓存响应数据，减少重复对远程服务器的访问。
2. 隐藏真实IP，增强客户端的隐秘性
3. 过滤限制请求，实现基于客户端的访问控制。



![img](./assets/v2-a8d7629e1edfba8749a552c22badffc1_r.jpg)

##### **反向代理**

反向代理（Reverse Proxy）：反向代理部署在服务器端，由服务器使用，它代替多个后端真实服务器对外服务，对外暴露反向代理服务器的地址。
工作原理：

1. 客户端向反向代理服务器发送请求
2. 反向代理服务器根据负载均衡或者负载分发规则，将请求转发给后端真实服务器集群某个服务器。
3. 后端真实服务器处理完请求后，将结果返回给反向代理服务器。
4. 反向代理服务器进一步返回结果到客户端。

主要功能：

1. 负载均衡：将负载分摊到各个后端服务器上，提高并发量和可用性。
2. 动静分离：分开处理静态文件和动态请求，减轻后端服务器资源压力
3. 高可用：当后端某服务节点挂掉后，通过代理迅速切换到另一服务节点。

<img src="./assets/v2-6976b25cdb5c0fbaf99bf6ed8120b9e3_r.jpg" alt="img" style="zoom: 80%;" />

两种的区别：

1. 位置不同
   - 正向代理架设在客户端，为客户端屏蔽真实服务器的地址，服务器和客户端只能看到代理服务器的地址。
   - 反向代理架设在服务端，
2. 代理对象不同
   - 正向代理代理客户端，服务端不知道实际发送请求的客户端
   - 反向代理代理服务端，客户端不知道实际提供服务的服务端。
3. 目的不同
   - 正向代理用于客户端资源请求，保护和隐藏客户端真实IP。
   - 反向代理用于服务器端资源访问，负载均衡，动静分离，增强系统可用性。









#### HDFS

HDFS: hadoop分布式文件存储系统。

适用场景：

1. 适合大数据文件：处理数据达到GB、TB甚至PB级别的数据，能够处理百万规模以上的文件数量。
2. 文件分块存储，适合批处理：HDFS将大文件分块，将一个完整的大文件平均分块存储到不同的计算机上，可以同时从多个主机读取不同区域的文件。
3. 高容错性：数据自动保存多个副本，通过增加副本的方式提高容错性。某一个副本丢失后可以自动恢复。
4. 流失文件（无结构文件）访问：一次写入，多次读取。文件一旦写入不能修改，只能追加，能保证数据的一致性。
5. 支持廉价硬件：能够应用在普通的廉价PC机器上。

不适用场景：

1. 不适合低延时数据访问，比如ms级的存储数据(hadoop的map/reduce执行会产生大量的中间数据，节点之间交换大量的数据，从而导致巨大的磁盘IO延迟，它必须在磁盘中保留大量数据以实现阶段之间的同步，以便它可以支持作业从故障中恢复)
2. 无法高效的对大量小文件进行存储
   - 存储大量小文件会占用NameNode大量的内存来存储文件目录和块信息。
   - 小文件存储的寻址时间会超过读取时间，违反了HDFS的设计目标。
3. 不支持并发写入，不支持文件随机修改。（一个文件只能有一个写，不允许多个线程同时写）

<img src="./assets/watermark,type_ZHJvaWRzYW5zZmFsbGJhY2s,shadow_50,text_Q1NETiBA5qKm6YeMQ29kaW5n,size_19,color_FFFFFF,t_70,g_se,x_16.png" alt="img" style="zoom:80%;" />

HDFS 采用的是master/slaves主从结构模型来管理数据，这种结构模型主要由四个部分组成：Client(客户端)、Namenode(名称节点)、Datanode(数据节点)和SecondaryNamenode(第二名称节点，辅助Namenode)。

NameNode（nn）：就是Master，它是一个主管、管理者。
（1）管理HDFS的名称空间；
（2）配置副本策略；
（3）管理数据块（Block）映射信息；
（4）处理客户端读写请求。

DataNode：就是Slave。NameNode下达命令，DataNode执行实际的操作。
（1）存储实际的数据块；
（2）执行数据块的读/写操作。

Client：就是客户端。
（1）文件切分。文件上传HDFS的时候，Client将文件切分成一个一个的Block，然后进行上传；
（2）与NameNode交互，获取文件的位置信息；
（3）与DataNode交互，读取或者写入数据；
（4）Client提供一些命令来管理HDFS，比如NameNode格式化；
（5）Client可以通过一些命令来访问HDFS，比如对HDFS增删查改操作；

Secondary NameNode：并非NameNode的热备。当NameNode挂掉的时候，它并不能马上替换NameNode并提供服务。
（1）辅助NameNode，分担其工作量，比如定期合并Fsimage和Edits，并推送给NameNode ；
（2）在紧急情况下，可辅助恢复NameNode。