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
