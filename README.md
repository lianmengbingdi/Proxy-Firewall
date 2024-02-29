项目**Proxy Firewall**包含两个部分，分别是客户端程序和服务端程序，二者都必须部署在Linux系统的机器上。

**部署方法：**
方法一：
1. 直接将可执行文件分别移植到客户端和服务端。
2. 打开服务端的8888端口。
3. 将客户端的代理IP设置为服务端IP，端口设置为8888.

方法二：
1. 分别在客户端和服务端编译源代码，编译方式参考项目报告。
2. 接下来同方法一。

**关闭浏览器自动缓存功能：**
打开Firefox浏览器并输入about:config在地址栏中，然后按下回车键。这将打开Firefox的高级配置页面。
在出现的警告页面上，点击"接受风险并继续"按钮。
在搜索栏中输入browser.cache来过滤配置项。
修改以下几个关于缓存的配置项：
browser.cache.disk.enable：将其设置为false，禁用磁盘缓存。
browser.cache.memory.enable：将其设置为false，禁用内存缓存。
browser.cache.offline.enable：将其设置为false，禁用离线缓存。
可以通过双击这些配置项来将其值修改为false。
修改完配置后，关闭并重新启动Firefox浏览器。

**实现的功能：**

1. **用户管理模块**实现了用户的增加、删除、查找和修改等基本功能。通过有效的用户管理，可以降低恶意用户对系统的攻击风险，并提高系统的安全性。可以多用户同时使用。
2. **信息处理模块**则是项目的核心模块，其包括规则控制、消息解析、文件控制、内容过滤和日志缓存等子功能。
3. **图形界面模块**为用户提供了友好和直观的交互界面，使用户能够方便地配置和管理防火墙的各项功能。

**注意点：**
运行客户端软件时，必须先打开服务端程序。
在关闭服务端程序后，需要等待监听套接字自动断开连接后再进行第二次使用，否则程序会自动关闭并退出。

