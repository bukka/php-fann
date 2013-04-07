--TEST--
Test function fann_get_cascade_candidate_stagnation_epochs() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_get_cascade_candidate_stagnation_epochs( $ann ) );

?>
--EXPECTF--
int(%d)