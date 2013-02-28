php_discuz
==========

将discuz的部分功能通过php扩展实现
当前实现了authcode(),扩展的函数为discuz_auth_encode()和discuz_auth_decode()

安装
放置代码于PHP_DIR/ext/discuz
cd PHP_DIR/ext/discuz
phpize
./configure
make
make install

discuz.so文件的位置可能会有所不同，在CentOS6.3测试时，其会安装到/usr/lib/php/modules/
在php.ini配置中开放模块,各发行版可能有所不同，在CentOS6.3
echo "extension=discuz.so" > /etc/php.d/discuz.ini

运行测试用例
php -f EXT_DIR/discuz/discuz.php

--------------
php encode:0766/kmSoi/mGCsXBthrbgWAJvaH57JAPsh+kNWUwS4
php decode:abc
ext encode:031ej2frRqJSMgawQ8XHXCXtWo0u6st2ieBVLMgrSso
ext decode:abc
php2ext decode:abc
ext2php decode:abc
