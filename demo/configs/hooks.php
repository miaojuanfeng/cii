<?php

$hook['pre_controller'][] = array(
    'class'    => 'MyClass1',
    'function' => 'MyMethod1',
    'filename' => 'Myclass.php',
    'filepath' => '',
    'initparams' => array('cbeer', 'cwine', 'csnacks'),
    'funcparams' => array('beer1', 'wine1', 'snacks1')
);
$hook['pre_controller'][] = array(
    'class'    => 'MyClass1',
    'function' => 'MyMethod2',
    'filename' => 'Myclass.php',
    'filepath' => '',
    'initparams' => array('cbeer', 'cwine', 'csnacks'),
    'funcparams'   => array('beer2', 'wine2', 'snacks2')
);
$hook['pre_controller'][] = array(
    'function' => 'MyFunction',
    'filename' => 'Myclass.php',
    'funcparams'   => array('func: 1123', 'func: wine2', 'func: snacks2')
);

$hook['post_controller_constructor'][] = array(
    'function' => 'MyFunction',
    'filename' => 'Myclass.php',
    'funcparams'   => array('post_controller_constructor\n', 'post_controller_constructor\n', 'func: snacks2\n')
);

$hook['post_controller'] = array(
    'class'    => 'MyClass2',
    'function' => 'MyMethod',
    'filename' => 'Myclass.php',
    'filepath' => '',
    'initparams' => array('cbeer1111', 'cwine1111', 'csnacks1111'),
    'funcparams'   => array('red', 'yellow', 'blue')
);
