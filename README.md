# The PHP FANN (Fast Artificial Neural Network) Extension

This is a PHP binding for [FANN (Fast Artificial Neural Network) library](http://leenissen.dk/fann/wp/).

## API

The API is very similar to the official [FANN C API](http://leenissen.dk/fann/html/files/fann-h.html). Just functions for fixed `fann_type` have not been mapped because PHP always support `float`. In addition unnecessary arguments for some functions have been left out (for example array length that is not necessary for PHP arrays).

## Example

These are just two basic examples for simple training and running supplied data on the trained network.

### `simple_train.php`

```php
$num_input = 2;
$num_output = 1;
$num_layers = 3;
$num_neurons_hidden = 3;
$desired_error = 0.001;
$max_epochs = 500000;
$epochs_between_reports = 1000;

$ann = fann_create_standard($num_layers, $num_input, $num_neurons_hidden, $num_output);

if (is_resource($ann)) {
    fann_set_activation_function_hidden($ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output($ann, FANN_SIGMOID_SYMMETRIC);

    $filename = dirname(__FILE__) . "/xor.data";
    if (fann_train_on_file($ann, $filename, $max_epochs, $epochs_between_reports, $desired_error))
        fann_save($ann, dirname(__FILE__) . "/xor_float.net");

    fann_destroy($ann);
}
```
### `simple_test.php`

```php
$train_file = (dirname(__FILE__) . "/xor_float.net");
if (!is_file($train_file))
    die("The file xor_float.net has not been created! Please run simple_train.php to generate it");

$ann = fann_create_from_file($train_file);
$input = array(-1, 1);
$calc_out = fann_run($ann, $input);
printf("xor test (%f,%f) -> %f\n", $input[0], $input[1], $calc_out[0]);
fann_destroy($ann);
```