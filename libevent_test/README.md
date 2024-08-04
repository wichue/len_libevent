源码目录include/event2文件夹拷贝过来。

构建目录build/include/event2/event-config.h文件拷贝到include/event2文件夹。

构建目录build/lib文件夹拷贝过来。

如果系统目录已经安装了libevent，编译运行时可能使用了系统目录下的头文件和库文件，此时可配置环境变量或删除系统目录下的libevent。
系统库文件目录：/usr/lib/x86_64-linux-gnu.
系统头文件目录：/usr/include/event2.
