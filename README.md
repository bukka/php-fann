# PHP FANN wrapper

This is a PHP wrapper for [FANN (Fast Artificial Neural Network) library](http://leenissen.dk/fann/wp/).

## API

The API is documented on http://www.php.net/manual/en/book.fann.php where is the complete documentation for PHP FANN.

The API is very similar to the official [FANN C API](http://leenissen.dk/fann/html/files/fann-h.html). Just functions for fixed `fann_type` have not been mapped because PHP always support `float`. In addition unnecessary arguments for some functions have been left out (for example array length that is not necessary for PHP arrays).

## Installation

The extension can be installed on Linux and Windows.

### Linux

Before you start installation make sure that `libfann` is installed on your system. It's part of the main repository in the most Linux distributions (search for `fann`). If not you need to install it first. Either download it from the [official site](http://leenissen.dk/fann/wp/) or get it from your distro repository. For example on Ubuntu:
```
$ sudo apt-get install libfann-dev
```
Fann installation can be skipped if an RPM for Fedora is used (`libfann` is in the package dependencies).

#### Fedora

The RPM package for PHP FANN is available in Remi's repository: http://rpms.famillecollet.com/

It is available for Fedora, RHEL and clones (CentOS, SC and others).

After downloading remi-release RPM, the package can be installed by executing following command:
```
$ sudo yum --enablerepo=remi install php-pecl-fann
```

#### PECL

This extension is available on PECL. The installation is very simple. Just run:

```
$ sudo pecl install fann
```

#### Manual Installation

It's important to have a git installed as it's necessary for recursive fetch of
[phpc](https://github.com/bukka/phpc).

First clone recursively the repository
```
git clone --recursive https://github.com/bukka/php-fann.git
```

Then go to the created source directory and compile the extension. You need to have a php development package installed (command `phpize` must be available).
```
cd php-fann
phpize
./configure --with-fann
make
sudo make install
```

Finally you need to add
```
extension=fann.so
```
to the php.ini

### Windows

Precompiled binary `dll` libraries for php-fann and libfann are available on [the PECL fann page](http://pecl.php.net/package/fann). The compiled version of libfann is 2.2.

## Examples

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

if ($ann) {
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
if (!$ann)
	die("ANN could not be created");

$input = array(-1, 1);
$calc_out = fann_run($ann, $input);
printf("xor test (%f,%f) -> %f\n", $input[0], $input[1], $calc_out[0]);
fann_destroy($ann);
```

