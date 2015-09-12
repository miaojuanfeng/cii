<?php
if( !class_exists("MyClass1") ){
	class MyClass1{
		public function __construct($a1, $a2, $a3){
			echo "MyClass1 __construct()\n";
			echo "construct: ".$a1."\n";
			echo "construct: ".$a2."\n";
			echo "construct: ".$a3."\n";
		}
		public function MyMethod1($a1, $a2, $a3){
			echo $a1."1\n";
			echo $a2."1\n";
			echo $a3."1\n";		
		}
		public function MyMethod2($a1, $a2, $a3){
			echo $a1."2\n";
			echo $a2."2\n";
			echo $a3."2\n";	
		}
	}
}
if( !class_exists("MyClass2") ){
	class MyClass2{
		public function __construct($a){
			echo "MyClass2 __construct()\n";
		}
		public function MyMethod($a1, $a2, $a3){
			echo $a1;
			echo $a2;
			echo $a3;		
		}
	}
}
if( !function_exists("MyFunction") ){
	function MyFunction($a1, $a2, $a3){
		echo $a1;
		echo $a2;
		echo $a3;
	}
}
