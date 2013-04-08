--TEST--
Test function fann_set_cascade_candidate_limit() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_set_cascade_candidate_limit( $ann, 950 ) );
var_dump( round( fann_get_cascade_candidate_limit( $ann ), 2 ) == round( 950, 2 ) );

?>
--EXPECT--
bool(true)
bool(true)