<?php
	class Home{
		public function __construct(){
			//parent::__construct();
		}
		public function index(){
			echo "Home::index()\n";
		}
		public function loadview(){
			$data = array('name'=>"title", 'content'=>"content");
			$this->load->view("index", $data);
		}
		public function getview($view,$data){
			//xdebug_debug_zval('data');
			//$this->load->view($view,$data,1);
			//xdebug_debug_zval('data');
			//$this->load->helper(array('array_helper','array_helper'));
			//$helper = 'array_helper';
			//$this->load->helper($helper);
			//array_hello();
			//echo $demo_var;
			//xdebug_call_class();
			//xdebug_call_function();
			//xdebug_get_declared_vars();
			//idebug_function_args();
			//idebug_function_call_stack();
		}
		public function gettitle(){
			//$this->lang = new cii_lang();
			$this->lang->load("title_lang");
			$this->lang->load("title_lang","english");
			$this->lang->load("title_lang","chinese");
			idebug_included_files();
			echo $this->lang->line("title");
		}
		public function getmodel(){
			var_dump($this->load->model("sample","rename"));
		}
	}
	/*class father{
		public $name = "father";
	}
	class son extends father{
		public $age;
	}
	$s = new son();
	echo $s->age;*/
	//cii_run();
	//$app = new Home();
	//var_dump($app->lang);
	//$app->model("sample");
	//$abb = new load();
	//$abb->view("index", array("name"=>"news","content"=>"content"));
/*var_dump(get_class_vars("CII_Controller"));
var_dump(get_class_vars("CII_Controller"));
var_dump(get_class_vars("CII_Controller"));
var_dump(get_class_vars("CII_Controller"));
var_dump(get_class_vars("CII_Controller"));*/
/*var_dump($app->segment_array());
var_dump($app->rsegment_array());
var_dump($app->total_segments());
var_dump($app->total_rsegments());*/
	//$app = new Home();
	//$app->gettitle();
	//var_dump($app);
	//$pp = &cii_get_config(array('url_suffix'=>'fuck your mother'));
	//$bb = new cii_config();
	//$bb->config["index_page"] = "asdasd";
	//var_dump($bb->slash_item("base_url"));
	//var_dump($bb->slash_item("index_page"));
	//$tt = &$bb->config;
	//$tt['base_url'] = "localhost";
	//var_dump(cii_is_https());
	//var_dump($bb->config);
	//$aa = &cii_get_config();
	//xdebug_debug_zval('aa');
	//$tt = &cii_get_config();
	//xdebug_debug_zval('tt');
	//$cc = &cii_get_config();
	//xdebug_debug_zval('cc');
	//var_dump($cc);
	//$app->gettitle();
	//$app->getmodel();
	//this is a memory leak, pls fix it!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	/*//2015-08-11 
	$title = 'miaojuanfeng';
	$content = 'aasd';
	$data = array(
			'name'=>$title,
			'content'=>&$content);
	//xdebug_debug_zval('title');
	//xdebug_debug_zval('content');
	//xdebug_debug_zval('data');
	//var_dump($app->load->view('index',$data));
	//xdebug_debug_zval('title');
	//xdebug_debug_zval('content');
	//xdebug_debug_zval('data');
	//$app->getview('index',$data);
	//var_dump($data);
	//var_dump(Home::get_instance());
	//$app->getmodel();
	$app->load->database();
	//xdebug_debug_zval('cc');
	//$app->db->connect("127.0.0.1","root",NULL,NULL);
	//$app->db->query("insert into user(user,pwd,name,createdate,modifydate) values('miaojuanfeng','19921030','Michael',now(),now())");
	$result = $app->db->query("select * from user");
	//var_dump($result);
	if($rs = $result->fetch_array()){
		echo $rs['name'];
	}
	*/
	
	//phpinfo();
	//cii_test();
	/*$rtv = &cii_get_config(array("asda"));
	$rt = &cii_get_config();
	xdebug_debug_zval('rtv');
	echo $rtv[0];*/
	/*$a = new cii_config();
	//cii_get_config(array('base_url' => 'localhost'));
	$a->set_item("base_url","localhost");
	$a->set_item("language","chinese");
	$a->set_item("proxy_ips","miaojuanfeng");
	var_dump($a);*/

	
	/*$app->load->model("sample");
	var_dump($app);
	var_dump(get_instance());
	$bb = &Home::get_instance();
	xdebug_debug_zval('bb');
	$cc = &get_instance();
	xdebug_debug_zval('cc');*/

	//idebug_class_table();
	//$conn = new mysqli("127.0.0.1","root","");
