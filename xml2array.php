<?php
$file = 'xml.xml';
//$file = '/Users/fanhenguang/temp/shenma_media_playing_1.xml';
//$file = '/Users/fanhenguang/temp/xianguoshenma.xml';
echo 'mem start', memory_get_usage(), "\n";
$start = microtime(true);
$ret =  xml2array(file_get_contents($file));
$end = microtime(true);
echo 'mem end', memory_get_usage(), "\n";
echo 'mem peak', memory_get_peak_usage(), "\n";
var_dump($start);
var_dump($end);
var_dump($end-$start);
//var_dump(($end-$start)/10);
var_dump($ret);
