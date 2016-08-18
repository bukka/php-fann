<html>
<head>
<style>
    .blue{color:blue;}
    .red{color:red;}
</style>
</head>
<body>
<?php

function Pathfinder($ann, $array) {
    $calc_out = fann_run($ann, $array);
    echo "<h1 class='blue'>Testing Pathfinder:</h1>";
    // Display Input Map
    for ($i = 0; $i <= 24; $i++){

        if(abs(round($array[$i]) == 1)) { 
            echo "<span class='red'>" . abs(round($array[$i])) . "</span>"; 
        }else {
            echo abs(round($array[$i]));
        }

        if($i > 0 && ($i % 5) - 4 == 0){
            echo "<br>" . PHP_EOL;
        }
    }
    echo "<h1 class='blue'>Results:</h1>";
    // Display Output Map
    for ($i = 0; $i <= 24; $i++){
        if(abs(round($calc_out[$i]) == 1)) { 
            echo "<span class='red'>" . abs(round($calc_out[$i])) . "</span>"; 
        }else {
            echo abs(round($calc_out[$i]));
        }
        
        if($i > 0 && ($i % 5) - 4 == 0){
            echo "<br>" . PHP_EOL;
        }
    }
}

$train_file = (dirname(__FILE__) . "/pathfinder_float.net");
if (!is_file($train_file)) {
    die('<span class="red">The file pathfinder_float.net has not been created! Please run <a href="pathfinder_train.php">pathfinder_train.php</a> to generate it</span>' . PHP_EOL);
}
   
$pathfinder_ann = fann_create_from_file($train_file);
if ($pathfinder_ann) {
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1));
    Pathfinder($pathfinder_ann, array(0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    Pathfinder($pathfinder_ann, array(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    Pathfinder($pathfinder_ann, array(0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    Pathfinder($pathfinder_ann, array(0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    Pathfinder($pathfinder_ann, array(0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0));
    Pathfinder($pathfinder_ann, array(0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1));
    Pathfinder($pathfinder_ann, array(0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0));
    Pathfinder($pathfinder_ann, array(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0));

    fann_destroy($pathfinder_ann);
} else {
    die('<span class="red">Unable to open Pathfinder.</span>' . PHP_EOL);
}

?>
</body>
</html>