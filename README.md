php_discuz
==========

将discuz的部分功能通过php扩展实现,当前实现了authcode(),扩展的函数为

    string discuz_auth_encode(string $str, string $key, int $expiry)
    string discuz_auth_decode(string $str, string $key)

安装
------
放置代码于PHP_DIR/ext/discuz

    cd PHP_DIR/ext/discuz 
    phpize
    ./configure
    make
    make install

编译生成discuz.so文件的位置可能会有所不同，在CentOS6.3测试时，其会安装到/usr/lib/php/modules/；
然后，在php.ini配置中开放模块,各发行版可能有所不同，在CentOS6.3中可以下面这样

    echo "extension=discuz.so" > /etc/php.d/discuz.ini

运行测试用例
------

    /PATH/TO/php -f PHP_DIR/ext/discuz/discuz.php

观察互解密情况
------

    php encode:0766/kmSoi/mGCsXBthrbgWAJvaH57JAPsh+kNWUwS4
    php decode:abc
    ext encode:031ej2frRqJSMgawQ8XHXCXtWo0u6st2ieBVLMgrSso
    ext decode:abc
    php2ext decode:abc
    ext2php decode:abc
    
编写插件过程笔记
------
<a href="http://blog.csdn.net/taft/article/details/596291">《如何编写PHP扩展》</a>一文很好，感谢作者的分享。

1）定义扩展要实现和暴露的函数原型，如本案创建名为discuz.def的文件，内容如下。

    string discuz_auth_encode(string str, string key, int expiry)
    string discuz_auth_decode(string str, string key)

2）进入PHP源代码的ext目录，内有名为ext_skel的脚本，该脚本会初始化扩展。
如果提供了第一步的函数原型定义，还会在初始化的扩展代码中自动添加相应的代码段，可以省去一些工作。

    ./ext_skel --extname=discuz --proto=discuz.def

3）如果扩展的逻辑中要用到php已实现的功能算法（如md5,base64），可以搜索PHP源码，查看实现，不过在实践过程中有一些疑问，函数暴露的方式不一致。

如md5

    /* {{{ proto string md5(string str, [ bool raw_output])
    Calculate the md5 hash of a string */
    PHP_NAMED_FUNCTION(php_if_md5)
    {
        ...
    }
    
base64_encode

    /* {{{ proto string base64_encode(string str)
    Encodes string using MIME base64 algorithm */
    PHP_FUNCTION(base64_encode)
    {
        ...
    }
    
    
