# php7-extension-xml-to-array

## php xml2array(xml to array) by php extension

this is a php extension for xml2array(xml to array)。 
 php xml to array 扩展实现， 即通过扩展的方式实现xml转换为array

## 版本
适用于php7

## 安装方式
```c
phpize 
export CFLAGS="-lxml2 -I/usr/include/libxml2"
./configure
make 
sudo make install
```

## 使用
```php
1 <?php
2 $file = 'xml.xml';
3 $content = file_get_contents($file);
4
5 $ret = xml2array($content);
6 var_dump($ret);

```
xml 内容为：

```xml
<?xml version="1.0" encoding="utf-8"?>
<root>
    <xx>
        <id>3</id>
        <id>4</id>
        <ee>
            <rr>444</rr>
        </ee>
    </xx>

    <xx>
        <ee>
            <rr>444</rr>
            <rr>444</rr>
        </ee>
        <ee>
            <rr>444</rr>
        </ee>
    </xx>
</root>

```


输出如下：

```
array(1) {
  ["root"]=>
  array(1) {
    ["xx"]=>
    array(2) {
      [0]=>
      array(2) {
        ["id"]=>
        array(2) {
          [0]=>
          string(1) "3"
          [1]=>
          string(1) "4"
        }
        ["ee"]=>
        array(1) {
          ["rr"]=>
          string(3) "444"
        }
      }
      [1]=>
      array(1) {
        ["ee"]=>
        array(2) {
          [0]=>
          array(1) {
            ["rr"]=>
            array(2) {
              [0]=>
              string(3) "444"
              [1]=>
              string(3) "444"
            }
          }
          [1]=>
          array(1) {
            ["rr"]=>
            string(3) "444"
          }
        }
      }
    }
  }
}
```



