--TEST--
Test FANNConnection class
--FILE--
<?php
$fc = new FANNConnection(3, 22, 0.5);
var_dump($fc->getFromNeuron());
var_dump($fc->getToNeuron());
var_dump($fc->getWeight());
$fc->setWeight(0.2);
var_dump($fc->getWeight());

?>
--EXPECT--
int(3)
int(22)
float(0.5)
float(0.2)
