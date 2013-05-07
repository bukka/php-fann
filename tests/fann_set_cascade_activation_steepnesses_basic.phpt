--TEST--
Test function fann_set_cascade_activation_steepnesses() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

$steepnesses = array(0.2, 0.5);

var_dump( fann_set_cascade_activation_steepnesses( $ann, $steepnesses ) );

$rs = fann_get_cascade_activation_steepnesses( $ann );

$sc = count( $steepnesses );
if ( $sc ==  count( $steepnesses ) ) {
	$cmp = 1;
	for ($i = 0; $i < $sc; $i++) {
		$cmp &= (abs( $steepnesses[$i] - $rs[$i] ) < 0.00001);
	}
	var_dump( !!$cmp );
}


?>
--EXPECTF--
bool(true)
bool(true)