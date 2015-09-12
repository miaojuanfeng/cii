<?php
define("BASEPATH","AAA");
//$app = &cii_get_config();
//xdebug_debug_zval('app');
//$l = &cii_is_loaded('Love');
//$start = microtime(true);
cii_run();
$app = &cii_get_instance();
$app->load->helper('date_helper');
//var_dump(cii_stringify_attributes(array("class"=>"red","href"=>"http://www.baidu.com","target"=>"_blank"),true));
var_dump(date_range('24234234', '453453434', TRUE, 'Y-m-d'));
//$app->load->helper('url_helper');
//var_dump(site_url(array('/sess_expiration/','/sdsdsf/','/242/')));
//var_dump($app->config->site_url(array('/sess_expiration/','/sdsdsf/','/242/')));
//var_dump($app->config->uri_string(array('/sess_expiration/','/sdsdsf/','/242/')));
//var_dump($app->config->site_url('sess_expiration','https'));
//var_dump($app->config->site_url(array('sess_expiration','sdsdsf','242'),NULL));
//var_dump(base_url(array('sess_expiration','sdsdsf','242')));
//var_dump($app->config->base_url(array('sess_expiration','sdsdsf','242')));
//var_dump(url_title("miaojuanfeng@qq.com"));

//var_dump($app->input->get());
//var_dump($app->input->post());
//var_dump($app->input->user_agent());
//var_dump($app->input->server());
//var_dump($app->uri->segment(1,'aa'));
//var_dump($app->uri->rsegment(2,'aa'));
//var_dump($app->log->write_log('InFo', 'aaaaaaaaaaaaaa'));
//var_dump($app->log->write_log('dasda', 'bbbbb'));
//$end = microtime(true);
//echo $end-$start;
//$l = &cii_is_loaded('Loader');
//$app = &cii_get_instance();
//$app->helper('array_helper');
//$data = array('name'=>'miaojuanfeng','content'=>'this is content');
//xdebug_debug_zval('app');
//var_dump($l);
//$app = &cii_get_instance();
//echo $app->benchmark->elapsed_time('total_execution_time_start','_____________________end');
//var_dump($app->lang->load('title_lang','chinese',1));
//echo $app->lang->line('key');
//$app->lang->load('title_lang');
//var_dump($app->hooks->call_hook('pre_controller'));
//$app->load->model('Sample');
//$app->output->append_output("miao");
//$app->output->append_output("juan");
//$app->output->append_output("feng");
//$data = array('name'=>'sss','content'=>'this is content');
//$app->load->view('index',$data);
//var_dump($app->output->final_output);
//$app->output->final_output;
//$app->output->display("miaojuanfeng123");
//$app->output->display();
//var_dump($app);
//$app->output->display("1231212");
/*
die();

cii_run();
//$app = &cii_get_config();
//var_dump($app);
//cii_run();
die();
	
	
$cfg = &cii_get_config();
xdebug_debug_zval('cfg');
	$app = &cii_get_instance();
	//echo $app->benchmark->elapsed_time('total_execution_time_start','123');
$cfg2 = &cii_get_config();
xdebug_debug_zval('cfg2');
	var_dump($app);
	echo $app->benchmark->memory_usage();
	die();
$l = &cii_is_loaded();
xdebug_debug_zval('l');
//die();
//$app->benchmark->mark('total_execution_time_start');
//$app->benchmark->mark('total_execution_time_end');
	//var_dump($app);
	//var_dump($app->lang);
	//var_dump($app->uri->rsegments[1]);
	//$class = &cii_load_class("Router");
	//var_dump($class);
//var_dump($app);
//$data = array('name'=>'miaojuanfeng','content'=>'this is content');
//$app->load->view('index', $data);
$app->lang->load('title_lang', "chinese");
$app->lang->load('content', "chinese");
//var_dump($app);
//var_dump($app->lang->line('key2'));
$app->load->model('sample');
//var_dump($app->sample->load);

//var_dump(get_class_methods('sample'));
//$app->benchmark->mark('asdasd');
var_dump($app->benchmark);

//$aaa = 1231231;
//$a = &$app->get_instance($aaa);
//xdebug_debug_zval('a');*/
//xdebug_debug_zval('a');
//$b = &cii_get_instance();
//echo $b->lang->line('key');
//var_dump($b);
	//$is_loaded = &cii_is_loaded();
	//xdebug_debug_zval('is_loaded');
	//$cii = new cii_controller();
	//var_dump($cii);*/
