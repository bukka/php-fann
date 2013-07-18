--TEST--
Test function fann_set_cascade_min_cand_epochs() by calling it with its expected arguments
--SKIPIF--
<?php if ( version_compare( FANN_VERSION, "2.2" ) < 0 ) die("Skip: FANN version lower than 2.2"); ?>
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_set_cascade_min_cand_epochs( $ann, 60 ) );
var_dump( fann_get_cascade_min_cand_epochs( $ann ) == 60 );

?>
--EXPECT--
bool(true)
bool(true)