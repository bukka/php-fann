<style>
    .blue{color:blue;}
    .green{color:green;}
    .red{color:red;}
</style>

<h1 class='blue'>OCR Test:</h1>
<?php

function OCR($img, $expected, $input, $lookup_array, $ann) {
    echo "Image: <img src='images/$img'><br>" . PHP_EOL;

    $calc_out = fann_run($ann, $input);
    
    echo 'Raw: ' .  $calc_out[0] . '<br>' . PHP_EOL;
    echo 'Trimmed: ' . floor($calc_out[0]*100)/100 . '<br>' . PHP_EOL;
    echo 'Decoded Symbol: ';
    
    for($i = 0; $i < count($lookup_array); $i++) {
       if( floor($lookup_array[$i][0]*100)/100 == floor($calc_out[0]*100)/100) {
            echo $lookup_array[$i][1] . '<br>' . PHP_EOL;
            echo "Expected: $expected <br>" . PHP_EOL;
            echo 'Result: ';
            if($expected == $lookup_array[$i][1]){
                echo '<span class="green">Correct!</span>';
            }else{
                echo '<span class="red">Incorrect!</span> <a href="train_ocr.php">Retrain OCR</a>';
            }
        }
    }
    echo '<br><br>' . PHP_EOL;
    
}


$train_file = (dirname(__FILE__) . '/ocr_float.net');
if (!is_file($train_file))
    die('<span class="red">The file ocr_float.net has not been created!</span><a href="train_ocr.php">Train OCR</a>' . PHP_EOL);

$ocr_ann = fann_create_from_file($train_file);
if ($ocr_ann) {
    
    $result_lookup_array = array();
    $curr = 0.00;
    for($i = 33; $i <= 126; $i++) {
        array_push($result_lookup_array, array($curr, chr($i)));
        $curr+= 0.01;
    }
    
    // Pixel data for F images/32.png
    /*
    0000000000
    0000000000
    0000000000
    0111111110
    0110000000
    0110000000
    0110000000
    0111111000
    0110000000
    0110000000
    0110000000
    0110000000
    0110000000
    0000000000
    0000000000
    0000000000
    */
    $test_F = array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
    // Pixel data for A images/37.png
    /*
    0000000000
    0000000000
    0000000000
    0000110000
    0001111000
    0011001100
    0110000110
    0110000110
    0110000110
    0111111110
    0110000110
    0110000110
    0110000110
    0000000000
    0000000000
    0000000000
    */
    $test_A = array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
    // Pixel data for N images/45.png
    /*
    0000000000
    0000000000
    0000000000
    0110000110
    0111000110
    0111100110
    0111100110
    0110110110
    0110110110
    0110011110
    0110001110
    0110001110
    0110000110
    0000000000
    0000000000
    0000000000
    */
    $test_N = array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    OCR('37.png', 'F', $test_F, $result_lookup_array, $ocr_ann);
    OCR('32.png', 'A', $test_A, $result_lookup_array, $ocr_ann);
    OCR('45.png', 'N', $test_N, $result_lookup_array, $ocr_ann);
    OCR('45.png', 'N', $test_N, $result_lookup_array, $ocr_ann);
    
    fann_destroy($ocr_ann);
} else {
    die("<span class='red'>Invalid file format.</span>" . PHP_EOL);
}

?>